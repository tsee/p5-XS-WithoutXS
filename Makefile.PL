use 5.008005;
use ExtUtils::MakeMaker;
WriteMakefile(
    NAME           => 'XS::WithoutXS',
    VERSION_FROM   => 'lib/XS/WithoutXS.pm',
    PREREQ_PM      => { 'Test::More' => 0 },
    ABSTRACT_FROM  => 'lib/XS/WithoutXS.pm',
    AUTHOR         => 'Steffen Mueller <smueller@cpan.org>',
    LIBS           => [''],         # e.g., '-lm'
    DEFINE         => '',           # e.g., '-DHAVE_SOMETHING'
    INC            => '-I.',        # e.g., '-I. -I/usr/include/other'
    OBJECT         => '$(O_FILES)', # link all the C files too
    #export list of the binary, on some OSes (Win32 for ex.) all symbols are
    #private from the dynamic linker unless explicitly specified
    FUNCLIST       => ['my_sum_no_macros', 'my_sum', 'boot_XS__WithoutXS'],
);

