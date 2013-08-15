#!/usr/bin/env perl
use strict;
use warnings;
no strict 'subs';
$|++;

use Device::SerialPort;
use MIDI::ALSA(SND_SEQ_EVENT_PORT_UNSUBSCRIBED);
use Digest::CRC qw/crc16/;

my %g_dev = (
	dev => '/dev/ttyACM0',
	speed => '38400',
);

# midi notes
my @notes = (
		8.1757989156,
		8.6619572180,
		9.1770239974,
		10.3008611535,
		10.3008611535,
		10.9133822323,
		11.5623257097,
		12.2498573744,
		12.9782717994,
		13.7500000000,
		14.5676175474,
		15.4338531643,
		16.3515978313,
		17.3239144361,
		18.3540479948,
		19.4454364826,
		20.6017223071,
		21.8267644646,
		23.1246514195,
		24.4997147489,
		25.9565435987,
		27.5000000000,
		29.1352350949,
		30.8677063285,
		32.7031956626,
		34.6478288721,
		36.7080959897,
		38.8908729653,
		41.2034446141,
		43.6535289291,
		46.2493028390,
		48.9994294977,
		51.9130871975,
		55.0000000000,
		58.2704701898,
		61.7354126570,
		65.4063913251,
		69.2956577442,
		73.4161919794,
		77.7817459305,
		82.4068892282,
		87.3070578583,
		92.4986056779,
		97.9988589954,
		103.8261743950,
		110.0000000000,
		116.5409403795,
		123.4708253140,
		130.8127826503,
		138.5913154884,
		146.8323839587,
		155.5634918610,
		164.8137784564,
		174.6141157165,
		184.9972113558,
		195.9977179909,
		207.6523487900,
		220.0000000000,
		233.0818807590,
		246.9416506281,
		261.6255653006,
		277.1826309769,
		293.6647679174,
		311.1269837221,
		329.6275569129,
		349.2282314330,
		369.9944227116,
		391.9954359817,
		415.3046975799,
		440.0000000000,
		466.1637615181,
		493.8833012561,
		523.2511306012,
		554.3652619537,
		587.3295358348,
		622.2539674442,
		659.2551138257,
		698.4564628660,
		739.9888454233,
		783.9908719635,
		830.6093951599,
		880.0000000000,
		932.3275230362,
		987.7666025122,
		1_046.5022612024,
		1_108.7305239075,
		1_174.6590716696,
		1_244.5079348883,
		1_318.5102276515,
		1_396.9129257320,
		1_479.9776908465,
		1_567.9817439270,
		1_661.2187903198,
		1_760.0000000000,
		1_864.6550460724,
		1_975.5332050245,
		2_093.0045224048,
		2_217.4610478150,
		2_349.3181433393,
		2_489.0158697766,
		2_637.0204553030,
		2_793.8258514640,
		2_959.9553816931,
		3_135.9634878540,
		3_322.4375806396,
		3_520.0000000000,
		3_729.3100921447,
		3_951.0664100490,
		4_186.0090448096,
		4_434.9220956300,
		4_698.6362866785,
		4_978.0317395533,
		5_274.0409106059,
		5_587.6517029281,
		5_919.9107633862,
		5_919.9107633862,
		6_644.8751612791,
		7_040.0000000000,
		7_458.6201842894,
		7_902.1328200980,
		8_372.0180896192,
		8_869.8441912599,
		9_397.2725733570,
		9_956.0634791066,
		10_548.0818212118,
		11_175.3034058561,
		11_839.8215267723,
		12_543.8539514160,
); # notes

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
	my $notes = int($size/2);
	my $crc = crc16( pack "CCCS*", (0,0,$notes,@_));

	return pack "CSCS${size}C", SLIP_END, $crc, $notes, @data, SLIP_END; 
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
MIDI::ALSA::client('Arduino MIDI Beeper', 1, 1, 0);

use Data::Dumper;

my $cnt = 0;
my @channel = @ARGV;
my $accumulate = 16;

while (1) {
	my @alsa_event = MIDI::ALSA::input();

	# last event -> exit
	last if ($alsa_event[0] == SND_SEQ_EVENT_PORT_UNSUBSCRIBED());

	# midi event to midi score event
	my @score_event = MIDI::ALSA::alsa2scoreevent( @alsa_event );

    # filter out empty events
	next unless (@score_event && defined $score_event[0] && defined $score_event[4]);

	# accept only note events
	next unless ('note' eq $score_event[0]);

	$score_event[2] = $score_event[2]/1000 - 0.5;
	print Dumper \@score_event;

	# accept only events on specified channel
	next unless (scalar @ARGV == 0 || grep { $_ == $score_event[3] } @ARGV );

	# calculate the pitch and duration
	my $note = int($notes[$score_event[4] % 128]);
	# my $dur = int( $score_event[2]/1000 - 0.5);
	my $dur = 50;

	my $send = $port->write(slip_send($note, $dur));
	while (!$port->write_drain) {}


	my ($count_in, $string_in) = $port->read(5);
	print "Sent: [$note/$dur] Recv [$count_in]: " . unpack "H*", $string_in;
	print "\n";
}

