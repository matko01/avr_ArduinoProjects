#!/usr/bin/env perl

use strict;
use warnings;
$|++;


use Device::SerialPort;
use Digest::CRC qw/crc16/;
use Time::HiRes qw/usleep/;


my %g_dev = (
	dev => '/dev/ttyACM0',
	speed => '9600',
);


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

	my $crc = crc16( pack "C*", (0,0,@_));
	return pack "CSC*", (SLIP_END, $crc, @data, SLIP_END); 
}


sub slip_recv {
	my $read_cb = shift || return;
	my @data = ();
	my $mode = 0;

	while (1) {
		my $c = 0x00;
		next unless &$read_cb(\$c);

		if ($c == SLIP_END) {
			last if (scalar @data);
			next;
		}

		if ($c == SLIP_ESC) {
			$mode = 1;
			next;
		}

		if ($mode) {
			$c = ($c == SLIP_ESC_END ? SLIP_END : 
					($c == SLIP_ESC_ESC ? SLIP_ESC : $c));
			$mode = 0;
		}

		push @data, $c;
	}
	
	pack "C*", @data;
}

# get the input values
die "host_slip.pl <prescaler> <ocr> <max_st>\n"
	unless (scalar @ARGV == 3);

my $prescaler = shift || 0;
my $ocr = shift || 0;
my $max_st = shift || 0;

# initialize the port
my $port = new Device::SerialPort($g_dev{dev}); 
$port->baudrate($g_dev{speed}); 
$port->parity("none"); 
$port->databits(8); 
$port->stopbits(1); 
$port->dtr_active(1);
$port->write_settings();
$port->purge_all;


# getchar wrapper for slip receive
sub serial_getc {
	my $cout = shift;
	my ($cin, $cdata) = $port->read(1);
	$$cout = unpack "C", $cdata;
	return $cin;
}


# wait for the reset
sleep 4;

# send the timer_data
$port->write(slip_send($prescaler, $ocr, $max_st));
until ($port->write_drain) {}

# wait for ACK or NACK
my $data = slip_recv(\&serial_getc);

# unpack the serialized data to variables
my ($crc, $status) = unpack("SC", $data);

# calculate the checksum of the response for verification
my $crc_calcd = crc16(pack("SC", (0,$status)));

# display the results
printf("CRC/CRC_CALCD/STATUS: 0x%04x/0x%04x/%s[%02x]\n", 
		$crc, 
		$crc_calcd,
		($status == 0x10 ? "ACK" : "NACK"),
		$status);
