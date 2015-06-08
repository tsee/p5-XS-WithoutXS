package XS::WithoutXS;
use 5.008005;
use warnings;
use DynaLoader ();

our $VERSION = '0.001';

my $SharedLibrary;

setup_so_access(__PACKAGE__); # assuming package==dist name, see also below

# Install the test subroutines!
newXS("my_sum", "my_sum", "this_is_not_xs.c");
newXS("my_sum_no_macros", "my_sum_no_macros", "this_is_not_xs.c");


# Locates the given symbol and, with the full assumption that
# it has the right function signature, installs it as an external
# subroutine!
sub newXS {
  my ($func_name, $full_symbol_name, $filename) = @_;
  my $sym = DynaLoader::dl_find_symbol($SharedLibrary, $full_symbol_name);
  die "Failed to locate $full_symbol_name" if not defined $sym;
  DynaLoader::dl_install_xsub($func_name, $sym, defined $filename ? $filename : "WithoutXS");
}


# The following is all just module setup for
# being able to access C functions without XS(Loader).
sub setup_so_access {
  my $pkg = shift;
  return if defined $SharedLibrary;

  my @pkg_components = split /::/, $pkg;

  my $pkg_path = join "/", @pkg_components;
  my @dirs = (map "-L$_/auto/$pkg_path", @INC);
  my (@mod_files) = DynaLoader::dl_findfile(@dirs, $pkg_components[-1]);
  die "Failed to locate shared library for '$pkg'"
    if not @mod_files;

  $SharedLibrary = DynaLoader::dl_load_file($mod_files[0]);
}

1;
