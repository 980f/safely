#include "matrixinverter.h"
#include "minimath.h"
#include <cmath>

#define forSize(si) for(unsigned si = size; si-->0;)

#include "logger.h"
static Logger mdbg("MatrixInverter");
static Logger vdbg("MatrixInverter.verbose");

void MatrixInverter::dump(bool please){
  if(please) {
    vdbg("Matrix Inverse:{   (in\tthen\t\tout)");
    forSize(rw){
      Logger::Combiner raii(vdbg);
      vdbg("%d", rw);
      forSize(cl){
        vdbg(",%g", xs[rw][cl]);
      }
      vdbg("\t\t");
      forSize(cl){
        vdbg(",%g", inv[rw][cl]);
      }
    }
    vdbg("}");
  }
} // MatrixInverter::dump

MatrixInverter::MatrixInverter(unsigned size) :size(size),xs(size),inv(size),  ignore(size){}

MatrixInverter::Matrix::Matrix(unsigned size) :
  std::vector< Column >(size){
  forSize(i){
    (*this)[i].resize(size);
  }
}

void MatrixInverter::clear(){
  forSize(rw){
    forSize(cl){
      xs[rw][cl] = 0;//todo:M perhaps we should have an identity matrix rather than a zeroed one?
    }
  }
}

double MatrixInverter::compute(){
  mdbg("Inverting matrix");
  if(size == 1) { //special case to expedite debug of non-special cases
    //ignoring 'ignores'
    double norm = xs[0][0];
    inv[0][0] = ratio(1.0, norm);
    return norm;
  }
  if(size==2 ) {//#optimized for the most frequent case, allowing this to be efficiently used for linear fits.
    if(ignore[1]) {
      double norm = xs[0][0];
      inv[0][0] = ratio(1.0, norm);
      return norm;
    }
    if(ignore[0]) {
      double norm = xs[1][1];
      inv[1][1] = ratio(1.0, norm);
      return norm;
    }
    //todo:1 failed to handle case of both ignored.
    double norm = xs[0][0] * xs[1][1] - xs[0][1] * xs[1][0];
    if(isNormal(norm)) {
      forSize(rw){
        forSize(cl){
          mdbg("In matrix inversion loop (quadratic efficiency)");
          inv[rw][cl] = polarity(rw==cl) * ratio(xs[rw][cl],norm);
        }
      }
      return norm;
    }
    //else fall through to get identical failed state info as before.
  }//end size 2, aka linear fit.
  forSize(rw){
    forSize(cl){
      inv[rw][cl] = xs[rw][cl]; //using separate before and after matrices for debuggability
      mdbg("In second matrix inversion loop (quadratic efficiency)");
    }
  }
  double norm = 1.0;

  forSize(focus){
    if(ignore[focus]) {
      continue;
    }

    double absmax = inv[focus][focus];
    if(isNormal(absmax)) {
      norm *= absmax;
      double invmax = -1 / absmax;
      forSize(rw){  //scale column by biggest entry
        if(inPlay(rw,focus)) {
          inv[rw][focus] *= invmax; //row is multiplied by -1/diagonal element
        }
      }
      forSize(rw){
        if(inPlay(rw,focus)) {
          double d = inv[rw][focus];
          if(isNormal(d)) { //skip iteration if going to be adding multiple of 0. isnormal handles the -0.0's we seem to get.
            forSize(cl){
              if(inPlay(cl,focus)) {
                inv[rw][cl] += inv[focus][cl] * d; //subtract scaled row from other rows
                mdbg("In third matrix inversion loop (quadratic efficiency)");
              }
            }
          }
        }
      }
      invmax = -invmax;
      forSize(cl){
        if(inPlay(cl,focus)) {
          inv[focus][cl] *= invmax; //row is scaled by 1/diagonal element
        }
      }
      inv[focus][focus] = invmax;
    } else {
      //determinant must be 0 as all remaining elements are 0
      return 0;
    }
  }
  return norm;
} // MatrixInverter::compute

bool MatrixInverter::test(){
  Matrix one(size);
  forSize(cl){
    if(!ignore[cl]) {
      forSize(rw){
        if(!ignore[rw]) {
          double product = 0;
          forSize(pi){
            if(!ignore[pi]) {
              product += xs[cl][pi] * inv[pi][rw];
            }
          }
          //product here should equal rw==cl. The deviations from 1.0 or 0.0 shows the noisiness of the fp math.
          one[cl][rw] = product;
        }
      }
    }
  }
  //todo:0 use mdbg() to dump matrix one, which should be the identity matrix.
  return true;//could do some sort of check for identity.
} // MatrixInverter::test
