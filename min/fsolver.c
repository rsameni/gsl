/* min/fsolver.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_min.h>

#include "min.h"

static int 
compute_f_values (gsl_function * f, double minimum, double * f_minimum,
                  gsl_interval x, double * f_lower, double * f_upper);


static int 
compute_f_values (gsl_function * f, double minimum, double * f_minimum,
                  gsl_interval x, double * f_lower, double * f_upper)
{
  SAFE_FUNC_CALL(f, x.lower, f_lower);
  SAFE_FUNC_CALL(f, x.upper, f_upper);
  SAFE_FUNC_CALL(f, minimum, f_minimum);

  return GSL_SUCCESS;
}

int
gsl_min_fminimizer_set (gsl_min_fminimizer * s, 
                        gsl_function * f, double minimum, gsl_interval x)
{
  int status ;

  double f_minimum, f_lower, f_upper;

  status = compute_f_values (f, minimum, &f_minimum, x, &f_lower, &f_upper);

  if (status != GSL_SUCCESS)
    {
      return status ;
    }
  
  status = gsl_min_fminimizer_set_with_values (s, f, minimum, f_minimum, 
                                               x, f_lower, f_upper);
  return status;
}

gsl_min_fminimizer *
gsl_min_fminimizer_alloc (const gsl_min_fminimizer_type * T) 
{
  gsl_min_fminimizer * s = 
    (gsl_min_fminimizer *) malloc (sizeof (gsl_min_fminimizer));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for minimizer struct",
			GSL_ENOMEM, 0);
    };

  s->state = malloc (T->size);

  if (s->state == 0)
    {
      free (s);		/* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for minimizer state",
			GSL_ENOMEM, 0);
    };

  s->type = T ;
  s->function = NULL;

  return s;
}

int
gsl_min_fminimizer_set_with_values (gsl_min_fminimizer * s, gsl_function * f, 
                                    double minimum, double f_minimum, 
                                    gsl_interval x, 
                                    double f_lower, double f_upper)
{
  s->function = f;
  s->minimum = minimum;
  s->interval = x;

  if (x.lower > x.upper)
    {
      GSL_ERROR ("invalid interval (lower > upper)", GSL_EINVAL);
    }

  if (minimum >= x.upper || minimum <= x.lower) 
    {
      GSL_ERROR ("minimum must lie inside interval (lower < x < upper)",
                 GSL_EINVAL);
    }

  s->f_lower = f_lower;
  s->f_upper = f_upper;
  s->f_minimum = f_minimum;

  if (f_minimum >= f_lower || f_minimum >= f_upper)
    {
      GSL_ERROR ("endpoints do not enclose a minimum", GSL_EINVAL);
    }

  return (s->type->set) (s->state, s->function, 
                         minimum, f_minimum, 
                         x, f_lower, f_upper);
}


int
gsl_min_fminimizer_iterate (gsl_min_fminimizer * s)
{
  return (s->type->iterate) (s->state, s->function, 
                             &(s->minimum), &(s->f_minimum),
                             &(s->interval), &(s->f_lower), &(s->f_upper));
}

void
gsl_min_fminimizer_free (gsl_min_fminimizer * s)
{
  free (s->state);
  free (s);
}

const char *
gsl_min_fminimizer_name (const gsl_min_fminimizer * s)
{
  return s->type->name;
}

double
gsl_min_fminimizer_minimum (const gsl_min_fminimizer * s)
{
  return s->minimum;
}

gsl_interval
gsl_min_fminimizer_interval (const gsl_min_fminimizer * s)
{
  return s->interval;
}

