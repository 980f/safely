#pragma once

#include "fstream"
#include "delimitingoutputstream.h"
#include "storable.h"
#include "storedgroup.h"

//#include "settable.h"
/** makes tabular human readable file from various of our named storage mechanisms */
class StorageExporter {
public:
  DelimitingOutputStream dos;
public:
  StorageExporter(std::ostream &fos);
  DelimitingOutputStream &exportNode(Storable &node, bool header = false, TextKey name = "");
  DelimitingOutputStream &exportTable(Storable &headers, Storable &rows);
  //DelimitingOutputStream &exportSettable(Settable &thing);
  /** line per groupie of a stored group whose grouping node is the given parameter. does not add spacer line after end.
   *  if @param title is not null then prefix table with number of entities,title */
  DelimitingOutputStream & exportGroup(Storable &groupnode,const char *title = 0);
  template<typename Ignored> DelimitingOutputStream & exportGroup(StoredGroup<Ignored> &group,const char *title = 0){
    return exportGroup(group.node,title);
  }
#define dout(label,value) header ? dos.put(label) : dos.put(value)

}; // class StorageExporter

