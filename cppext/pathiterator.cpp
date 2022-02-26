#include "pathiterator.h"


PathIterator::PathIterator(char seperator):bracket(seperator,false,false,false){
  //#nada
}

unsigned PathIterator::start(Indexer<const char> &scan){
  if(!bracket.coalesce){//if you know you are not coalescing then you shouldn't bother to call start.
    return 0;
  }
  unsigned leading=0;
  while(scan.hasNext()&&bracket.isSlash(scan.peek())){
    ++leading;
    scan.skip(1);//burn extra seperating slashes lest they be seen as leading slashes
  }
  return leading;
}

Span PathIterator::next(Indexer<const char> &scan){
  Span cutter;
  if(scan.hasNext()){
    if(bracket.coalesce){
      if( bracket.isSlash(scan.peek())){//if begins with seperator
        scan.next();
        if(!scan.hasNext()){//solitary slash was reporting both before and after slashing
          return cutter;//exiting early here gives us pre, not post, no pieces.
        }
      }
    }
  } else {
    return cutter;//which will be empty so caller should quit calling back at us.
  }
  cutter.lowest=scan.ordinal();//
  //cutter.highest is still invalid

  while(scan.hasNext()) {
    if(bracket.isSlash(scan.next())) {
      cutter.highest=scan.ordinal()-1;
      if(bracket.coalesce){
        if(cutter.empty()){
          //adjacent seperators are as if they are one
          cutter.lowest=scan.ordinal();//which has a side effect of making it invalid, which is good
        } else {
          while(scan.hasNext()&&bracket.isSlash(scan.peek())){
            scan.skip(1);//burn extra seperating slashes lest they be seen as leading slashes
          }
          bracket.after=!scan.hasNext();
          return cutter;//receiver bumps indexer and calls us again
        }
      }
    }
  }

  //the following detection of trailing separator is suspect
  if(scan.contains(cutter.lowest)){//then we found a start of something
    cutter.highest=scan.ordinal();//this trusts internal behavior of ordinal()==lengh when hasNext first fails
    if(cutter.empty()){
      bracket.after=true;
    } else {
      return cutter;
    }
  } else {
    //if (cutter.lowest==span.length), but not if empty string
    bracket.after=scan.ordinal()>0;
  }
  return cutter;
}

PathIterator::Rules::Rules(char slash, bool after, bool before, bool coalesce):
slash(slash)
,after(after)
,before(before)
,coalesce(coalesce)
{
  //do nothing
}
