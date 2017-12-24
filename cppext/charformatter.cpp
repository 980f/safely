#include "charformatter.h"
#include "transactionalbuffer.h"

#include "ctype.h"
#include "limits.h"
#include "minimath.h" //for power
#include <cmath>  //floot, log10
//#include <cstdio> //snprintf
#include "string.h"
#include "numberparser.h"
#include "safely.h" //ascii framing characters
#include "cstr.h"

struct NumberParser : public PushedNumberParser  {

  /** @param buf points to terminator */
  bool parseNumber(CharFormatter &buf){
    while(buf.hasNext()) {
      if(next(buf.peek())){
        buf.skip(1);
      } else {
        break;
      }
    }
    return seemsOk();
  }

  double getValue(CharFormatter &buf, double backup = 0.0){
    if(parseNumber(buf)) {
      return lastParsed=packed();//track for debug
    } else {
      return backup;
    }
  }

};


CharFormatter::CharFormatter(char * content) : CharScanner(content, content ? strlen(content) : 0){
  //nada
}

CharFormatter::CharFormatter(char * content, unsigned size) : CharScanner(content, size){
  //nada
}

//this should clone or wrap the remaining part of 'other'
CharFormatter::CharFormatter(const Indexer<char> &other) : CharScanner(other,0){
  //nada
}

CharFormatter::CharFormatter(const Indexer<unsigned char> &other) : CharScanner( reinterpret_cast<char *>(other.internalBuffer()),other.used()){
  //nada
}

//this should clone or wrap the remaining part of 'other'
CharFormatter::CharFormatter(ByteScanner &other) : CharScanner(other,0){
  //nada
}

CharFormatter::CharFormatter(){

//#nada
}

Indexer<u8> CharFormatter::asBytes(){
  return Indexer<u8>(reinterpret_cast<u8*>(buffer),used());
}

//CharFormatter CharFormatter::wrap(Indexer<u8> raw){
//  return CharFormatter(reinterpret_cast<char *>(raw.internalBuffer()),raw.allocated());
//}

double CharFormatter::parseDouble(void){
  NumberParser n;
  //maydo: use transactional buffer? if so then have to return a status and pass a pointer.
  return n.getValue(*this);
} /* parseDouble */

int CharFormatter::parseInt(int def){
  s64 dry = parse64(def);
  if(dry> INT_MAX) {
    return INT_MAX;
  } else if (dry< INT_MIN) {
    return INT_MIN;
  } else {
    return int(dry);
  }
}

unsigned CharFormatter::parseUnsigned(unsigned def){
  s64 dry = parse64(def);
  if(dry> UINT_MAX) {
    return def;
  } else if (dry< 0) {
    return def;
  } else {
    return unsigned(dry);
  }
}

bool CharFormatter::move(int delta){
  if(delta==0) {
    return true;//successfully did nothing, as was requested.
  }
  unsigned target = pointer + delta;//data at pointer should move to target
  if(canContain(target)) {
    unsigned amount = freespace();//max that will move
    if(delta>0) {
      amount -= delta;//don't push data past end of allocation.
    }
    copyObject(buffer + pointer,buffer + target,amount);//promises to deal with overlap sanely
    pointer = target;
    return true;
  } else {
    return false;
  }
} // CharFormatter::move

bool CharFormatter::move(int delta, int keep){
  if(keep<=0) {
    return true;
  }
  if((delta + int(pointer))<0) {
    return false;
  }
  if(delta + pointer + keep>allocated()) {
    return false;
  }
  //move pointer through pointer+keep delta to the right (if positive)
  memmove(buffer + pointer + delta,buffer + pointer,keep);
  return true;
} // CharFormatter::move

s64 CharFormatter::parse64(s64 def){
  NumberParser n;

  if(n.parseNumber(*this)) {
    if(n.hasEterm) {//trying to tolerate some values, may produce nonsense.
      int logProduct = ilog10(n.predecimal) + n.pow10 + n.exponent;//??
      if(logProduct<=18) {
        n.predecimal = 0x7FFFFFFFFFFFFFFFLL;
      } else {
        n.predecimal *= pow10(int(n.exponent));//#cast needed for overload resolution
      }
    }
    return n.negative ? -n.predecimal : n.predecimal;
  } else {
    return def;
  }
} /* parse64 */

bool CharFormatter::printChar(char ch){
  if(hasNext()) {
    next() = ch;
    return true;
  } else {
    return false;
  }
}

bool CharFormatter::printChar(char ch, unsigned howMany){
  if(stillHas(howMany)) {//todo:0 if BadIndex fill to end
    while(howMany--> 0 && hasNext()) {
      next() = ch;
    }
    return true;
  } else {
    return false;
  }
}

bool CharFormatter::printAtWidth(unsigned int value, unsigned width, char padding){
  unsigned numDigits = value? ilog10(value)+1 : 1; //ilog10 gives -1 for zero, here we lump zero in with 1..9
  if(numDigits > width) {//if you cant fit the whole thing don't put any digits into the field.
     printChar('*', width);
    return false;
  }

  if(stillHas(width)) {
    printChar(padding, width-numDigits);
    while(numDigits--> 0) {
      printDigit(digitsAbove(value,numDigits));
    }
    return true;
  }
  return false;
} // CharFormatter::printAtWidth

bool CharFormatter::printDigit(unsigned digit){
  return printChar(digit + ((digit < 10) ? '0' : 'A' - 10)); //'A' - 10 so we can get a letter beginning with 'A' at 10, for hex
}

bool CharFormatter::printUnsigned(unsigned int value){
  if(value == 0) {//frequent case
    return printChar('0'); // simpler than dicking with the suppression of leading zeroes.
  }
  int numDigits = ilog10(value) + 1;
  if(stillHas(numDigits)) {//this doesn't include checking for room for a separator
    while(numDigits--> 0) {
      unsigned digit = revolutions(value,i32pow10(numDigits));
      printDigit(digit);
    }
    return true;
  } else {
    return false;
  }
} // CharFormatter::printUnsigned

bool CharFormatter::printUnsigned64(u64 value){
  if(value == 0) {
    return printChar('0');
  }
  int numDigits = ilog10(value) + 1;
  if(stillHas(numDigits)){//this doesn't include checking for room for a separator
    while(numDigits--> 0) {
      unsigned digit = revolutions(value,i64pow10(numDigits));
      printDigit(digit);
    }
    return true;
  } else {
    return false;
  }
} // CharFormatter::printUnsigned

//alh made return compatible with other methods of this class.
bool CharFormatter::printHex(unsigned value, unsigned width){
  if(stillHas(width)) {
    while(width--> 0) {
      printDigit((value >> (4 * width)) & 15); //pick out the nibble to print
    }
    return true;
  }
  return false;
}

bool CharFormatter::printSigned(int value){
  if(value < 0) {
    return printChar('-') & printUnsigned(unsigned(-value));
  } else {
    return printUnsigned(unsigned(value));
  }
}

const double _2gig = 2147483648.0;
//todo:1 const double _u63=

bool CharFormatter::printNumber(double d, int sigfig){
  if(d == 0.0) { //to avert taking the log of 0. and frequent value.
    printChar('0');
    return true;
  }
  if(isNan(d)) {
    return printString("+NaN");//we need a sign in various usages to even call this guy.
  }
  if(isSignal(d)) {
    return printString("+Inf");//we need a sign in various usages to even call this guy.
  }
  TransactionalBuffer<char > checker(*this);
  if(d < 0) {//print optional sign
    checker &= printChar('-');
    d = -d;
  }
  double dint = floor(d);//print integer part of value
  bool is32 = (d == dint && d < _2gig);//todo:1 much better detection of fixed point versus scientific format.
  if(is32) {//try to preserve integers that were converted to double.
    checker &= printUnsigned(u32(d));
  } else {
    double logd = log10(d);
    int div = 1 + logd - sigfig;
    if(div>0) {//more digits than we need, divide by a power of 10 and add an E expression.
      if(sigfig>9) {
        //need to maybe reduce the number and have more trailing zeroes.
        div += sigfig - 9;
      }
      d /= pow10(div);
      checker &= printUnsigned(u32(d));
      if(div>3) {
        checker &= printChar('E');
        checker &= printUnsigned(u32(div));
      } else {
        while(div-->0) {
          checker &= printChar('0');
        }
      }
    } else if(div==0) { //exact number of desired digits to left of .
      if(sigfig<=9) {
        checker &= printUnsigned(u32(d));
      } else {
        //todo:1 print first 9 digits then a decimal point then struggle.
        //struggle: add 10^excess, print that then replace the leading 1 with a '.'
      }
    } else { // 'div' decimals will be needed
      if(dint>0) { //we have some to the left of the dp
        checker &= printUnsigned(u32(dint));
        d -= dint;
        logd = log10(d);
      } else {
        checker &= printChar('0');
      }
      checker &= printChar('.');
      int numzeros = -logd;
      if(-logd==floor(-logd)) {
        --numzeros;
      }
      if(numzeros + sigfig>18) {//18 digit limit in parser
        checker &= printChar('0');
        //ridiculously small, blow it off or add a useless E expression.
      } else {
        d *= pow10(-div); //if d>_2gig we will lose significant digits.
        while(numzeros-->0) {
          checker &= printChar('0');
        }
        checker &= printUnsigned(u32(d));
        //todo:2 trim trailing zeroes here stop at DP  and if you see that add one back on.
      }
    }
  }
  return checker.commit();
} // CharFormatter::printNumber

bool CharFormatter::printNumber(double d, const NumberFormat &nf, bool addone){
  //first: round!
  if(d!=0.0) {
    u64 lsd = i64pow10(nf.decimals);
    d += 0.5 / lsd;
    //and now we can truncate later on
  }
  TransactionalBuffer<char > checker(*this);
  NumberPieces np(d); //decompose number into digits and field sizes.
  if(np.isZero) { //to avert taking the log of 0. and frequent value.
    checker &= printChar('0');
  } else if(np.isNan) {
    checker &= printString(nf.showsign ? "+NaN" : "NaN");
  } else if(np.isInf) {
    return checker &= printString(np.negative ? "-Inf" : nf.showsign ? "+Inf" : "Inf");
  } else {
    if(nf.scientific) {
      checker &= printDecimals(d,nf.decimals + addone);
    } else {
      if(nf.showsign && !np.negative) {
        checker &= printChar('+');
      }
      if(np.negative) {
        checker &= printChar('-');
      }
      checker &= printUnsigned64(np.predecimal);
      if(nf.decimals>0) {//if we want radix and digits
        checker &= printChar('.');//not doing locale's herein.
        unsigned stillwant = nf.decimals + addone;
        if(np.postdecimal==0||np.postDigits==0) {//all zeros after radix, frequent case, when number was actually an integer
          checker &= printChar('0',stillwant);
          //and we are done
        } else {
          //some of postdecimal's digits are zeroes.
          unsigned digitsPresent=1+ilog10(np.postdecimal);
          auto numZeroes=np.postDigits-digitsPresent;
          if(numZeroes>=stillwant){
             checker &= printChar('0',stillwant);
             //and we are done
          } else {
             checker &= printChar('0',numZeroes);
             stillwant-=numZeroes;

             if(digitsPresent>=stillwant) {//have more than desired
               u64 postdec = truncateDecimals(np.postdecimal,stillwant);
               checker &= printUnsigned64(postdec);
             } else {//neeed a few tailing zeroes
               checker &= printUnsigned64(np.postdecimal);
               //if fixed width ... which we have presumed before here.
               checker &= printChar('0',stillwant -digitsPresent);
             }
          }
        }
      }
    }
  }
  return checker.commit();
} /* printNumber */

bool CharFormatter::printDecimals(double d, unsigned decimals){
  NumberFormat nf;
  nf.decimals=decimals;
  return printNumber(d,nf,false);
} /* printNumber */

bool CharFormatter::printString(TextKey s){
  TransactionalBuffer<char > checker(*this);
  if(s) {
    while(*s && printChar(*s++)) {
      //empty body
    }
  }
  return checker.commit();
}

int CharFormatter::cmp(const CharScanner&other) const {
  CharScanner me(*this); //this constructor gives us a pointer to the used part of the argument
  CharScanner him(other);
  while(me.hasNext()) {
    char mine = me.next();
    if(him.hasNext()) {
      char his = him.next();
      if(mine != his) {
        return mine < his;
      }
      //else continue
    } else {
      //same except I am longer so I am after other
      return +1;
    }
  }
  if(him.hasNext()) {
    return -1; //same except other is longer.
  } else {
    return 0; //same bytes, same length.
  }
} /* cmp */


CharFormatter CharFormatter::infer(char *content){
  Cstr wrap(content);
  return CharFormatter(wrap.violated(),wrap.length());//#does not include the null.
}
