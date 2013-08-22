#!/usr/bin/env perl

use strict;
use warnings;
$|++;

use Device::SerialPort;
use Digest::CRC qw/crc16/;
use Time::HiRes qw/usleep/;

my %g_dev = (
	dev => '/dev/ttyACM0',
	speed => '115200',
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

	my $size = scalar @data;
	my $crc = crc16( pack "C*", (0,0,$size,@_));
	return pack "CSC*", (SLIP_END, $crc, $size, @data, SLIP_END); 
}


my $port = new Device::SerialPort($g_dev{dev}); 
$port->baudrate($g_dev{speed}); 
$port->parity("none"); 
$port->databits(8); 
$port->stopbits(1); 
$port->dtr_active(1);
$port->write_settings();
$port->purge_all;

# wait for the reset
sleep 2;

my $wav = shift || die "<wpwm.pl <8kHz 8bit mono wave-file>\n";
open my $g_fh, '<', $wav ||
	die "Unable to open file [$wav]\n";

binmode ($g_fh);
my $header = 0x00;
my $offset = 0x00;
die "Unable to read WAV header\n" unless($offset = read $g_fh, $header, 44);
my @header = unpack "a4la4a4ls2l2s2a4l", $header;

# verifications
die "Header is empty\n" unless scalar @header;
die "File is not a WAVE RIFF file\n"
	unless ($header[0] =~ /RIFF/ && $header[2] =~ /WAVE/);
die "File is [$header[7]] Hz/$header[10]. Should be 8000/8bit\n"
	unless ($header[7] == 8000 && $header[10] == 8);

my $read = $offset;
while ($read) {
	print "Sending data. Offset: [$offset]\n";

	my $data = 0x00;
	$read = read $g_fh, $data, 32;
	$offset += $read;

	$port->write(slip_send(unpack "C*", $data));
	until ($port->write_drain) {}

	# 32 * 0.000125
	usleep(3000);
}

close ($g_fh);
