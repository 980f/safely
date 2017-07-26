#include "asciiframer.h"
#include "string.h" //strlen  strchr
#include "cheaptricks.h" //flagged
#include "hassettings.h" //for ordinalOfID()
//todo: use Char class to share the set of escapes, perhaps even integrate utf8

bool AsciiFramer::onReception(int octet) { //#called from an ISR
  if(octet > 0) { //nulls are ignored, so that they can be used as part of flushing a transmitter.
    if(octet == SOF) {
      flush();
      purgeUntilNewline=0;
      return true; //accepted
    }
    if(purgeUntilNewline){
      if(octet == EOL){
        purgeUntilNewline=0;
      }
      return true;//drop the newline that gets us back into phase.
    }
    //do crlf to lf translation, as well as lone cr to lf.
    if(octet=='\r'){
      cred=true; //multiple successive \r's become one. cr cr lf == lf
      return true;
    }
    if(flagged(cred)){//if previous was a \r that we didn't push into the fifo
      if(octet != EOL){//... and is not followed by lf
        if(fifo.push(EOL)){//push an lf
          ++eols;
          //don't return here, we still have that octet to insert.
        } else {
          //we lose the incoming char as there wasn't room for either the cr->lf or the incoming char
          return false;
        }
      }
    }

    if(fifo.push(octet)) {
      if(octet == EOL) {
        ++eols;
      }
      return true;
    } else {
      return false;
    }
  } else if(octet==-2){//receiver overrun
    flush();
    purgeUntilNewline=1;
    return true; //accepted
  } else  {
    return octet == 0; //nulls are 'ok' but don't go into buffer.
  }
} /* receive */

//will add escape chars as we discover the need, not necessarily all that C supports.
static const char escapes[] = "\\\r\n\t\x03";
static const char escapee[] = "\\rntc";

bool AsciiFramer:: getLine(CharScanner&p, bool deslash){
  if(eols > 0) {
    char ch = 0; //4debug
    while(p.hasNext() && fifo.hasNext()) {//NB: it is pathological to exit loop by this condition
      ch = fifo.next();
      if(ch == 8 || ch == 127) { //backspace and delete both treated like backspace
        p.unget(); //kill previous output
        continue; //kill backspace/del as well
      }
      if(ch=='\r'){//must escape CR's to not have them disappear.
        continue;
      }
      if(deslash) { //then process c-escape sequences.
        if(ch == '\\') {
          ch = fifo.peek();
          const int ordinal = strchr(escapee,ch)-escapee;
          if(ordinal>=0) { //then we found one
            ch = escapes[ordinal];
            fifo.next();//remove the second char of the \x
          } else {
            ch = '\\';
            //and following char is still in fifo.
          }
        }
      }
      if(ch == EOL) { //end of line gets converted to a null for end-of-string indication
        p.next() = 0;
        --eols;
        p.getHead(p);
        return true;
      }
      p.next() = ch;
    }
    //excessively long command, or other major fup.
    return false;
  } else {
    return false;
  }
} /* getLine */


bool AsciiFramer::stuff(const char *preloads){
  if(preloads) {
    char ch;
    while((ch = *preloads++) != 0) {
      if(!onReception(ch)) {
        return false; //ran out of room
      }
    }
  }
  return true;
} /* stuff */


AsciiFramer::AsciiFramer(char *storage, int sizeofstorage): fifo(storage,sizeofstorage){
  flush();
  purgeUntilNewline=1;
}

void AsciiFramer::flush(){
  eols = 0;
  cred=false;
  fifo.flush(false);
}

//end of file
