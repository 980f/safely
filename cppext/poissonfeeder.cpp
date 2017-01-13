#include "poissonfeeder.h"

PoissonFeeder::PoissonFeeder(double cps):
    uniform (randomSource()),
    generator(cps){
    //#nada
}
