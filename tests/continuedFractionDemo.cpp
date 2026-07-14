#include <iostream>

#include "continuedfractionratiogenerator.h"
using namespace std;

int main(int argc,char *argv[]){

    if(argc>1){
        double arg=atof(argv[1]);
        ContinuedFractionRatioGenerator cg;

        cout<<arg<<" was input"<< ", math will be done using "<< ContinuedFractionRatioGenerator::maxWorkingBits << " bits" << endl
        << " epsilon is:"<<ContinuedFractionRatioGenerator::epsilon << endl
        ;
        // << " wtf is:"<<ContinuedFractionRatioGenerator::wtf << endl
        ;
        cg.restart(arg);
        for (unsigned step=0; cg.step() && step++<ContinuedFractionRatioGenerator::maxWorkingBits; ){
            cout <<step<< " " << cg.numerator() << "/" << cg.denominator()<< " error:" << cg.better()-arg <<endl;
        }

    }
    return 0;
}
