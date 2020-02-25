#pragma once

#include <vector>
#include <functional>
template <typename V> class VectorIterator {
  std::vector<V> &v;
public:
  VectorIterator(std::vector<V> &v):v(v){
    //#done
  }
  void foreach (std::function<void(V&)> body) {
    for(auto it=v.begin();it!=v.end();++it){
      body(*it);
    }
  }
};

template <typename V> class ConstVectorIterator {
  const std::vector<V> &v;
public:
  ConstVectorIterator(const std::vector<V> &v):v(v){
    //#done
  }
  void foreach (std::function<void(const V&)> body) {
    for(auto it=v.begin();it!=v.end();++it){
      body(*it);
    }
  }
};
