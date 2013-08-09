#!/usr/bin/env perl

use strict;
use warnings;
no strict 'refs';
$|++;

use Digest::CRC qw/crc8 crc16 crc32/;

BEGIN {
	srand() if $] < 5.004;
}

my $fn = "perl_binary";
my $count = 8;

my @i = ( 
		[ 8, 'C' ],
		[ 16, 'v'],
		[ 32, 'V'] ,
);

foreach (keys @i) {
	open my $fh, '>', ($fn . '_' . $i[$_]->[0] . '.dat') ||
		die "Unable to create a file [$fn]\n";

	my @data = ();
	push @data, int rand(256) for (1..$count);
	unshift @data, 0 foreach (0..($i[$_][0]/8));

	print $fh pack $i[$_]->[1] . "*", &{ 'crc' . $i[$_][0] }(pack "C*", @data);
	shift @data foreach (0..($i[$_][0]/8));
	print $fh pack "C*", @data; 

	close $fh if ($fh);
}
