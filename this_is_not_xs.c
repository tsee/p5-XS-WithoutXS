#include "this_is_not_xs.h"

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

void
my_sum(pTHX_ CV *cv)
{
  /* Declare Perl-interfacing related variables including "items",
   * the number of parameters on the Perl stack. And declare and set
   * "sp", the local copy of the stack pointer (which we access
   * through the SP macro.). */
  dVAR; dXSARGS;

  int i;
  double sum = 0.;

  SP -= items; /* Move stack pointer back by # of arguments so ST() works */

  /* Go through arguments (as SVs) and add their *N*umeric *V*alue to
   * the output sum. */
  for (i = 0; i < items; ++i)
    sum += SvNV( ST(i) );

  /* Push return value on the Perl stack, convert number to Perl SV. */
  mPUSHn(sum); 
  XSRETURN(1); /* Tell Perl there's one value waiting on the stack,
                * incrementing the global stack pointer. */
  return;
}

