#include "cpshelpers.h"

using namespace sigc;

void SingleShotEmitter::run() {
  conn.disconnect();
  continuation();
  delete this; //NEW@ SingleShotEmitter::getInstance()
}

SingleShotEmitter::SingleShotEmitter(signal< void > sig, slot< void > continuation):
  continuation(continuation) {
  conn = sig.connect(mem_fun(this, &SingleShotEmitter::run));
}

SingleShotEmitter &SingleShotEmitter::getInstance(signal< void > sig, slot< void > continuation) {
  return *new SingleShotEmitter(sig, continuation); //DEL@ SingleShotEmitter::run()
}

//////////////////////////////

Barrier::Barrier(const slot< void > continuation, int count):
  continuation(continuation),
  count(count) {
}

Barrier &Barrier::getInstance(const sigc::slot< void > continuation, int count) {
  return *new Barrier(continuation, count); //DEL@ Barrier::hit()
}

void Barrier::hit(int hits) {
  count -= hits;
  if(count <= 0) {
    continuation();
    delete this;//NEW@ Barrier::getInstance()
  }
}

void Barrier::unhit(int hits) {
  count += hits;
}

slot< void > Barrier::make_hitter(int hits, bool unhitit) {
  if(unhitit){
    unhit(hits);
  }
  return bind(mem_fun(this, &Barrier::hit), hits);
}

void Barrier::connect_single_shot_hitter(signal< void > sig, int hits) {
  SingleShotEmitter::getInstance(sig, make_hitter(hits));
}
