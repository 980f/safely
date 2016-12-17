#include "jsonstore.h"
#include "logger.h"


const char *JsonStore::Lexer::separator(":,{}[]"); //maydo: bare newline is a comma.const char
#define CEscape(ch) replace(pointer - 1, 2, 1, ch )  //may be off by one

/** consumes bytes from string to get a hex constant. Bytes are NOT removed.
 * *@param pointer points to first char after last hex char. read code for failure cases.
 */
u32 parseHex(unsigned pointer, const std::string&image, int numDigits){
  if(pointer + numDigits <= image.length()) {
    u32 packed(0);
    for(int n = numDigits; n-- > 0; ) {
      packed <<= 4;
      char c = image.at(pointer++);
      if(c >= '0' && c <= '9') {
        packed |= c - '0';
      } else if(c >= 'A' && c <= 'F') {
        packed |= c - 'A' + 10;
      } else if(c >= 'a' && c <= 'f') {
        packed |= c - 'a' + 10;
      } else {
        return u32(-1); //bad hex constant!  No biscuit!
      }
    }
    return packed;
  }
  return u32(-1); //all ones
} // parseHex

/** in=place conversion of escape sequences to utf8 sequences */
void replaceUEscape(std::string&tokenImage, unsigned&pointer, int numDigits){
  u32 packed(parseHex(pointer, tokenImage, numDigits));

  if(packed != u32(-1)) { //now expand packed to utf8:
    pointer -= 2;
    tokenImage.erase(pointer, numDigits + 2); //erase sequence

    int more = UTF8::numFollowers(packed);
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
  unsigned pointer(0); //using int as std::strin::iterators are painful and incomplete.

  while(pointer < tokenImage.length()) { //NB: length will somtimes change inside loop
    UTF8 utf8(tokenImage.at(pointer++)); //increment here for safety, to avert infinite loop on coding error.
    if(utf8 == '\\') {
      switch(tokenImage.at(pointer)) { //one past slash
      case '\\':
      case '\'': //some poeple just like escaping any kind of quote char.
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
        //          u32 packed(parseHex(pointer,tokenImage,8));
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
  Storable& child(node->addChild(name));
  child.setImage(tokenImage, Storable::Parsed);
  child.convertToNumber(true);
}

bool JsonStore::Parser::acceptToken(std::string&tokenImage, UTF8 term){
  //if term is not a separator then we are somewhat lost.
  if(node == nullptr) {
    return false;
  }
  switch(term.raw) {
  case ':':
    name = tokenImage.c_str();
    break;
  case ',':
    if(tokenImage.length() != 0) {
      addNode(tokenImage);
    }
    name.clear();
    break;
  case '{':
    if(flagged(notStarted)) {
      if(!name.empty()) {
        node->setName(name);
      }
    } else {
      node = &(node->addChild(name)); //push the "node stack"
      node->setQuality(Storable::Parsed);
    }
    name.clear();
    break;
  case '}':
    if(tokenImage.length() != 0) { //final value of a wad
      addNode(tokenImage);
    }
    node = node->parent;  //pop the "node stack"
    name.clear();
    break;
  } // switch
  return true;

  //if value (comma or end brace) then NOW we process \ escapes.
  //and then create a child with the name ...
  // ... and processed value.
  //maydo: recognize the the text is that of a number and make the node a number here. If not then Storable itself will need to do the conversion when someone access a
  // Text number with a number accessor. Legacy says we should do it here.
} // acceptToken

///////////////////////


JsonStore::Printer::Printer(Storable&node, std::ostream&os) : tablevel(0), os(os), node(&node){
}

void JsonStore::Printer::indent(){
  for(int tabs = tablevel; tabs-- > 0; ) {
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
  while(c = *p++) {
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
      u32 packer(0);
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
  if(node->name.empty()) {
    return false;
  } else {
    os << '"';
    os << node->name;
    os << '"';
    //printText(node->name, true); //forcing quotes here, may not need to
    os << ':';
    return true;
  }
} // JsonStore::Printer::printName

void JsonStore::Printer::printWad(){
  os << '{' << std::endl;
  ++tablevel;
  ChainScanner<Storable> scanner(node->kinder());
  while(scanner.hasNext()) {
    node = &scanner.next();
    if(printValue()) { //if node actual was emitted into the output stream
      if(scanner.hasNext()) {
        os << ',';
      }
      os << std::endl;
    }
  }
  node = node->parent;
  --tablevel;
  indent();
  os << '}';
} /* printWad */

bool JsonStore::Printer::printValue(){
  if(!node) { //COA
    return false;
  }
  if(node->isTrivial()) { //drop autocreated but unused nodes.
    return false;
  }
  node->preSave(); //when not trivial this flushes cached representations

  switch(node->getType()) {
  default:
  case Storable::NotKnown:
    if(printName()) {
      os << "!Unknown"; //fix code whenever you see one of these
    } else {
      return false;
    }
    //else the node disappears
    break;

  case Storable::Numerical: {
    printName();
    double number = node->getNumber<double>();
    //todo:1 output nans as keyword
    if(int(number) == number) {
      os << int(number); //makes small numbers more readable.
    } else if(s64(number) == number) {
      os << s64(number);
    } else {
      os << number; //high precision must be set by the creator of the os.
    }
  }
  break;

  case Storable::Textual:
    printName();
    printText(node->image().c_str(), true); //always quote
    break;
  case Storable::Wad:
    if(node->numChildren() > 0) { //suppressing empty wads to get rid of trivial "trailing comma" false alarms.
      printName();
      printWad();
    }
    break;
  } /* switch */
  return true;
} /* printValue */

#if 0

void parseEscapedString(Ustring&utf8, CharScanner&encoded){
  while(encoded.hasNext()) {
    char c = encoded.next();
    if(c == '\\') {
      if(!is.good()) {
        return token.parseError(is);
      }
      c = is.get();
      switch(c) {
      default:
        token.string += '\\'; //without this swallows slash of unknown escape sequence
        token.string += c;
        break;
      case '\\':
      case '\'': //some poeple just like escaping any kind of quote char.
      case '"':
        token.string += c; //escaped quote
        break;
      case 'b':
        token.string += '\b';
        break;
      case 'f':
        token.string += '\f';
        break;
      case 'n':
        token.string += '\n';
        break;
      case 'r':
        token.string += '\r';
        break;
      case 't':
        token.string += '\t';
        break;
      case 'u':
        Unichar uc = 0;
        for(int i = 0; i < 4; i++) {
          uc <<= 4;
          if(!is.good()) {
            return token.parseError(is);
          }
          c = is.get();
          if(c >= '0' && c <= '9') {
            uc |= c - '0';
          } else if(c >= 'A' && c <= 'F') {
            uc |= c - 'A' + 10;
          } else if(c >= 'a' && c <= 'f') {
            uc |= c - 'a' + 10;
          } else {
            return token.parseError(is);
          }
        }
        utf8 += uc;
      } // switch
    } else {
      utf8 += c;
    }
  }
} // parseEscapedString

JsonStore::JsonStore(){
}

/** @returns next non-white character, modifying @param is to point past it.*/
static UTF8 findNext(Sequence<char>&is){
  while(is.hasNext()) {
    UTF8 ch = is.next();
    if(!ch.isWhite()) {
      return ch; //normal exit
    }
  }
  return 0; //error
}

UTF8 JsonStore::parseValue(Storable&parent, LatentSequence<char>&is){
  UTF8 ch = findNext(is);

  switch(int(ch)) {
  case '"': //standard quote
    ch = parseText(text, is, ch); // the keyname ends up in the this->text
    if(ch == ':') { //is name not value
      name = text;
      text.restore();
      return parseValue(is); //cheap loop
    }
    type = Textual;
    if(ch) {
      q = Parsed;
    }
    return ch;
  //todo:3 case '<': embedded file, seek ending '>' and then open file and parse it.

  case '}':
    //could perhaps deal with nominally illegal trailing comma here instead of where it is now.
    return ch;
  case '{': {
    parent.addChild()
  }
    type = Wad;
    return parseWad(is);

  case ',':
    if(name.isBlank()) { //fixing legacy trash, isolated comma
      return parseValue(is); //cheap loopback, trying to make this field disappear
    } else {
      //empty content
      type = NotKnown;
      q = Parsed;
      return ch;
    }
    break;
  default:
    text.restore();
    text.next() = ch;
    ch = parseText(text, is, 0); //extract until next whitespace
    NumberParser n;
    CharScanner safecopy(text);
    if(n.parseNumber(safecopy)) {
      //todo:3 could detect apparent integerness here
      parsedNumber(n.packed());
      return cleanEOL(ch, is);
    }
#define isToken(cc) (same(token, cc))
    const char *token = text.asciiz();
    if(isToken("nan")) {
      parsedNumber(Nan);
      return cleanEOL(ch, is);
    }
    if(isToken("true")) { //@untested
      parseLogical(true);
      return cleanEOL(ch, is);
    }
    if(isToken("false")) { //@untested
      parseLogical(false);
      return cleanEOL(ch, is);
    }
    if(isToken("null")) { //@untested
      q = Empty;
      return cleanEOL(ch, is);
    }
    if(isToken("!Unknown")) { //@untested
      q = Errorneous;
      return cleanEOL(ch, is);
    }
    {
      return cleanEOL(0, is); //attempt resync.
    }
  } /* switch */
} /* parseValue */

#endif // if 0

#if 0


UTF8 Storable::findNext(Sequence<char>&is){
  while(is.hasNext()) {
    UTF8 ch = is.next();
    if(!ch.isWhite()) {
      return ch; //normal exit
    }
  }
  return 0; //error
}

UTF8 Storable::parseWad(LatentSequence<char>&is){
  UTF8 term;

  type = Wad;
  q = Parsed;
  //  dbg(" ParseWad \"%s\":", getName());
  while(1) {
    Storable&newone(addChild());
    term = newone.parseValue(is); //recurses here
    if(term == ',') {
      continue;
    } else if(term == '}') {
      //      dbg(" EndWad: \"%s\"", getName());
      //if there was a trailing comma we have a useless node 'newone'
      if(newone.name.isBlank() && newone.q == Empty) {
        dbg("Storable: trailing comma on last value of %s", getName());
        removeChild(newone);
      }
      return findNext(is);
    } else { //horrible error has occured
      //maydo: attach child marked errorneous and try to continue parse
      dbg("bad value separator @%s=%g / %s ", newone.getName(), newone.number, newone.text.asciiz()); //#don't use image() here, it alters the type
      newone.q = Errorneous;
      //burn to end of line? pretend a comma was seen?
      return findNext(is); //try to proceed
    }
  }
  dbg(" Can't get here.");
  return 0; //horrible error
} /* parseWad */

//not called until after a leading '"' has been scarfed up.
UTF8 Storable::parseText(CharScanner&text, Sequence<char>&is, char endQuote){
  UTF8 ch;

  while(is.hasNext()) {
    ch = is.next();
    if(ch.isMultibyte()) {
      text.next() = ch;
      for(int skip = ch.numFollowers(); skip-- > 0; ) {
        text.next() = is.next();
      }
    } else if(ch == '\\') {
      if(!is.hasNext()) {
        return 0;
      }
      ch = is.next();
      //lots to do here:
      switch(int(ch)) {
      case 't':
        ch = '\t';
        break;
      } /* switch */
      text.next() = ch;
    } else if(endQuote && (ch == endQuote)) {
      text.next() = 0; //add null terminator for sake of easy printing
      return findNext(is); //swallow terminator and following whitesapce
    } else if(!endQuote && !ch.numAlpha()) { //not a keyword char
      text.next() = 0; //add null terminator for sake of easy printing
      return ch; //don't swallow term.
    } else {
      text.next() = ch;
    }
  }
  return 0;
} /* parseText */

void Storable::parsedNumber(double d){
  number = d;
  type = Numerical;
  q = Parsed;
}

void Storable::parseLogical(bool truth){
  //      bit=truth;
  number = truth ? 1.0 : 0.0;
  type = Bitten;
  q = Parsed;
} /* parseLogical */

//returns char that stopped the parsing
UTF8 Storable::parseValue(LatentSequence<char>&is){
  UTF8 ch = findNext(is);

  switch(int(ch)) {
  //  case '#':
  //    if(ch=='#'){//for now we lose comments, but at least can annotate the master copy.
  //      //it would be nice to have a comment member and tack the characters being tossed here to that string.
  //      commentary.append(1,char(ch));
  //      while(is.hasNext()) {
  //        ch = is.next();
  //        if(ch=='\n'){
  //          break;
  //        }
  //        commentary.append(1,char(ch));//todo:2 allow unicode in comments
  //      }
  //      continue;
  //    }
  //    break;
  case '}':
    //could perhaps deal with nominally illegal trailing comma here instead of where it is now.
    return ch;
  case '{':
    type = Wad;
    return parseWad(is);
  case '"': //standard quote
    ch = parseText(text, is, ch); // the keyname ends up in the this->text
    if(ch == ':') { //is name not value
      name = text;
      text.restore();
      return parseValue(is); //cheap loop
    }
    type = Textual;
    if(ch) {
      q = Parsed;
    }
    return ch;
  //todo:3 case '<': embedded file, seek ending '>' and then open file and parse it.
  case ',':
    if(name.isBlank()) { //fixing legacy trash, isolated comma
      return parseValue(is); //cheap loopback, trying to make this field disappear
    } else {
      //empty content
      type = NotKnown;
      q = Parsed;
      return ch;
    }
    break;
  default:
    text.restore();
    text.next() = ch;
    ch = parseText(text, is, 0); //extract until next whitespace
    NumberParser n;
    CharScanner safecopy(text);
    if(n.parseNumber(safecopy)) {
      //todo:3 could detect apparent integerness here
      parsedNumber(n.packed());
      return cleanEOL(ch, is);
    }
#define isToken(cc) (same(token, cc))
    const char *token = text.asciiz();
    if(isToken("nan")) {
      parsedNumber(Nan);
      return cleanEOL(ch, is);
    }
    if(isToken("true")) { //@untested
      parseLogical(true);
      return cleanEOL(ch, is);
    }
    if(isToken("false")) { //@untested
      parseLogical(false);
      return cleanEOL(ch, is);
    }
    if(isToken("null")) { //@untested
      q = Empty;
      return cleanEOL(ch, is);
    }
    if(isToken("!Unknown")) { //@untested
      q = Errorneous;
      return cleanEOL(ch, is);
    }
    {
      return cleanEOL(0, is); //attempt resync.
    }
  } /* switch */
} /* parseValue */

UTF8 Storable::cleanEOL(UTF8 ch, LatentSequence<char>&is){
  if(ch.isWhite()) {
    ch = findNext(is); //stifle false alarm on last member of block.
  }
  return ch;
}

static void tabit(int tablevel, std::ostream&os){
  for(int tabs = tablevel; tabs-- > 0; ) {
    os << '\t';
  }
}

#define INDENT tabit(tablevel, os);

void Storable::printText(std::ostream&os, CharFormatter&textish){
  os << '"';
  for(const char *p = textish.asciiz(); *p; ++p) {
    switch(*p) {
    case '\t':
      os << "\t";
      break;
    case '\\':
    case '\"':
      os << '\\';
    // fallthrough
    default:
      os << *p;
      break;
    } // switch
  }
  os << '"';
} // printText

//writers:
bool Storable::printWad(int tablevel, std::ostream&os){
  List scanner = children();

  os << '{' << std::endl;
  ++tablevel;
  while(scanner.hasNext()) {
    Storable&child = scanner.next();
    if(child.printValue(tablevel, os)) {
      if(scanner.hasNext()) {
        os << ',';
      }
      os << std::endl;
    }
  }
  --tablevel;
  INDENT;
  os << '}';
  return true;
} /* printWad */

bool Storable::printValue(int tablevel, std::ostream&os){
  if(isVolatile) {
    return false; //didn't print something used as a temp node.
  }
  if(q == Empty) { //drop autocreated but unused nodes.
    return false;
  }
  if(dropDefaults && q == Defaulted) {
    return true;
  }
  if(q == Errorneous) { //drop malformed nodes.
    //keep them , we are erroneously marking nodes as erroneous.  return false;
  }
  /*bool lastOk =*/ monitor.emitNow(CAST_TO_GPOINTER(Presave));
  //todo:2 figure out how to add an 'anding' accumulator to the monitor, that returns true on no-invocations.
  //if(!lastOk) {
  //false alarm due to no-one listening dbg("monitor is unhappy presaving %s",getName());
  //}
  INDENT;
  //        if(child.commentary.length()>0){
  //          os<<child.commentary;//includes newline, but not indent
  //          INDENT;
  //        }
  bool isNamed = !name.isBlank();

  switch(type) {
  default:
  case NotKnown:
    if(isNamed) {
      printText(os, name);
      os << ':';
      os << "!Unknown"; //fix code whenever you see one of these
    }
    break;
  case Bitten:
  case Numerical:
    if(isNamed) {
      printText(os, name);
      os << ':';
    }
    //todo:1 output nans as keyword
    if(int(number) == number) {
      os << int(number); //makes small numbers more readable.
    } else if(s64(number) == number) {
      os << s64(number);
    } else {
      os << number; //high precision is set in treeFile
    }
    break;
  case Textual:
    if(isNamed) {
      printText(os, name);
      os << ':';
    }
    printText(os, text);
    break;
  case Wad:
    if(numChildren() > 0) { //suprresing empty wads to get rid of trivial "trailing comma" false alarms.
      if(isNamed) {
        printText(os, name);
        os << ':';
      }
      return printWad(tablevel, os /*, true*/);
    } else {
      return false;
    }
    break;
  } /* switch */
  return true;
} /* printValue */

/////////////////

char TreeFile::parseTreeFile(){
  fstream fs(filename, fstream::in);

  if(fs) {
    TextInputStream tis(fs);
    return root.parseValue(tis);
  } else {
    dbg("Cannot open file <%s>.", filename);
    return 0;
  }
} /* parseTreeFile */

bool TreeFile::printTree(){
  fstream fs(filename, fstream::out);

  if(fs) {
    fs.setf(ios_base::scientific);
    fs.precision(17); //without this floating point values can erode as storage is cycled.
    root.printValue(0, fs /*, true*/); //true may give empty name if the tree is top level...
    fs << std::endl; //newline at end of file as a courtesy.
    return true;
  } else {
    dbg("Cannot open file <%s>.", filename);
    return false;
  }
} /* printTree */

#endif // if 0
