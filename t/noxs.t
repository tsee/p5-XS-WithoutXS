#!perl
use strict;
use warnings;

use Test::More tests => 2;
use XS::WithoutXS qw(:all);

# The my_sum function is written in plain C without the XS language.
is(my_sum(5,1,2,3), 5+1+2+3);
is(my_sum_no_xs_macros(5,4,2,3), 5+4+2+3);

