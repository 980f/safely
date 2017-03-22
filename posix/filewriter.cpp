#include "filewriter.h"

#include "fcntlflags.h"
#include "fileinfo.h"

#include <cstdio>
#include "logger.h"

static Logger bug("FileWriter",true);

//bool FileWriter::onWrite(__ssize_t ret){
//  if(ret>=0){//then it is # of bytes transferred
//    ++blocksout;
//    sent+=ret;
//    buf.skip(ret);
//    action();

//    if(buf.freespace()){
//      return true;
//    } else {
//      onCompletion();
//      return false;
//    }
//  }
//  return false;
//}

//bool FileWriter::action(){
////  bug("Received: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
//  return false;
//}

//void FileWriter::onCompletion(){
////  bug("Completed: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
//}

FileWriter::FileWriter():
  fmover(false/*write*/,fd,buf)
{
  //#nada
}

FileWriter::~FileWriter()
{

}

bool FileWriter::process(TextKey fname, const ByteScanner &source){
  if(fd.open(fname,O_CREAT|O_WRONLY|O_TRUNC)){
    bug("Launching xfer of file %s",fname);
    buf=source;
    fmover.prepare(512);
    if(fmover.go()){
      return true;
    }
  }
  return false;
}

unsigned FileWriter::remaining() const {
  return buf.freespace();
}
