#ifndef REALSTATISTIC_H
#define REALSTATISTIC_H

/** standard statistics on a real number */

class RealStatistic {
  friend class RealCorrelation;
//  friend class StoredStatistic; //for saving and loading, and perhaps editing
protected:
  double sum;
  double sumSquares;
  int count;//changed to ease mating to StoredInt.
public:
  RealStatistic(void);
  void reset(void);
  void insert(double datum);
  void remove(double datum);
  void replace(double older, double newer);
  double mean(void) const;
  /** common subexpression of other members */
  double Lform(void) const;
  /** sum of squares of deviation from mean */
  double ssd() const;
  /** std, pop-std etc depending upon @param used*/
  double sxd(int used) const;
  int N() const{
    return count;
  }
};

/** standard statistics on paired data */
class RealCorrelation {
public:
  RealStatistic xx;
  RealStatistic yy;
  double sumCross;

  RealCorrelation(void);
  void reset();
  /** add a point to the data set*/
  void insert(double x, double y);
  /** remove a point presumed to be in the data set*/
  void remove(double x, double y);
  /** crossproduct term*/
  double Lcross(void) const;
  /** should be same as R^2 */
  double crossCorrelation(void) const;
  double slope(void) const;
  double offset(void) const;
  /** -log10(r-squared) */
  double nines(void) const;
  /** sum of squares of differences */
  double ssd()const;
  /** @returns square root of chi^2 divided by @param adj defaulted for linear fit */
  double var(int adj=2) const;
};
#endif // REALSTATISTIC_H
