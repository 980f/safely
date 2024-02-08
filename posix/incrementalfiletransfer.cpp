//"(C) Andrew L. Heilveil, 2017,2024"
#include "incrementalfiletransfer.h"
#include "logger.h"

SafeLogger(incrementalFile,true);

IncrementalFileTransfer::IncrementalFileTransfer(bool reader, Fildes &fd, ByteScanner &buf):PosixWrapper ("IFT"),
  amReader(reader),
  fd(fd),
  buf(buf){
  //#nada
}


void IncrementalFileTransfer::prepare(unsigned amount){
  fd.setBlocking(false);//don't trust callers.
  transferred=0;
  blockstransferred=0;
  if(amReader){
    expected=amount;
    blocksexpected=quanta(expected,buf.allocated());
  } else {
    expected=buf.allocated();
    blocksexpected=quanta(expected,amount);
  }
}

bool IncrementalFileTransfer::onChunk(__ssize_t amount){
  if(amount>=0){//then it is # of bytes transferred
    ++blockstransferred;
    transferred+=amount;
    buf.skip(amount);
    //if block receiver doesn't signal to give up and more bytes are expected
    if(onEachBlock(amount) && (expected>transferred)){
      if(amReader){
        if(buf.freespace()==0){
          buf.rewind();
        }
      }
      return true;
    } else {
      onDone();
      return false;//normal exit
    }
  } else {
    incrementalFile("onchunk: %s",errorText());
  }
  return false;//abnormal exit
}

//stub virtual functions.
bool IncrementalFileTransfer::onEachBlock(__ssize_t amount){
  incrementalFile("block: %d \tbytes: %ld \tchunk:%ld",blockstransferred,transferred,amount);
  return true;
}

void IncrementalFileTransfer::onDone(){
  incrementalFile("block: %d/%d \tbytes: %ld/%ld",blockstransferred,blocksexpected,transferred,expected);
}

IncrementalFileTransfer::~IncrementalFileTransfer(){
  //#nada, just squelching a gratuitous warning
}
