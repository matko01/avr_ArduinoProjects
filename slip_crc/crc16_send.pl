#!/usr/bin/env perl

use strict;
use warnings;
no strict 'refs';
$|++;

use Digest::CRC qw/crc16/;
use Device::SerialPort;

BEGIN {
	srand() if $] < 5.004;
}

my %g_dev = (
	dev => '/dev/ttyACM0',
	speed => '4800',
);

my $input = shift || "123456789";
my $crc = crc16($input . "\0\0");
my @dat = unpack "C*", ($input . pack "S", $crc);

use constant {
	SLIP_END => 0300,
	SLIP_ESC => 0333,
	SLIP_ESC_END => 0334,
	SLIP_ESC_ESC => 0335,
};

sub slip_send {
	my @data = map { 
		$_ == SLIP_END ? (SLIP_ESC, SLIP_ESC_END) : 
			($_ == SLIP_ESC ? (SLIP_ESC, SLIP_ESC_ESC) : $_) } @_ ;

	my $size = @data;
	return pack "CC${size}C", SLIP_END, @data, SLIP_END; 
}

my $port = new Device::SerialPort($g_dev{dev}); 
$port->baudrate($g_dev{speed}); 
$port->parity("none"); 
$port->databits(8); 
$port->stopbits(1); 
$port->dtr_active(0);
$port->write_settings();
$port->purge_all;

while (1) {
	my $send = $port->write(slip_send(@dat));
	my ($count_in, $string_in) = $port->read(32);
	
	print "Sent/Recv: [$send/$count_in]\n";
	print "$string_in\n";

	sleep 1;
}
