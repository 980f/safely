#ifndef DIRECTORYWALKER_H
#define DIRECTORYWALKER_H

#include "ftw.h"
#include "textkey.h"
#include "fildes.h"

#include "functional"

/** wraps usage of ntfw treewalker.
 * ntfw is blocking in the sense that you give it a callback but it runs on your thread so the call to ntfw doesn't return until the callback has been called on all files.
 * The underlying nftw makes our walker be non-thread safe, hopefully that is worth the better features it brings to the game.
 */
class DirectoryWalker {
  static std::function<void(Fildes &)> action;
  TextKey path;
  static int readerthunk(const char *,const struct stat* stat,int flags,FTW *ftw);
public:
  DirectoryWalker(TextKey path);
  /** processes all the files, does not recurse, does follow symlinks, opens file for read before calling action*/
  void flatread(std::function<void(Fildes &)> reader);

};

#endif // DIRECTORYWALKER_H
