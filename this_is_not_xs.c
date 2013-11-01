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

/* The below eschews macros where they wouldn't lead to lots of C
 * preprocessor gunk about threads when expanded. */

/* pTHX_ passes the Perl interpreter when using ithreads,
 * otherwise compiled out. */
void my_sum_xs_macros_are_evil(pTHX_ CV *cv)
{
  /* NOTE(ARGUNUSED(cv)) */

  dVAR; /* ithreads dependent way of getting global-interpreter-vars
           struct out of perl for faster access. To obnoxious to inline,
           see perl.h. */

  /* Declare Perl-interfacing related variables including "items",
   * the number of parameters on the Perl stack. And declare and set
   * "sp", the local copy of the stack pointer (which we access
   * through the SP macro.). */

  /* Local copy of the global Perl argument stack pointer.
   * This is the top of the stack, not the base! */
  SV **sp = PL_stack_sp;

  /* Get the top "mark" offset from the stack of marks.
   * The mark tells us where on the Perl stack
   * the parameters for this function begin. */
  I32 ax = *PL_markstack_ptr--;
  SV **mark = PL_stack_base + ax++;

  /* And finally, the number of parameters for this function. */
  I32 items = (I32)(sp - mark);

  int i;
  double sum = 0.;

  /* Move stack pointer back by # of arguments so we have
   * more convenient access to arguments. */
  sp -= items;

  /* Go through arguments (as SVs) and add their *N*umeric *V*alue to
   * the output sum. */
  for (i = 0; i < items; ++i)
    sum += SvNV( PL_stack_base[ax + i] );

  /* Push return value on the Perl stack, convert number to Perl SV. */
  mPUSHn(sum);
  XSRETURN(1); /* Tell Perl there's one value waiting on the stack,
                * incrementing the global stack pointer. */
  return;
}
