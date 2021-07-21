#ifndef POISSONFEEDER_H
#define POISSONFEEDER_H

#include <random>

/** wrapper for common use of a generator of poisson distribution test data.
 TODO:1 expose seeding so that reproducible sequences are possible, and so that we don't have an OS dependency (time service is used for default seed)*/
class PoissonFeeder {
  //random number generation song and dance:
  std::random_device randomSource;
  std::mt19937 uniform;
  std::exponential_distribution<double> generator;

public:
  PoissonFeeder(double cps);

  double operator()(){
    return generator(uniform);
  }

};
#endif // POISSONFEEDER_H
