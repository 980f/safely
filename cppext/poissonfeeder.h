#ifndef POISSONFEEDER_H
#define POISSONFEEDER_H

#include <random>

/** wrapper for common use of a generator of poisson distribution test data.
 TODO: expose seeding so that reproducible sequences are possible, and so that we don't have an OS dependency (time service is used for default seed)*/
class PoissonFeeder {
  //random number generation song and dance:
  std::random_device randomSource;//this gets fed into 'uniform' as its seeder.
  //Mersenne twister
  std::mt19937 uniform;
  // ReSharper disable once CppRedundantTemplateArguments
  std::exponential_distribution<double> generator;

public:
  /** @param cps is the target mean of the Poisson distribution that his object will generate. */
  PoissonFeeder(double cps);

  double operator()(){
    return generator(uniform);
  }

};
#endif // POISSONFEEDER_H
