package XS::WithoutXS;

use strict;
use warnings;
use Exporter;
use DynaLoader ();

use base 'Exporter';

our $VERSION     = '0.001';
our @EXPORT_OK;
our %EXPORT_TAGS = ( 'all' => \@EXPORT_OK );

my $SharedLibrary;

setup_so_access(__PACKAGE__); # assuming package==dist name, see also below

# Install a test subroutine!
newXS("my_sum", "my_sum", "this_is_not_xs.c");
push @EXPORT_OK, "my_sum";
newXS("my_sum_no_xs_macros", "my_sum_xs_macros_are_evil", "this_is_not_xs.c");
push @EXPORT_OK, "my_sum_no_xs_macros";


# Locates the given symbol and, with the full assumption that
# it has the right function signature, installs it as an external
# subroutine!
sub newXS {
  my ($func_name, $full_symbol_name, $filename) = @_;
  my $sym = DynaLoader::dl_find_symbol($SharedLibrary, $full_symbol_name);
  die "Failed to locate $full_symbol_name" if not defined $sym;
  DynaLoader::dl_install_xsub($func_name, $sym, $filename // "WithoutXS");
}


# The following is all just module setup for
# being able to access C functions without XS(Loader).
sub setup_so_access {
  my $pkg = shift; 
  my @pkg_components = split /::/, $pkg;

  my $pkg_path = join "-", @pkg_components;
  my @dirs = (map "-L$_/auto/$pkg_path", @INC);
  my (@mod_files) = DynaLoader::dl_findfile(@dirs, $pkg_components[-1]);
  die if not @mod_files;

  $SharedLibrary = DynaLoader::dl_load_file($mod_files[0]);
}

1;
