#include <gsl_statistics.h>

double 
gsl_stats_max (const double data[], unsigned int n)
{
  /* finds the largest member of a dataset */

  double max = data[0];
  unsigned int i;

  for (i = 0; i < n; i++)
    {
      if (data[i] > max)
	max = data[i];
    }

  return max;
}

double 
gsl_stats_min (const double data[], unsigned int n)
{
  /* finds the smallest member of a dataset */

  double min = data[0];
  unsigned int i;

  for (i = 0; i < n; i++)
    {
      if (data[i] < min)
	min = data[i];
    }

  return min;
}

