#ifndef REALSTATISTIC_H
#define REALSTATISTIC_H


/** standard statistics on a real number */
class RealStatistic {
public://until we can get friend stuff worked out
/** sum of data */
    double sum;
    /** sum of squares of data */
  double sumSquares;
  /** number of points in the sums*/
  int count;
public:
  RealStatistic(void);
  /** zero all internal data */
  void reset(void);
  /** include @param datum in the stats */
  void insert(double datum);
  /** remove @param datum in the stats, trusting that it was a value once entered via insert */
  void remove(double datum);
  /** remove @param older, insert @param newer */
  void replace(double older, double newer);
  /** compute and return the average */
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
public: //until we get friendship worked out, then should protect.
  double sumCross;
public:
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
