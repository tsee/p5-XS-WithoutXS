#define PERL_NO_GET_CONTEXT
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
  mXPUSHn(sum);
  XSRETURN(1); /* Tell Perl there's one value waiting on the stack,
                * incrementing the global stack pointer. */
  return;
}


/* The below eschews macros where they wouldn't lead to lots of C
 * preprocessor gunk about threads when expanded. So it does
 * use "pTHX" and "aTHX" variants. */

/* pTHX_ passes the Perl interpreter when using ithreads,
 * otherwise compiled out. */
void my_sum_no_macros(pTHX_ CV *cv)
{
  /* NOTE(ARGUNUSED(cv)) */

  /* Local copy of the global Perl argument stack pointer.
   * This is the top of the stack, not the base! */
  SV **sp = PL_stack_sp;

  /* Get the top "mark" offset from the stack of marks.
   * The mark tells us where on the Perl stack
   * the parameters for this function begin. */
  I32 ax = *PL_markstack_ptr--;
  SV **mark = PL_stack_base + ax;

  /* And finally, the number of parameters for this function. */
  I32 items = (I32)(sp - mark);

  int i;
  double sum = 0.;
  IV num_return_values;

  /* Move stack pointer back by number  of arguments so we have
   * more convenient access to arguments.
   * Basically, this means argument access by increasing index
   * in "first to last" order instead of access
   * in "last to first" order by using negative offsets. */
  sp -= items;

  /* Go through arguments (as SVs) and add their *N*umeric *V*alue to
   * the output sum. */
  for (i = 0; i < items; ++i) {
    /* the SV * under sp isn't valid, since sp == mark means 0 items */
    SV * sv =  *(sp + i+1);
    sum += (sv->sv_flags & (SVf_NOK|SVs_GMG)) == SVf_NOK ?
      ((XPVNV*)(sv->sv_any))->xnv_u.xnv_nv
      : Perl_sv_2nv_flags(aTHX_ sv, SV_GMAGIC);
  }

  num_return_values = 1;
  /* Make sure we have space on the stack (in case the function was
   * called without arguments... */
  if (PL_stack_max - sp < (SSize_t)num_return_values) {
    /* Oops, not enough space, extend. Needs to reset the
     * sp variable since it might have caused a proper realloc. */
    sp = Perl_stack_grow(aTHX_ sp, sp, (SSize_t)num_return_values);
  }

  /* Push return value on the Perl stack, convert number to Perl SV. */
  /* Also makes the value mortal, that is avoiding a memory leak. */
  *++sp = Perl_sv_2mortal(aTHX_ Perl_newSVnv(aTHX_ sum) );

  /* Commit the changes we've done to the stack by setting the global
   * stack pointer to our modified copy. */
  PL_stack_sp = sp;

  return;
}

void my_count_args(pTHX_ CV *cv)
{
  /* create local copy of sp for efficiency */
  SV **sp = PL_stack_sp;

  /* read our mark offset */
  I32 ax = *PL_markstack_ptr;
  /* turn the mark offset into a stack addr */
  SV **mark = PL_stack_base + ax;
  /* difference betwee sp and the mark behind it is number of items */
  I32 items = (I32)(sp - mark);
  SV * result_sv;

  /* remove our mark from mark stack, caller expects it */
  PL_markstack_ptr -= 1;

  result_sv = Perl_sv_newmortal(aTHX);
  Perl_sv_setiv(aTHX_ result_sv, items);

  /* wipe incoming args to zero slots*/
  sp = mark;

  /* bounds check the stack, we might have had a 0 length list*/
  if (PL_stack_max - sp < 1) {
    sp = Perl_stack_grow(aTHX_ sp, sp, 1);
  }
  /* now advance sp 1 slot ahead to return an arg */
  sp++;
  /* write the SV * to Perl stack */
  *sp = result_sv;
  /* update global sp with new addr/position */
  PL_stack_sp = sp;

  return;
}


/* absolute minimum XSUB */
/* sub my_return_at_arg {return @_;} */
void my_return_at_arg(pTHX_ CV *cv)
{
  PL_markstack_ptr--;
}

/* sub my_print_scalar { print($_[0].""); return ();} */
void my_print_scalar(pTHX_ CV *cv)
{
  /* create local copy of sp for efficiency */
  SV **sp = PL_stack_sp;
  /* read our mark offset */
  I32 ax = *PL_markstack_ptr;
  /* turn the mark offset into a stack addr */
  SV **mark = PL_stack_base + ax;
  /* calculate item count */
  I32 items = (I32)(sp - mark);
  SV * sv;
  char * string;

  /* remove our mark from mark stack, caller expects it */
  PL_markstack_ptr -= 1;

  /* throw exception if wrong count */
  if(items != 1) Perl_croak(aTHX_ "usage: XS::WithoutXS::print_scalar(sv)");
  /* read first and only arg */
  sv = *sp;
  /* get string out of SV pointer */
  string = (sv->sv_flags & (SVf_POK|SVs_GMG)) == SVf_POK
      ? sv->sv_u.svu_pv
      : Perl_sv_2pv_flags(aTHX_ sv, NULL, SV_GMAGIC);
  printf("%s", string);
  /* 1 arg to zero args */
  sp--;
  /* update global sp */
  PL_stack_sp = sp;
}

/* sub print_arg_count_sum {
    my @count = count_args(@_);
    die "count_args returned wrong list count"
      if @count != 1;
    my @sum = sum(@_);
    die "sum returned wrong list count"
      if @sum != 1;
    printf("count is %s sum is %s", $count[0], $sum[0]);
   } */

void my_print_arg_count_sum(pTHX_ CV *cv)
{
  /* create local copy of sp for efficiency */
  SV **sp = PL_stack_sp;
  /* read our mark offset */
  I32 ax = *PL_markstack_ptr;
  /* turn the mark offset into a stack addr */
  SV **mark = PL_stack_base + ax;
  /* calculate item count */
  I32 items = (I32)(sp - mark);
  SV * count_sv;
  SV * sum_sv;
  SV * string_sv;
  I32 callee_ax;

  /* remove our mark from mark stack, caller expects it */
  PL_markstack_ptr -= 1;
  { /* bounds check the mark stack */
    I32 * mark_stack_entry;
    if ((mark_stack_entry = ++PL_markstack_ptr) == PL_markstack_max)
      mark_stack_entry = markstack_grow();
     /* put new offset onto it */
    *mark_stack_entry  = callee_ax = (I32)(sp - PL_stack_base);
  }
  /* grow stack if needed */
  if (PL_stack_max - sp < items) {
    sp = Perl_stack_grow(aTHX_ sp, sp, items);
  }
  /* advance sp off last incoming element onto first outgoing element */
  /* copy incoming arguments to outgoing arguments */
  memcpy(sp+1,mark+1, items * sizeof(SV*));
  /* PL_stack_sp must be a ptr to the last element, not 1 after last */
  PL_stack_sp = sp+items;
  {
    CV * count_args = Perl_get_cvn_flags(aTHX_ "XS::WithoutXS::count_args", sizeof("XS::WithoutXS::count_args")-1, 0);
    my_count_args(aTHX_ count_args);
  }
  sp = PL_stack_sp;
  if((I32)(sp - (PL_stack_base + callee_ax)) != 1)
    Perl_croak(aTHX_ "count_args returned wrong list count");
  count_sv = *sp;
  sp--;
  /* dont bounds check, done already once */
  *++PL_markstack_ptr = callee_ax;

  /* recompute mark it might have been realloced after last sub call */
  mark = PL_stack_base + ax;
  /* copy incoming arguments to outgoing arguments */
  memcpy(sp+1,mark+1, items * sizeof(SV*));
  PL_stack_sp = sp+items;
  {
    CV * sum_no_macros = Perl_get_cvn_flags(aTHX_ "XS::WithoutXS::sum", sizeof("XS::WithoutXS::sum")-1, 0);
    my_sum_no_macros(aTHX_ sum_no_macros);
  }
  sp = PL_stack_sp;
  if((I32)(sp - (PL_stack_base + callee_ax)) != 1)
    Perl_croak(aTHX_ "sum returned wrong list count");
  sum_sv = *sp;
  sp--;
  /* %-p is a perl printf flag for SV * */
  string_sv = Perl_newSVpvf(aTHX_  "count is %-p sum is %-p", count_sv, sum_sv);
  Perl_sv_2mortal(aTHX_ string_sv);
  /* bounds check done earlier */
  *++PL_markstack_ptr = callee_ax = (I32)(sp - PL_stack_base);
  sp++;
  *sp = string_sv;
  PL_stack_sp = sp;
  {
    CV * print_scalar = Perl_get_cvn_flags(aTHX_ "XS::WithoutXS::print_scalar", sizeof("XS::WithoutXS::print_scalar")-1,0);
    my_print_scalar(aTHX_ print_scalar);
  }

  /* recompute mark it might have been realloced after last sub call */
  mark = PL_stack_base + ax;
  /* wipe everything, "return ();" */
  PL_stack_sp = mark;
}



void boot_XS__WithoutXS(pTHX_ CV *cv)
{
    SV **mark;
    I32 ax = *PL_markstack_ptr;
    PL_markstack_ptr -= 1;
    mark = PL_stack_base + ax;
    /* return nothing */
    PL_stack_sp = mark;
    printf("inside boot_XS__WithoutXS\n");
    Perl_newXS(aTHX_ "XS::WithoutXS::count_args", my_count_args, "this_is_not_xs.c");
    Perl_newXS(aTHX_ "XS::WithoutXS::return_at_arg", my_return_at_arg, "this_is_not_xs.c");
    Perl_newXS(aTHX_ "XS::WithoutXS::print_scalar", my_print_scalar, "this_is_not_xs.c");
    Perl_newXS(aTHX_ "XS::WithoutXS::print_arg_count_sum", my_print_arg_count_sum, "this_is_not_xs.c");
    Perl_newXS(aTHX_ "XS::WithoutXS::sum", my_sum_no_macros, "this_is_not_xs.c");
}
