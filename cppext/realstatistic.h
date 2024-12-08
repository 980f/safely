#pragma once

/** standard statistics on a real number */
// class RealCorrelation;
// class StoredStatistic;

class RealStatistic {
  // friend class RealCorrelation;
  // friend class StoredStatistic;
public://friending wasn't working well enough.
  /** sum of data */
  double sum;
  /** sum of squares of data */
  double sumSquares;
  /** number of points in the sums*/
  unsigned count;

public:
  RealStatistic();

  /** zero all internal data */
  void reset();

  /** include @param datum in the stats */
  void insert(double datum);

  /** remove @param datum in the stats, trusting that it was a value once entered via insert */
  void remove(double datum);

  /** remove @param older, insert @param newer */
  void replace(double older, double newer);

  /** compute and return the average */
  double mean() const;

  /** common subexpression of other members */
  double Lform() const;

  /** sum of squares of deviation from mean */
  double ssd() const;

  /** std, pop-std etc depending upon @param used*/
  double sxd(unsigned used) const;

  unsigned N() const {
    return count;
  }
}; // class RealStatistic

class StoredCorrelation;
/** standard statistics on paired data */
class RealCorrelation {
  friend class StoredCorrelation;
protected:
  RealStatistic xx;
  RealStatistic yy;
  double sumCross;

public:
  RealCorrelation();

  void reset();

  /** add a point to the data set*/
  void insert(double x, double y);

  /** remove a point presumed to be in the data set*/
  void remove(double x, double y);

  /** crossproduct term like Lform of the statistic */
  double Lcross() const;

  /** should be same as R^2 */
  double crossCorrelation() const;

  double slope() const;

  double offset() const;

  /** -log10(R^2)
   * The number of 9's after the decimal point in the R^2 statistic, used when a fit has to be really very good.
   */
  double nines() const;

  /** sum of squares of differences */
  double ssd() const;

  /** @returns square root of chi^2 divided by n - @param adj, defaulted for linear fit */
  double var(unsigned adj = 2) const;

  /** a correlation is not meaningful if we get infinities when computing the variance.
   *  @returns whether stats are significant or if @param barely then or's whether A and B are significant
   */
  bool isMeaningful(bool barely) const {
    int num = xx.count;
    return num > 2 || (barely && num == 2);
  }

  unsigned N() const {
    return xx.count;
  }
}; // class RealCorrelation
