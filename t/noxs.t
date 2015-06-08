#!perl
use strict;
use warnings;

use Test::More tests => 8;
use XS::WithoutXS;

# The my_sum function is written in plain C without the XS language.
is(XS::WithoutXS::my_sum(5,1,2,3), 5+1+2+3);
is(XS::WithoutXS::my_sum_no_macros(5,4,2,3), 5+4+2+3);

use XSLoader;
# boot not called by default
XSLoader::load('XS::WithoutXS');

my @ret = XS::WithoutXS::count_args();
ok(@ret == 1 && $ret[0] == 0, 'count_args with empty list works');
@ret = XS::WithoutXS::count_args(1);
ok(@ret == 1 && $ret[0] == 1, 'count_args with one arg works');
@ret = XS::WithoutXS::count_args(1,2,3,4);
ok(@ret == 1 && $ret[0] == 4, 'count_args with list works');
@ret = XS::WithoutXS::sum(1,2,3,4);
ok(@ret == 1 && $ret[0] == 10, 'sum with list works');
@ret = XS::WithoutXS::sum();
ok(@ret == 1 && $ret[0] == 0, 'sum with empty list works');
@ret = XS::WithoutXS::sum(8);
ok(@ret == 1 && $ret[0] == 8, 'sum with one arg works');