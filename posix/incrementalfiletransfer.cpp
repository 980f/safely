//"(C) Andrew L. Heilveil, 2017,2024"
#include "incrementalfiletransfer.h"

#include <aio.h>

#include "logger.h"

IncrementalFileTransfer::IncrementalFileTransfer(bool reader) : PosixWrapper{"incrementalFile"}, amReader(reader), fd(reader ? "IFT-reader" : "IFT-writer") {}

IncrementalFileTransfer::~IncrementalFileTransfer() {
  fd.close();//and expect a flurry of errors that do god knows what.
}

void IncrementalFileTransfer::prepare(unsigned amount) {
  fd.setBlocking(false); // don't trust callers.
  transferred = 0;
  blockstransferred = 0;
  if (amReader) {
    expected = amount;
    blocksexpected = quanta(expected, buf.allocated());
  } else {
    expected = buf.allocated();
    blocksexpected = quanta(expected, amount);
  }
}

bool IncrementalFileTransfer::onChunk(ssize_t amount) {
  if (amount >= 0) { // then it is # of bytes transferred
    ++blockstransferred;
    transferred += amount;
    buf.skip(amount);
    // if block receiver doesn't signal to give up and more bytes are expected
    if (onEachBlock(amount) && (expected > transferred)) {
      if (amReader) {
        if (buf.freespace() == 0) {
          buf.rewind();
        }
      }
      return true;
    } else {
      onDone();
      return false; // normal exit
    }
  } else {
    errornumber = amount; // todo: might need a negation
    dbg("onchunk: %s", errorText());
  }
  return false; // abnormal exit
}

// stub virtual functions.
bool IncrementalFileTransfer::onEachBlock(ssize_t amount) {
  dbg("block: %d \tbytes: %ld \tchunk:%ld", blockstransferred, transferred, amount);
  return true;
}

void IncrementalFileTransfer::onDone() {
  dbg("block: %d/%d \tbytes: %ld/%ld", blockstransferred, blocksexpected, transferred, expected);
}
