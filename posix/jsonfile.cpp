//"(C) Andrew L. Heilveil, 2017-2018"
#include "jsonfile.h"

#include "filer.h"
#include "storejson.h"
#include "charscanner.h"

#include "stdio.h" //for printing until we apply our textFormatter
#include "fcntlflags.h"

JsonFile::JsonFile(Storable &node):root(node){
  //#nada
}

int JsonFile::reload(){
  return loadFile(loadedFrom);
}

int JsonFile::loadFile(Cstr thename){
  //#_# this did a crazy thing when the node being loaded is a StoredGroup<StoredNumeric>  root.child("#loadedFromFile").setImage(thename.c_str());
  //... we dare not inject nodes into other people's trees.
  Filer optionFile("LoadJSON");
  loadedFrom=thename;//mark intended file name
  if(! optionFile.openFile(thename)){
    dbg("Couldn't open \"%s\", error:[%d]%s",thename.c_str(),optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  if(! optionFile.readall()){
    dbg("Couldn't read all of \"%s\", error:[%d]%s",thename.c_str(),optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  //maydo: set loadedFrom to absolute canonical pathname.
  //free up options file for external editing (in case we ever open_exclusive)
  optionFile.close();//this does not lose the data already read.
  CharScanner optsText(optionFile.contents());
  Indexer<char> arg(optsText.internalBuffer(),optsText.allocated());//type casting

  StoreJsonParser parser(arg);
  parser.core.root=&root;
  //allow variation of syntax where an '=' is the same as an ':' (when not quoted of course)
  parser.parser.rule.equalscolon=true;//todo: make this configurable
  parser.parser.rule.semicomma=true;//needed in case we embed dp5 config :(
  parser.parser.lookFor(StandardJSONFraming ";=");

  parser.parse();
  dbg("loaded %d nodes, %d levels, from %s",parser.stats.totalNodes,parser.stats.maxDepth.extremum,thename.c_str());
  return 0;//#an errno
}


bool indent(FILE *fp, unsigned tab){
  if(Index(tab).isValid()){
    fputc('\n',fp);
    for(unsigned tabs = tab; tabs-->0; ) {
      fprintf(fp,"  ");
    }
    return true;
  } else {
    return false;
  }
}

void printNode(unsigned tab, Storable &node, FILE *fp,bool showVolatiles){
  if(!showVolatiles && node.isVolatile){
    return;
  }
  if(fp==nullptr){
    fp=stderr;
  }
  bool pretty=indent(fp, tab);
  if(node.name.empty()) {
    //just print tabs
  } else {
    fprintf(fp,"\"%s\" : ",node.name.c_str());
  }
  switch (node.getType()) {
  case Storable::Wad:
    fputc(node.isOrdered?'[':'{',fp);
    for(ChainScanner<Storable> list(node.kinder()); list.hasNext(); ) {
      Storable & it(list.next());
      printNode((pretty?  tab + 1 : tab),it,fp,showVolatiles);
      if(list.hasNext()) {
        fputc(',',fp);
      }
    }
    indent(fp, tab);
    fputc(node.isOrdered?']':'}',fp);
    break;
  case Storable::Numerical:
    fprintf(fp,"%g ",node.getNumber<double>());
    break;
  case Storable::Uncertain:
  case Storable::NotDefined:
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


bool JsonFile::printOn(Cstr somefile, unsigned indent, bool showVolatiles){
  Filer fout("JsonSave");
  if(fout.openFile(somefile,O_CREAT|O_RDWR,true)){
    printNode(indent,root,fout.getfp("w"),showVolatiles);
    return true;
  } else {
    return false;
  }
}

Cstr JsonFile::originalFile(){
  return loadedFrom.c_str();
}

bool JsonFile::printOn(Fildes &alreadyOpened, unsigned indent, bool showVolatiles){
  if(alreadyOpened.isOk()){
    auto fp=alreadyOpened.getfp("w");
    printNode(indent,root,fp,showVolatiles);
    return true;
  } else {
    return false;
  }
}
