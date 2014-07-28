#!/usr/bin/env perl

use Test::More;

use strict;
use warnings;

my $bf = './bf';
my @tests;

for my $run (@tests) {
	my ($prog, $output, $input) = @$run;
	$input ||= '';

	my $result = `echo -n "$input" | $bf -e '$prog'`;

	is($result, $output);
}

done_testing();

BEGIN {

my $cp = ',[.,]';

my $hello = '
++++++++++[>+++++++>++++++++++>+++>+<<<<-]
H  >++.
e  >+.
l  +++++++.
l  .
o  +++.
   >++.
W  <<+++++++++++++++.
o  >.
r  +++.
l  ------.
d  --------.
!  >+.
\n >.
';

my $rot13 = ',[
[>>++++[>++++++++<-]<+<-[>+>+>-[>>>]<[[>+<-]>>+>]<<<<<-]]
>>>[-]+>--[-[<->+++[-]]]
<[++++++++++++<[>-[>+>>]>[+[<+>-]>+>>]<<<<<-]>>[<+>-]>[-[-<<[-]>>]<<[<<->>-]>>]<<[<<+>>-]]
<[-]<.[-]<-,
]';

my $hstr = "Hello World!\n";

@tests = (

[$cp, '', ''],
[$cp, $hstr, $hstr],
[$hello, $hstr],
[$rot13, $hstr, "Uryyb Jbeyq!\n"],
[$hstr, ''],

);

}

