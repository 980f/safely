#include "jsonstore.h"
#include <charscanner.h>
#include "bitwise.h"

//only thing needed from Index.h, and using new name for it "Not a Value":
#define NaV ~0U
#include <ostream>

const char *JsonStore::Lexer::separator(":,{}[]"); //maydo: bare newline is a comma.const char
#define CEscape(ch) replace(pointer - 1, 2, 1, ch )  //may be off by one

/** consumes bytes from string to get a hex constant. Bytes are NOT removed.
 * *@param pointer points to first char after last hex char. read code for failure cases.
 */
uint32_t parseHex(unsigned pointer, const std::string&image, unsigned numDigits){
  if(pointer + numDigits <= image.length()) {
    uint32_t packed(0);
    for(unsigned n = numDigits; n-- > 0; ) {
      packed <<= 4;
      auto c = static_cast<unsigned char> (image.at(pointer++));
      if(c >= '0' && c <= '9') {
        packed |= c - '0';
      } else if(c >= 'A' && c <= 'F') {
        packed |= c - 'A' + 10;
      } else if(c >= 'a' && c <= 'f') {
        packed |= c - 'a' + 10;
      } else {
        return NaV; //bad hex constant!  No biscuit!
      }
    }
    return packed;
  }
  return NaV; 
} // parseHex

/** in=place conversion of escape sequences to utf8 sequences */
void replaceUEscape(std::string&tokenImage, unsigned&pointer, unsigned numDigits){
  uint32_t packed(parseHex(pointer, tokenImage, numDigits));

  if(packed != NaV) { //now expand packed to utf8:
    pointer -= 2;
    tokenImage.erase(pointer, numDigits + 2); //erase sequence

    unsigned more = UTF8::numFollowers(packed);
    if(more > 0) {
      //todo:M can be made slicker with a local char array filled then inserted.
      tokenImage.insert(pointer++, 1, UTF8::firstByte(packed, more));
      while(more-- > 0) {
        tokenImage.insert(pointer++, 1, UTF8::nextByte(packed, more));
      }
    } else { //is ascii char
      tokenImage.insert(pointer++, 1, packed);
    }
  } else {
    ++pointer;  //skip past '\'
  }
} // replaceUEscape

void processEscapes(std::string&tokenImage){
  unsigned pointer(0); //using int as std::string::iterators are painful and incomplete.

  while(pointer < tokenImage.length()) { //NB: length will somtimes change inside loop
    UTF8 utf8(tokenImage.at(pointer++)); //increment here for safety, to avert infinite loop on coding error.
    if(utf8 == '\\') {
      switch(tokenImage.at(pointer)) { //one past slash
      case '\\':
      case '\'': //some people just like escaping any kind of quote char.
      case '"':
        //keep char at pointer
        tokenImage.erase(pointer - 1); //erase slash, pointer now points to char past escaped one
        break;
      case 'b':
        tokenImage.CEscape('\b');
        break;
      case 'f':
        tokenImage.CEscape('\f');
        break;
      case 'n':
        tokenImage.CEscape('\n');
        break;
      case 'r':
        tokenImage.CEscape('\r');
        break;
      case 't':
        tokenImage.CEscape('\t');
        break;
      case 'u': {
        replaceUEscape(tokenImage, ++pointer, 4);
      }
      break;
      case 'U': { //same as little u but with 8 chars
        //          uint32_t packed(parseHex(pointer,tokenImage,8));
        //todo:0 see little u and do the same thing here
      }
      break;
      } // switch
    }
  }
} // processEscapes

JsonStore::Lexer::Lexer(JsonStore::Parser&parser) : parser(parser){
  start();
}

void JsonStore::Lexer::start(){
  escaped = false;
  accumulator.clear();
  state = PreWhite;
}

JsonStore::JsonStore(Storable&rootNode) : parser(rootNode), lexer(parser){
}

bool JsonStore::parse(ByteScanner&is){
  while(is.hasNext()) {
    lexer.accept(is.next());
  }
  return true;
}

void JsonStore::Lexer::accept(char ch){
  UTF8 utf8(ch); //char type analyzer

  switch(state) {
  case PreWhite:
    if(utf8.isWhite()) {
      //do nothing
    } else if(utf8 == '"') {
      state = InQuote;
    } else if(utf8.in(separator)) {
      parser.acceptToken(accumulator, utf8);
      start();
    } else {
      accumulator += ch; //1st char of token
      escaped = ch == '\\';
      state = InText;
    }
    break;
  case InQuote:
    if(flagged(escaped)) {
      accumulator += ch;
    } else if(utf8 == '"') {
      state = PostWhite;
    } else {
      accumulator += ch;
      escaped = ch == '\\';
    }
    break;
  case InText: //looking for word separator char, in case of no trailing whitesapce
    if(flagged(escaped)) {
      accumulator += ch;
    } else if(utf8.in(separator)) {
      parser.acceptToken(accumulator, utf8);
      start();
    } else if(utf8.isWhite()) {
      state = PostWhite;
    } else {
      accumulator += ch;
      escaped = ch == '\\';
    }
    break;
  case PostWhite:
    if(utf8.isWhite()) {
      //be happy
    } else {
      parser.acceptToken(accumulator, utf8);
      start();
      //      accumulator += ch;
      //      escaped=ch=='\\';
    }
    break;
  } // switch
} // accept

JsonStore::Parser::Parser(Storable&rootNode) : notStarted(true), node(&rootNode){
  //#nada
}

void JsonStore::Parser::addNode(std::string&tokenImage){
  processEscapes(tokenImage);
  Storable *child(node.findChild(name, true/*create*/));
  if(child){
    child->setImage(tokenImage.c_str(), Storable::Parsed);
  }
}

bool JsonStore::Parser::acceptToken(std::string &tokenImage, UTF8 term) {
  //if term is not a separator then we are somewhat lost.
  if (node.empty()) {
    return false;
  }
  switch(term.raw) {
  case ':':
    name = tokenImage.c_str();
    return true;
  case ','://assign value
    if(tokenImage.length() != 0) {
      addNode(tokenImage);
    }
    name.clear();
    break;
  case '{'://'[' has been replaced with '{' when we get here.
    if(flagged(notStarted)) {
      if(!name.empty()) {
//todo:1 why did we think we could rename the present node?        node->setName(name);
      }
    } else {
      node.pushChild(name, true); //push the "node stack"
      node.setType(Storable::Wad);
//      node->setQuality(Storable::Parsed);//this should be intrinsic in the addChild of findChild
    }
    name.clear();
    break;
  case '}'://assign and pop
    if(tokenImage.length() != 0) { //final value of a wad
      addNode(tokenImage);
    }
    node.pop();
    name.clear();
    break;
  } // switch
  return true;
} // acceptToken

///////////////////////


JsonStore::Printer::Printer(Storable&node, std::ostream&os) : tablevel(0), os(os), node(&node){
}

void JsonStore::Printer::indent(){
  for(unsigned tabs = tablevel; tabs-- > 0; ) {
    os << '\t'; //or we could use spaces
  }
}

//this macro did not compile.
#define CSLASH( slasher )  case '\## slasher ##': os << # slasher; continue

void JsonStore::Printer::printText(const char *p, bool forceQuote){
  if(forceQuote) {
    os << '"';
  }
  UTF8 c;
  while((c = *p++)) {
    switch(char(c)) {
    case '\t': os << "\\t"; continue;
    case '\r': os << "\\r"; continue;
    case '\n': os << "\\n"; continue;
    //more C slashes later, if we can get a macro to work.
    case '"':  os << "\\\""; continue;
    case '\\': os << "\\\\"; continue;

    }
    if(c.isMultibyte()) {
      if(0) {//1:suppress generating unicode escapes
        os << c;
        for(int i = c.numFollowers(); i >0; --i) {
          os << *p++;
        }
        continue;
      }
      uint32_t packer(0);
      //need to make a unicode point, then expand it?
      int numf(c.numFollowers());
      unsigned mask = fieldMask(6 - numf);
      mask &= c.raw;
      packer |= mask; //pick bits from utf lead byte
      while(numf-- > 0) {
        if(c==0) {
          if(forceQuote) {
            os << '"';
          }
          return;  //this is hopeless.  Survive.
        }
        c = *p++;
        if(!c.isMultibyte()) {
          break; //invalid sequence
        }
        packer <<= 6;
        packer |= c & fieldMask(6);
      }
      if(numf) {
        //string ended early or invalid sequence
      }
      if(packer < 65536) {
        os << "\\u";
        for(int digit = 4; digit-- > 0; ) {
          int nibble = (packer >> (digit * 4)) & 0xF;
          os << static_cast<char>((nibble < 10) ? ('0' + nibble) : ('A' - 10 + nibble));
        }
      } else {
        os << "\\U";
        for(int digit = 8; digit-- > 0; ) {
          int nibble = (packer >> (digit * 4)) & 0xF;
          os << static_cast<char>((nibble < 10) ? ('0' + nibble) : ('A' - 10 + nibble));
        }
      }
    } else {
      os << c.raw;
    }
  }
  if(forceQuote) {
    os << '"';
  }
} // printText

bool JsonStore::Printer::printName(){
  indent();
  if (node.p->name.empty()) {
    return false;
  } else {
    os << '"';
    os << node.p->name;
    os << '"';
    //printText(node->name, true); //forcing quotes here, may not need to
    os << ':';
    return true;
  }
} // JsonStore::Printer::printName

void JsonStore::Printer::printWad(){
  os << '{' << std::endl;
  ++tablevel;
  ChainScanner<Storable> scanner(node.p->kinder());
  while(scanner.hasNext()) {
    node.p = &scanner.next();
    if(printValue()) { //if node actual was emitted into the output stream
      if(scanner.hasNext()) {
        os << ',';
      }
      os << std::endl;
    }
  }
  node.pop();
  --tablevel;
  indent();
  os << '}';
} /* printWad */

bool JsonStore::Printer::printValue(){
  if (node.empty()) { //COA
    return false;
  }
  if (node.p->isTrivial()) { //drop autocreated but unused nodes.
    return false;
  }
  node.p->preSave(); //when not trivial this flushes cached representations

  switch (node.p->getType()) {
  default:
  case Storable::Type::Uncertain:
    if(printName()) {
      os << "!Unknown"; //fix code whenever you see one of these
    } else {
      return false;
    }
    //else the node disappears
    break;

  case Storable::Numerical: {
    printName();
    double number = node.p->getNumber<double>();
    //todo:1 output nans as keyword
    if(int(number) == number) {
      os << int(number); //makes small numbers more readable.
    } else if(int64_t(number) == number) {
      os << int64_t(number);
    } else {
      os << number; //high precision must be set by the creator of the os.
    }
  }
  break;

  case Storable::Textual:
    printName();
    printText(node.p->image().c_str(), true); //always quote
    break;
  case Storable::Wad:
    if (node.p->numChildren() > 0) { //suppressing empty wads to get rid of trivial "trailing comma" false alarms.
      printName();
      printWad();
    }
    break;
  } /* switch */
  return true;
} /* printValue */
