#!perl
use strict;
use warnings;

use Test::More tests => 1;
use XS::WithoutXS qw(:all);

is(my_sum(5,1,2,3), 5+1+2+3);

