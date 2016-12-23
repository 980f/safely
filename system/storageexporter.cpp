#include "storageexporter.h"

//using namespace Glib;

StorageExporter::StorageExporter(std::ostream &fos) : dos(fos){

}

//#define dout(label,value) header ? dos.put(label) : dos.put(value)
//'as header line' only works if master object provided is well-formed.
DelimitingOutputStream &StorageExporter::exportNode(Storable &node, bool header, TextKey name){
  node.preSave();
  if(node.is(Storable::Numerical)) {
    dout(name, node.getNumber< double >());
  } else if(node.is(Storable::Textual)) {
    dout(name, node.image());
  } else if(node.is(Storable::Wad)) {
    ForKinder( node){
      Storable &kid(list.next());
      NodeName kidname(Ustring::format(name, ".", kid.name).c_str());
      exportNode(kid, header, kidname);
    }
  }
  return dos;
} // StorageExporter::exportNode

DelimitingOutputStream &StorageExporter::exportTable(Storable &headers, Storable &rows){
  exportNode(headers).endl();
  ForKinder( rows){
    exportNode(list.next()).endl();
  }
  dos.gs();
  return dos;
}

DelimitingOutputStream &StorageExporter::exportSettable(Settable &wrapped){
  makeArgs(wrapped.numParams());
  wrapped.getParams(args);
  ArgSet writer(args);
  while(writer.hasNext()) {
    dos.put(writer.next());
  }
  return dos;
}

DelimitingOutputStream &StorageExporter::exportGroup(Storable &node, const char *title){
  if(title) {//title blocks include the number of rows in the block
    int num = node.numChildren();
    dos.put(num).put(title).endl();
  }

  if(node.has(0)) {
    exportNode(node[0], true, node[0].name);//names of 0th child are headers
    dos.endl();
  }
  ForKinder( node){
    exportNode(list.next());
    dos.endl();
  }
  return dos;
} // StorageExporter::exportGroup
