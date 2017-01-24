#ifndef SERIALAGENT_H
#define SERIALAGENT_H

class SerialAgent {
public:
  virtual bool onReception(int incoming)=0;
};

#endif // SERIALAGENT_H
