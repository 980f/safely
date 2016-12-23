#include "segmentedname.h"
#include "textpointer.h"

SegmentedName::SegmentedName(bool rooted):rooted(rooted)
{

}

void SegmentedName::prefix(TextKey parent)
{
  this->insert(new Cstr(parent),0);
}

void SegmentedName::suffix(TextKey child)
{
  append(new Cstr(child));
}

void SegmentedName::prefix(const Cstr &parent)
{
  insert(new Text(parent),0);
}

void SegmentedName::suffix(const Cstr &child)
{
  append(new Text(child));
}

ChainScanner<Cstr> SegmentedName::indexer()
{
  return ChainScanner<Cstr>(*this);
}

ConstChainScanner<Cstr> SegmentedName::indexer() const
{
  return ConstChainScanner<Cstr>(*this);
}

