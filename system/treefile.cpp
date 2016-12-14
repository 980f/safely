#include <fstream>
#include <giomm.h>
#include "logger.h"
#include "perftimer.h"
#include "treefile.h"
#include "jsonstore.h"
#include "filer.h"
#include "fcntl.h"
#include "storednumeric.h"
extern double svn();

using namespace Gio;
using namespace Glib;
using namespace std;

TreeFile::TreeFile(const ustring &fname, Storable &root):
  filename(fname),
  root(root) {
}

TreeFile::TreeFile(FileName &fname, Storable &root):
  filename(fname),
  root(root) {
}

bool TreeFile::parseTreeFile(void) {
  PerfTimer perf(ustring::compose("parse tree %1", filename).c_str());
  Filer file;
  if(file.openFile(filename.c_str(),O_RDONLY,false)) {
    if(file.readall(20000000)) {
      JsonStore parser(root);
      ByteScanner scanner(file.contents());
      bool parsedok=parser.parse(scanner);
      root.wasModified();//to clear flags set by parsing;
      return parsedok;
    }
    else {
      dbg("Filename %s too big to be parsed.", filename.c_str());
      return false;
    }
  }
  else {
    dbg("Cannot open file <%s>.", filename.c_str());
    return false;
  }

} /* parseTreeFile */


bool TreeFile::printTree(bool blocking, bool debug) {
  PerfTimer perf(ustring::compose("printTree %1", filename).c_str());
  StoredReal(root("svnnumber"))=::svn();//update to current svn.
  FileName temp_path("var/printTree.tmp");
  fstream fs(temp_path.c_str(), fstream::out);
  if(!fs) {
    dbg("Cannot open file <%s>.", temp_path.c_str());
    return false;
  }
  JsonStore::Printer printer(this->root, fs);
  printer.printValue();
  fs << std::endl; //newline at end of file as a courtesy.
  fs.close();
  if(debug) {   //don't copy from temp
    return false;
  }
  //Ensure directory exists
  Filer::mkDirDashP(filename.c_str(), true);
  Filer::cp(temp_path.c_str(), filename.c_str());
//  if(!debug){
//    Filer::rm(temp_path.c_str());
//  }
  if(blocking) {
    sync();
  }
  root.wasModified();
  return true;
}

double TreeFile::svn(){
  return StoredReal(root("svnnumber"));
}

bool TreeFile::dumpStorage(Storable&root, const char *location){
  FileName rootname("var");
  rootname.folder(location).ext("art");
  TreeFile writer(rootname, root);
  return writer.printTree(false);
}


double svn() {
  return
    #include "svnrevision.txt"
      ;
}
