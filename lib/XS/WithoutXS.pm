package XS::WithoutXS;

use strict;
use warnings;
use Exporter;
use DynaLoader ();

use base 'Exporter';


our $VERSION     = '0.001';
our @EXPORT_OK;
our %EXPORT_TAGS = ( 'all' => \@EXPORT_OK );

# The following four lines are all just module setup for
# being able to access C functions without XS
my @dirs = (map "-L$_/auto/XS-WithoutXS", @INC);
my (@mod_files) = DynaLoader::dl_findfile(@dirs, "WithoutXS");
die if not @mod_files;

my $lib = DynaLoader::dl_load_file($mod_files[0]);


# Locates the given symbol and, with the full assumption that
# it has the right function signature, installs it as an external
# subroutine!
sub newXS {
  my ($func_name, $full_symbol_name, $filename) = @_;
  my $sym = DynaLoader::dl_find_symbol($lib, $full_symbol_name);
  die "Failed to locate $full_symbol_name" if not defined $sym;
  DynaLoader::dl_install_xsub($func_name, $sym, $filename // "WithoutXS");
}

# Install a test subroutine!
newXS("my_sum", "my_sum", "this_is_not_xs.c");
push @EXPORT_OK, "my_sum";

1;

