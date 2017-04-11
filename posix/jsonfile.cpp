#include "jsonfile.h"

#include "filer.h"
#include "storejson.h"
#include "charscanner.h"

#include "stdio.h" //for printing until we apply our textFormatter

JsonFile::JsonFile(Storable &node):root(node)
{

}

int JsonFile::loadFile(Cstr thename){
  Filer optionFile;
  root.child("#loadedFromFile").setImage(thename);//record where we try to load from.
  if(! optionFile.openFile(thename)){
    dbg("Couldn't open \"%s\", error:[%d]%s",thename.c_str(),optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  if(! optionFile.readall(~0U)){
    dbg("Couldn't read all of \"%s\", error:[%d]%s",thename.c_str(),optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  //free up options file for external editing (in case we ever open_exclusive)
  optionFile.close();//this does not lose the data already read.
  ByteScanner optsText(optionFile.contents());
  Indexer<u8> arg(optsText.internalBuffer(),optsText.allocated());//type casting

  StoreJsonParser parser(arg);
  parser.core.root=&root;
  //allow variation of syntax where an '=' is the same as an ':' (when not quoted of course)
  parser.parser.rule.equalscolon=true;//todo: make this configurable
  parser.parse();
  root.resolve(true);//until we patch the loader to not create uncertainty
  loadedFrom=thename;//record after success
  //todo: stats?
  return int(parser.stats.totalNodes);
}


//bug: empty wads do not print closing },
void printNode(unsigned tab, Storable &node, FILE *fp){
  if(fp==nullptr){
    fp=stderr;
  }
  bool pretty=Index(tab).isValid();
  if(pretty){
    fputc('\n',fp);
    for(unsigned tabs = tab; tabs-->0; ) {
      fprintf(fp,"  ");
    }
  }
  if(node.name.empty()) {
    //just print tabs
  } else {
    fprintf(fp,"\"%s\" : ",node.name.c_str());
  }
  switch (node.getType()) {
  case Storable::Wad:
    fprintf(fp,"{");
    for(auto list(node.kinder()); list.hasNext(); ) {
      Storable & it(list.next());
      printNode(pretty?tab + 1:BadIndex,it,fp);
      if(list.hasNext()) {
        fputc(',',fp);
      } else {
        if(pretty){
          fputc('\n',fp);
          for(unsigned tabs = tab; tabs-->0; ) {
            fprintf(fp,"  ");
          }
        }
        fputc('}',fp);
      }
    }
    break;
  case Storable::Numerical:
    fprintf(fp,"%g ",node.getNumber<double>());
    break;
  case Storable::Uncertain:
  case Storable::NotKnown:
    fprintf(fp,"%s ",node.image().c_str());
    break;
  case Storable::Textual:
    if(node.image().empty()){
      fputc('"',fp);
      //else printf converts null ptr to (null)
      fputc('"',fp);
    } else {
      fprintf(fp,"\"%s\" ",node.image().c_str());
    }
    break;
  } // switch
  fflush(fp);
}


void JsonFile::printOn(Cstr somefile, unsigned indent){
  FILE *fout=fopen(somefile,"w");
  if(fout){
    printNode(indent,root,fout);
  }
}

Cstr JsonFile::originalFile(){
  return root.child("#loadedFromFile").image();
}


void JsonFile::printOn(Fildes &alreadyOpened, unsigned indent){
  printNode(indent,root,alreadyOpened.getfp("w"));

}
