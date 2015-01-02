#!./perl
#
# Tests for Perl run-time environment variable settings
#
# $SPECPERLOPT, $SPECPERLLIB, etc.

BEGIN {
    chdir 't' if -d 't';
    @INC = '../lib';
    require Config; import Config;
    unless ($Config{'d_fork'}) {
        print "1..0 # Skip: no fork\n";
	    exit 0;
    }
}

use Test;

plan tests => 17;

my $STDOUT = './results-0';
my $STDERR = './results-1';
my $PERL = './perl';
my $FAILURE_CODE = 119;

delete $ENV{PERLLIB};
delete $ENV{SPECPERLLIB};
delete $ENV{SPECPERLOPT};

# Run perl with specified environment and arguments returns a list.
# First element is true if Perl's stdout and stderr match the
# supplied $stdout and $stderr argument strings exactly.
# second element is an explanation of the failure
sub runperl {
  local *F;
  my ($env, $args, $stdout, $stderr) = @_;

  unshift @$args, '-I../lib';

  $stdout = '' unless defined $stdout;
  $stderr = '' unless defined $stderr;
  local %ENV = %ENV;
  delete $ENV{PERLLIB};
  delete $ENV{SPECPERLLIB};
  delete $ENV{SPECPERLOPT};
  my $pid = fork;
  return (0, "Couldn't fork: $!") unless defined $pid;   # failure
  if ($pid) {                   # parent
    my ($actual_stdout, $actual_stderr);
    wait;
    return (0, "Failure in child.\n") if ($?>>8) == $FAILURE_CODE;

    open F, "< $STDOUT" or return (0, "Couldn't read $STDOUT file");
    { local $/; $actual_stdout = <F> }
    open F, "< $STDERR" or return (0, "Couldn't read $STDERR file");
    { local $/; $actual_stderr = <F> }

    if ($actual_stdout ne $stdout) {
      return (0, "Stdout mismatch: expected [$stdout], saw [$actual_stdout]");
    } elsif ($actual_stderr ne $stderr) {
      return (0, "Stderr mismatch: expected [$stderr], saw [$actual_stderr]");
    } else {
      return 1;                 # success
    }
  } else {                      # child
    for my $k (keys %$env) {
      $ENV{$k} = $env->{$k};
    }
    open STDOUT, "> $STDOUT" or exit $FAILURE_CODE;
    open STDERR, "> $STDERR" or it_didnt_work();
    { exec $PERL, @$args }
    it_didnt_work();
  }
}


sub it_didnt_work {
    print STDOUT "IWHCWJIHCI\cNHJWCJQWKJQJWCQW\n";
    exit $FAILURE_CODE;
}

sub try {
  my ($success, $reason) = runperl(@_);
  $reason =~ s/\n/\\n/g if defined $reason;
  ok( !!$success, 1, $reason );
}

#  SPECPERLOPT    Command-line options (switches).  Switches in
#                    this variable are taken as if they were on
#                    every Perl command line.  Only the -[DIMUdmtw]
#                    switches are allowed.  When running taint
#                    checks (because the program was running setuid
#                    or setgid, or the -T switch was used), this
#                    variable is ignored.  If SPECPERLOPT begins with
#                    -T, tainting will be enabled, and any
#                    subsequent options ignored.

try({SPECPERLOPT => '-w'}, ['-e', 'print $::x'],
    "", 
    qq{Name "main::x" used only once: possible typo at -e line 1.\nUse of uninitialized value in print at -e line 1.\n});

try({SPECPERLOPT => '-Mstrict'}, ['-e', 'print $::x'],
    "", "");

try({SPECPERLOPT => '-Mstrict'}, ['-e', 'print $x'],
    "", 
    qq{Global symbol "\$x" requires explicit package name at -e line 1.\nExecution of -e aborted due to compilation errors.\n});

# Fails in 5.6.0
try({SPECPERLOPT => '-Mstrict -w'}, ['-e', 'print $x'],
    "", 
    qq{Global symbol "\$x" requires explicit package name at -e line 1.\nExecution of -e aborted due to compilation errors.\n});

# Fails in 5.6.0
try({SPECPERLOPT => '-w -Mstrict'}, ['-e', 'print $::x'],
    "", 
    <<ERROR
Name "main::x" used only once: possible typo at -e line 1.
Use of uninitialized value in print at -e line 1.
ERROR
    );

# Fails in 5.6.0
try({SPECPERLOPT => '-w -Mstrict'}, ['-e', 'print $::x'],
    "", 
    <<ERROR
Name "main::x" used only once: possible typo at -e line 1.
Use of uninitialized value in print at -e line 1.
ERROR
    );

try({SPECPERLOPT => '-MExporter'}, ['-e0'],
    "", 
    "");

# Fails in 5.6.0
try({SPECPERLOPT => '-MExporter -MExporter'}, ['-e0'],
    "", 
    "");

try({SPECPERLOPT => '-Mstrict -Mwarnings'}, 
    ['-e', 'print "ok" if $INC{"strict.pm"} and $INC{"warnings.pm"}'],
    "ok",
    "");

try({SPECPERLOPT => '-w -w'},
    ['-e', 'print $ENV{SPECPERLOPT}'],
    '-w -w',
    '');

try({SPECPERLOPT => '-t'},
    ['-e', 'print ${^TAINT}'],
    '-1',
    '');

try({SPECPERLLIB => "foobar$Config{path_sep}42"},
    ['-e', 'print grep { $_ eq "foobar" } @INC'],
    'foobar',
    '');

try({SPECPERLLIB => "foobar$Config{path_sep}42"},
    ['-e', 'print grep { $_ eq "42" } @INC'],
    '42',
    '');

try({SPECPERLLIB => "foobar$Config{path_sep}42"},
    ['-e', 'print grep { $_ eq "foobar" } @INC'],
    'foobar',
    '');

try({SPECPERLLIB => "foobar$Config{path_sep}42"},
    ['-e', 'print grep { $_ eq "42" } @INC'],
    '42',
    '');

try({SPECPERLLIB => "foo",
     PERLLIB => "bar"},
    ['-e', 'print grep { $_ eq "foo" } @INC'],
    'foo',
    '');

try({SPECPERLLIB => "foo",
     PERLLIB => "bar"},
    ['-e', 'print grep { $_ eq "bar" } @INC'],
    '',
    '');

# SPECPERLLIB tests with included arch directories still missing

END {
    1 while unlink $STDOUT;
    1 while unlink $STDERR;
}