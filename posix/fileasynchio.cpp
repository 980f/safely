//"(C) Andrew L. Heilveil, 2017"
#include "fileasynchio.h"
#include "logger.h"
#include "nanoseconds.h"

SafeLogger(FAA, false);

FileAsyncAccess::FileAsyncAccess(bool reader) : IncrementalFileTransfer(reader) {
  EraseThing(cb); // 4 debug
  cb.aio_fildes=~0;//bad FD, 0 has a tendency to be stdin.
}

bool FileAsyncAccess::go() {
  EraseThing(cb); // forget prior operation
  //-- not doing this so that we can 'cat' multiple files into one buffer: buf.rewind();
  /* define the signal for the aio lib to send when something happens */
  cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  cb.aio_sigevent.sigev_signo = SIGIO;
  cb.aio_sigevent.sigev_value.sival_ptr = this;

  /* Map the Signal to the Signal Handler */
  struct sigaction sig_act;
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO; // to get our 'this' sent back to us
  sig_act.sa_sigaction = sighandler; // NB: gnu header is wonky for this member, plays poorly with preprocessor
  return ok(sigaction(SIGIO, &sig_act, nullptr)) && launch(false /* not a continuation*/);
} // FileAsyncAccess::go

bool FileAsyncAccess::notDone() const {
  return !isDone() && (transferred < expected);
}

bool FileAsyncAccess::block(double seconds) {
  NanoSeconds ns(seconds);
  aiocb *list = &cb; // make a list of 1 item:
  return ok(aio_suspend(&list, 1, &ns));
}

void FileAsyncAccess::loiter() {
  FAA("waiting for about %d events", blocksexpected);
  while (notDone()) {
    if (block(1)) {
      if (!isOk()) {
        FAA("While waiting got: %d(%s)", errornumber, errorText());
        if (errornumber == EINTR) { // on read or block shorter than buffer.
          if (transferred == expected) {
            FAA("...which is pointless, happens in last incompletely filled block");
          }
        }
      }
    }
    // todo: detect freader errornumber's that are fatal and break
  }
} // FileAsyncAccess::loiter

void FileAsyncAccess::cancel() {
  aio_cancel(fd, &cb);
}

bool FileAsyncAccess::launch(bool more) {
  if (more) {
    // maydo: add 'lastSuccessfulTransferAmount' so we can loosen up retry on error in continuation
    cb.aio_offset += cb.__return_value; // #! this is gnu glibc specific, could call aio_return to get the value
  } else {
    cb.aio_offset = 0;
  }
  cb.aio_fildes = fd;
  cb.aio_buf = &buf.peek(); // next spot in buffer
  cb.aio_nbytes = buf.freespace(); // maximum to xfer
  if (amReader) { // data sink
    // a callback with failure occurs before this function gets to the call to ok.
    // the chunk handler will have to capture that error and honor it over what launch returns.
    return ok(aio_read(&cb));
  } else { // data source
    return ok(aio_write(&cb));
  }
} // FileAsyncAccess::launch

void FileAsyncAccess::sighandler(int signo, siginfo_t *info, void * /*ucontext_t context*/) {
  if (signo == SIGIO) {
    FileAsyncAccess *req = reinterpret_cast<FileAsyncAccess *>(info->si_value.sival_ptr);
    req->notified(info->si_code, info->si_errno);
  }
}

void FileAsyncAccess::notified(int code, int ernumber) {
  // a possibly interesting fact:
  if (code == SI_ASYNCIO && ernumber == 0) { // ernumber is related to the signal, not the trigger for the signal
    if (!failure(aio_error(&cb))) { /* Request completed successfully, get the return status of the underlying read operation */
      __ssize_t ret = aio_return(&cb);
      if (onChunk(ret)) { // notify recipient, return asks us to repeat
        // caller is responsible for rewinding the buffer
        if (failed(launch(true))) { // if requested retry fails
          onChunk(-errornumber);
          // but do not allow for chaining after a failure due to possibility of infinite loop
        }
      }
    } else {
      onChunk(-errornumber);
      // but do not allow for chaining after a failure
    }
  } else {
    // wtf("Unexpected signal in " __FILE__);
  }
} // FileAsyncAccess::notified

bool FileAsyncAccess::onEachBlock(__ssize_t amount) {
  if (amReader) {
    buf.peek() = 0;
    FAA("READ:[%d]:%s", amount, buf.internalBuffer());
  } else {
    FAA("WRITE:[%d]: remaining=%ld", amount, buf.freespace());
  }
  return true;
}

void FileAsyncAccess::onDone() {
  //  FAA("ONDONE: not overloaded");
}
