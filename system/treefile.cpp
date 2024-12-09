#include "treefile.h"
#include "logger.h"
#include "storednumeric.h"  //version number

#include <fstream>
//#include <giomm.h>

#include <unistd.h>
#include <ustring.h>

#include "jsonstore.h"
#include "filer.h"
#include "fcntl.h"

#include "perftimer.h"

/** expects a supplier of a linear number such as svn produces. */
extern double svn();

//using namespace Gio;
//using namespace Glib;
//using namespace std;

TreeFile::TreeFile(FileName &fname, Storable &node) :
  filename(fname),
  root(node),
  ConnectChild(svnnumber){
}

bool TreeFile::parseTreeFile(void){
//  PerfTimer perf(ustring::compose("parse tree %1", filename).c_str());
  auto fname=filename.pack();
  Filer file("TreeFile");
  if(file.openFile(fname.c_str(),O_RDONLY,false)) {
    if(file.readall(20000000)) {
      JsonStore parser(root);
      ByteScanner scanner(file.contents());
      bool parsedok = parser.parse(scanner);
      root.wasModified();//to clear flags set by parsing;
      return parsedok;
    } else {
      dbg("Filename %s too big to be parsed.", fname.c_str());
      return false;
    }
  } else {
    dbg("Cannot open file <%s>.", fname.c_str());
    return false;
  }

} /* parseTreeFile */

bool TreeFile::printTree(bool blocking, bool debug){
  PerfTimer perf(Ustring::compose("printTree %1", filename).c_str());
  StoredReal(root("svnnumber")) = ::svn();//update to current svn.
  FileName temp_path("var/printTree.tmp");
  auto temp_name=temp_path.pack();
  std::fstream fs(temp_name, std::fstream::out);
  if(!fs) {
    dbg("Cannot open file <%s>.", temp_name.c_str());
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
  auto fname=filename.pack();
  Filer::mkDirDashP(fname, true);
  Filer::cp(temp_name, fname);
//  if(!debug){
//    Filer::rm(temp_path.c_str());
//  }
  if(blocking) {
    sync();
  }
  root.wasModified();
  return true;
} // TreeFile::printTree

double TreeFile::svn(){
  return StoredReal(root("svnnumber"));
}

bool TreeFile::dumpStorage(Storable&root, const char *location){
  FileName rootname("var");
  rootname.append(location).append(".art");
  TreeFile writer(rootname, root);
  return writer.printTree(false);
}
