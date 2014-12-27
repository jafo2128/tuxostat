#!/usr/bin/perl -w
#----------------------------------------------------------------
# Skelett Programm fuer die Ansteuerung eines
# Crystelfontz 20x4 Display Typ: 635 (USB)
# Based on Code from "Vincent" Crystelfontz Tech Forum
#----------------------------------------------------------------

use strict;
use vars qw( $OS_win $ob $port);
use Time::HiRes qw( usleep ualarm gettimeofday tv_interval );
use Device::SerialPort;

#----------------------------------------------------------------

my $maxretries 	= 1;
my $timer 		= 14000;
my $result		= 0;
my @resarray;

# LED Variables
# - Led0 is on top
# - Led3 is on botton
# - Each Led can be green or red or both

# LED Pins
my $LED0_GREEN	= 11;
my $LED0_RED	= 12;
my $LED1_GREEN	= 9;
my $LED1_RED	= 10;
my $LED2_GREEN	= 7;
my $LED2_RED	= 8;
my $LED3_GREEN	= 5;
my $LED3_RED	= 6;
	
# LED State
my $led0_green	= 0;
my $led0_red	= 0;
my $led1_green	= 0;
my $led1_red	= 0;
my $led2_green	= 0;
my $led2_red	= 0;
my $led3_green	= 0;
my $led3_red	= 0;

# Keys

my $KEY_UP_PRESS 	= 1;
my $KEY_DOWN_PRESS	= 2;
my $KEY_LEFT_PRESS	= 3;
my $KEY_RIGHT_PRESS	= 4;
my $KEY_ENTER_PRESS	= 5;
my $KEY_EXIT_PRESS	= 6;

# USB Port with the Display behind
my $port 		= '/dev/ttyUSB1';

# Values for Contrast and Backlight
# Ok for the Blue-Version of CF635
my $contrast	= 110;
my $backlight 	= 25;

# Checksum Table for the CRC Calculations
my @CRC_LOOKUP =
(0x00000,0x01189,0x02312,0x0329B,0x04624,0x057AD,0x06536,0x074BF,
0x08C48,0x09DC1,0x0AF5A,0x0BED3,0x0CA6C,0x0DBE5,0x0E97E,0x0F8F7,
0x01081,0x00108,0x03393,0x0221A,0x056A5,0x0472C,0x075B7,0x0643E,
0x09CC9,0x08D40,0x0BFDB,0x0AE52,0x0DAED,0x0CB64,0x0F9FF,0x0E876,
0x02102,0x0308B,0x00210,0x01399,0x06726,0x076AF,0x04434,0x055BD,
0x0AD4A,0x0BCC3,0x08E58,0x09FD1,0x0EB6E,0x0FAE7,0x0C87C,0x0D9F5,
0x03183,0x0200A,0x01291,0x00318,0x077A7,0x0662E,0x054B5,0x0453C,
0x0BDCB,0x0AC42,0x09ED9,0x08F50,0x0FBEF,0x0EA66,0x0D8FD,0x0C974,
0x04204,0x0538D,0x06116,0x0709F,0x00420,0x015A9,0x02732,0x036BB,
0x0CE4C,0x0DFC5,0x0ED5E,0x0FCD7,0x08868,0x099E1,0x0AB7A,0x0BAF3,
0x05285,0x0430C,0x07197,0x0601E,0x014A1,0x00528,0x037B3,0x0263A,
0x0DECD,0x0CF44,0x0FDDF,0x0EC56,0x098E9,0x08960,0x0BBFB,0x0AA72,
0x06306,0x0728F,0x04014,0x0519D,0x02522,0x034AB,0x00630,0x017B9,
0x0EF4E,0x0FEC7,0x0CC5C,0x0DDD5,0x0A96A,0x0B8E3,0x08A78,0x09BF1,
0x07387,0x0620E,0x05095,0x0411C,0x035A3,0x0242A,0x016B1,0x00738,
0x0FFCF,0x0EE46,0x0DCDD,0x0CD54,0x0B9EB,0x0A862,0x09AF9,0x08B70,
0x08408,0x09581,0x0A71A,0x0B693,0x0C22C,0x0D3A5,0x0E13E,0x0F0B7,
0x00840,0x019C9,0x02B52,0x03ADB,0x04E64,0x05FED,0x06D76,0x07CFF,
0x09489,0x08500,0x0B79B,0x0A612,0x0D2AD,0x0C324,0x0F1BF,0x0E036,
0x018C1,0x00948,0x03BD3,0x02A5A,0x05EE5,0x04F6C,0x07DF7,0x06C7E,
0x0A50A,0x0B483,0x08618,0x09791,0x0E32E,0x0F2A7,0x0C03C,0x0D1B5,
0x02942,0x038CB,0x00A50,0x01BD9,0x06F66,0x07EEF,0x04C74,0x05DFD,
0x0B58B,0x0A402,0x09699,0x08710,0x0F3AF,0x0E226,0x0D0BD,0x0C134,
0x039C3,0x0284A,0x01AD1,0x00B58,0x07FE7,0x06E6E,0x05CF5,0x04D7C,
0x0C60C,0x0D785,0x0E51E,0x0F497,0x08028,0x091A1,0x0A33A,0x0B2B3,
0x04A44,0x05BCD,0x06956,0x078DF,0x00C60,0x01DE9,0x02F72,0x03EFB,
0x0D68D,0x0C704,0x0F59F,0x0E416,0x090A9,0x08120,0x0B3BB,0x0A232,
0x05AC5,0x04B4C,0x079D7,0x0685E,0x01CE1,0x00D68,0x03FF3,0x02E7A,
0x0E70E,0x0F687,0x0C41C,0x0D595,0x0A12A,0x0B0A3,0x08238,0x093B1,
0x06B46,0x07ACF,0x04854,0x059DD,0x02D62,0x03CEB,0x00E70,0x01FF9,
0x0F78F,0x0E606,0x0D49D,0x0C514,0x0B1AB,0x0A022,0x092B9,0x08330,
0x07BC7,0x06A4E,0x058D5,0x0495C,0x03DE3,0x02C6A,0x01EF1,0x00F78);

#----------------------------------------------------------------
# Program Start
#----------------------------------------------------------------

$ob = Device::SerialPort->new($port);
die "Can't open serial port $port: $^E\n" unless ($ob);

# Set the Baudrate and other Parameters
setport();

# Set Contrast and Backlight
contrast($contrast);
backlight($backlight);

# Disable the Reporting of all Key-Releases, only
# Keypresses are reported
Set_Key_Reporting_Mask(63,0);

# Clear the Screen
clear_lcd();

# Sets the Cursor Style
set_cursor_style(0);

# Print Template (x,y,text)
# x = col, y = line 
#            01234567890123456789
printxy(0,0,'>> Test Program   <<');

while (1)
{
	@resarray=splitpkt(getpkt($ob));
	foreach (@resarray)
	{
		my $ret = decodepkt($_);
		print "Returncode: $ret\n";

		# Now you have the Key, witch was pressed:
		#  1 = KEY_UP_PRESS
		#  2 = KEY_DOWN_PRESS
		#  3 = KEY_LEFT_PRESS
		#  4 = KEY_RIGHT_PRESS
		#  5 = KEY_ENTER_PRESS
		#  6 = KEY_EXIT_PRESS

		# Play around with the LED's, UP-Key toggles LED0
		# form green to red
		# DOWN-Key turns LED0 off

		if ($ret == $KEY_UP_PRESS)
		{
			printxy(0,3,'                    ');
			printxy(0,3,'UP-Key pressed');
			if ($led0_green == 100)
			{
				$led0_green = 0;
				$led0_red   = 100;
			}
			else
			{
				$led0_green = 100;
				$led0_red   = 0;
			}
			toggle_led($LED0_GREEN,$led0_green);
			toggle_led($LED0_RED,$led0_red);
		}
		elsif ($ret == $KEY_DOWN_PRESS)
		{
			printxy(0,3,'DOWN-Key pressed');
			$led0_green = 0;
			$led0_red   = 0;
		
			toggle_led($LED0_GREEN,$led0_green);
			toggle_led($LED0_RED,$led0_red);
		}
	}
	sleep 1;
}
undef $ob;

exit 0;

#----------------------------------------------------------------
# Subroutines
#----------------------------------------------------------------

#----------------------------------------------------------------
#	Convert a string into hex for easier viewing
#----------------------------------------------------------------

sub str2hex
{
	my ($line) = @_;
	$line =~ s/(.)/sprintf("%02x ",ord($1))/eg;
	return $line;
}

#----------------------------------------------------------------
#	Converts hexvalues to a string. 
#	Used to create strings to send to serial port
# 	$string = hex2str ( $hexstring );
# 	Where string is of the form "xx xx xx xx" where x is 0-9a-f
# 	hex numbers are limited to 8 bits.
#----------------------------------------------------------------

sub hex2str
{
	my ($l) = @_;
	$l =~ s/([0-9a-f]{1,2})\s*/sprintf("%c",hex($1))/egi;
	return $l;
}

#----------------------------------------------------------------
# 	Send a string over the serial port
#	Input: port,string
#	Output: string returned by device
#----------------------------------------------------------------

sub sendpkt
{
	my ($lcd,$packet)=@_;
	my $count_out;
	my $result='';
	my $fail=0;
	while ($result eq '' && $fail != $maxretries) {
		$count_out = $lcd->write($packet);
		warn "write failed\n" unless ($count_out);
		warn "write incomplete\n" if ( $count_out != length($packet) );
		$result=getpkt($lcd);
		$fail++ if $result eq '';
		if ($fail == $maxretries){
			print"CF635 not responding. Increase delay\n";
			$result='';
		}
	}	
	return $result;
}

#----------------------------------------------------------------
#	Read string from port
#	Input: port
#----------------------------------------------------------------

sub getpkt
{
	my ($lcd)=@_;
	usleep($timer);
	return $lcd->input;
}

#----------------------------------------------------------------
#	Generate a string packet with correct CRC, for sending over serial port
#	Input: 
#		packet type (one byte,in decimal), according to CF spec, in decimal
#		data: string representing arguments. Must be a string
#			data should not include length. It is added automatically
#----------------------------------------------------------------

sub genpkt
{
	my ($type,$paramdata) = @_;
	my $finalpacket;
	my $data = pack 'a*',$paramdata;
	my $length = length($data);
	my $packet = pack 'C2a*',$type,$length,$data;
	my $crc = 0xFFFF;
	
	foreach my $char (split //, $packet)
		{
		# & is bitwise AND
		# ^ is bitwise XOR
		# >> bitwise shift right

		$crc = ($crc >> 8) ^ $CRC_LOOKUP[($crc ^ ord($char) ) & 0xFF] ;

		# print out the running crc at each byte
		# printf ("char=%s char=0x%x ord(char)=%d\n",$char,ord($char),ord($char));
	}
	# get the complement

	$crc = ~$crc ;
	$crc = ($crc & 0xFFFF) ;

	#printf("CRC for packet is: 0x%x 0x%x\n",$crc & 0x00FF,$crc >> 8);

	$finalpacket = $packet.chr($crc & 0x00FF).chr($crc >> 8);

	#print "Packet length:".$length." Packet data:".str2hex($packet)."\n";
	#print "Packet is:".str2hex($finalpacket)."\n";

	return $finalpacket;
}

#----------------------------------------------------------------
# Set the serial port properties
#----------------------------------------------------------------

sub setport {
	$ob->user_msg(1);	# misc. warnings
	$ob->error_msg(1);	# hardware and data errors
	$ob->baudrate(115200);
	$ob->parity_enable(1);   # for any parity except "none"
	$ob->databits(8);
	$ob->stopbits(1);
	$ob->parity("none");
	$ob->handshake("none");
	$ob->datatype('raw');
	$ob->write_settings;
	#$ob->save("portcfg.cfg");
}

#----------------------------------------------------------------
#	Unserialize an incoming string
#	Input: string
#	Output: array of messages
#----------------------------------------------------------------

sub splitpkt{
	my $data=shift;
	my $i=0;
	my $start=0;
	my @result;
	#print "Isolating packets from: ".str2hex($data)."\n";
	if ($data ne '')
	{
		for ( $start=0;$start < length($data)-2; $start++)
		{
	    	for ($i=0;$i < length ($data)-$start-2;$i++)
			{
				my $tmppkt=substr($data,$start,$i+2);#possible packet without crc
				my $packet=substr($data,$start,$i+4);#possible packet including crc
				my $calculated=genpkt(ord(substr($tmppkt,0,1)),substr($tmppkt,2,length($tmppkt)));
				if ($calculated eq $packet)
				{
					push @result,$packet;
					next;
				}
	    	}
		}
	}
	return @result;
}

#----------------------------------------------------------------
# Decode a Packet
# Return: 
# 99 = Error when no Data
#  1 = KEY_UP_PRESS
#  2 = KEY_DOWN_PRESS
#  3 = KEY_LEFT_PRESS
#  4 = KEY_RIGHT_PRESS
#  5 = KEY_ENTER_PRESS
#  6 = KEY_EXIT_PRESS
#  7 = KEY_UP_RELEASE
#  8 = KEY_DOWN_RELEASE
#  9 = KEY_LEFT_RELEASE
# 10 = KEY_RIGHT_RELEASE
# 11 = KEY_ENTER_RELEASE
# 12 = KEY_EXIT_RELEASE
#
# In the Moment we ignore als Response-Codes from CF635
#----------------------------------------------------------------

sub decodepkt 
{
	my $data=shift;
	my @pkts;
	my $pktcode;
	my $pktlen;
	my $pktcrc;
	my $pktdata='';
	my $i;
	my $char;

	if ( $data ne '')
	{
		foreach $char (split(//,$data))
		{
			push @pkts,$char;
		}

		$pktcode=$pkts[0];
		$pktlen=$pkts[1];

		for ($i=2;$i<$#pkts-1;$i++)
		{
			$pktdata.=$pkts[$i];
		}

		$pktcrc=$pkts[@pkts-2..@pkts];

		if ( genpkt(ord($pktcode),$pktdata) ne $data)
		{
			return 99;
		}
	}
	
	if (ord($pktcode) == 128)
	{
		return ord($pktdata);
	}
	else
	{
		return 99;
	}
}

#----------------------------------------------------------------
# Make sure a packet is valid (crc check) and decode it by providing useful info
# Input: a complete packet, and only one. Unserialize first, with splitpkt
# 
# For Debugging  
#----------------------------------------------------------------

sub decodepkt_print {
	my $data=shift;
	my @pkts;
	my $pktcode;
	my $pktlen;
	my $pktcrc;
	my $pktdata='';
	my $i;
	my $char;
	#print "\nDecoding\n";

	if ( $data ne '')
	{
		foreach $char (split(//,$data))
		{
			push @pkts,$char;
		}

		$pktcode=$pkts[0];
		$pktlen=$pkts[1];

		for ($i=2;$i<$#pkts-1;$i++)
		{
			$pktdata.=$pkts[$i];
		}

		$pktcrc=$pkts[@pkts-2..@pkts];

		#print "Packet code:".str2hex($pktcode)."Packet len:".ord($pktlen)." Packet crc:".str2hex($pkts[@pkts-2])."".str2hex($pkts[@pkts-1])."\n";
		#print "Packet data:".str2hex($pktdata)." ";

		# warn ("Error occured while decoding packet. Unserialize before ?". str2hex($pktdata)."\n") if ( genpkt(ord($pktcode),$pktdata) ne $data);

		if ( genpkt(ord($pktcode),$pktdata) ne $data)
		{
			warn ("Error occured while decoding packet. Unserialize before ?". str2hex($pktdata)."\n");
		}

		if  ( ord($pktcode)>>6 == 1)
		{
			print "Normal response packet from CF635\n";
		}
		elsif ( ord($pktcode)>>6 == 2)
		{
			print "Normal report fromCF635\n";
		}
		elsif ( ord($pktcode)>>6 == 3)
		{
			print "Error reported from CF635\n";
		}
	}
	
	if (ord($pktcode) == 128)
	{
		if (ord($pktdata)==1){print "KEY_UP_PRESS\n"}
		elsif (ord($pktdata)==2){print "KEY_DOWN_PRESS\n"}
		elsif (ord($pktdata)==3){print "KEY_LEFT_PRESS\n"}
		elsif (ord($pktdata)==4){print "KEY_RIGHT_PRESS\n"}
		elsif (ord($pktdata)==5){print "KEY_ENTER_PRESS\n"}
		elsif (ord($pktdata)==6){print "KEY_EXIT_PRESS\n"}
		elsif (ord($pktdata)==7){print "KEY_UP_RELEASE\n"}
		elsif (ord($pktdata)==8){print "KEY_DOWN_RELEASE\n"}
		elsif (ord($pktdata)==9){print "KEY_LEFT_RELEASE\n"}
		elsif (ord($pktdata)==10){print "KEY_RIGHT_RELEASE\n"}
		elsif (ord($pktdata)==11){print "KEY_ENTER_RELEASE\n"}
		elsif (ord($pktdata)==12){print "KEY_EXIT_RELEASE\n"}
	}
}

#----------------------------------------------------------------
# Print a Text on the Display at Position x=col, y=line
# Screen starts with 0,0 at upper left Corner
#
#      Col                 1 1 1 1 1 1 1 1 1 1
# Line 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
#      1
#      2
#      3
#----------------------------------------------------------------

sub printxy
{
	my ($col,$line,$text) = @_;

	$result=sendpkt($ob,genpkt(31,chr($col).chr($line).$text));
}

#----------------------------------------------------------------
# Sends a Ping to the Display. For Testing
#----------------------------------------------------------------

sub ping
{
	print "\nSending ping packet\n";
	$result=sendpkt($ob,genpkt(0x00,chr(255)));
	decodepkt_print($result);
	#print "Result:".str2hex($result)."\n";
	#$result=sendpkt($ob,hex2str("00 01 FF 6C D0"));#ping packet (hand crafted)
	#decodepkt_print($result);print "Result:".str2hex($result)."\n";
}

#----------------------------------------------------------------
# Gets the Version of the Display
#----------------------------------------------------------------

sub version
{
	print "\nSending version packet\n";
	$result=sendpkt($ob,genpkt(0x01,''));
	decodepkt_print($result);
	print "Result in hex:".str2hex($result)."\n";
	print "Result in str:".($result)."\n";

	#$result=sendpkt($ob,hex2str("01 00 9F 16"));#get version packet (hand crafted)
	#decodepkt_print($result);
	#print "Result:".str2hex($result)."\n";
}

#----------------------------------------------------------------
# Clears the Display
#----------------------------------------------------------------

sub clear_lcd
{
	$result=sendpkt($ob,genpkt(06,''));
	decodepkt($result);
}

#----------------------------------------------------------------
# Sets the Contrast 
# Good Value for the Blue Display: 110
#----------------------------------------------------------------

sub contrast
{
	my ($contrast)=@_;

	# Check the Value 
	$contrast = 100 if (($contrast > 255) || ($contrast < 0));

	$result=sendpkt($ob,genpkt(13,chr($contrast)));
}

#----------------------------------------------------------------
# Sets the Backlight
# Good Value for the Blue Display: 25
#----------------------------------------------------------------

sub backlight
{
	my ($backlight)=@_;

	# Check the Value 
	$backlight = 50 if (($backlight > 100) || ($backlight < 0));

	$result=sendpkt($ob,genpkt(14,chr($backlight)));
}

#----------------------------------------------------------------
# Sets the Key Reporting
# Parameter: Keypress mask
#            Keyrelease mask
#
# Bit 0 = KP_UP
# Bit 1 = KP_ENTER
# Bit 2 = KP_CANCEL
# Bit 3 = KP_LEFT
# Bit 4 = KP_RIGHT
# Bit 5 = KP_DOWN
# Bit 6 = N/A
# Bit 7 = N/A
#
# Example: press_mask -> 63 -> 0011 1111: All Keypresses will 
#          be reported
# Example: release_mask -> 0 -> 0000 0000: All Keyreleases will 
#          be supressed
#----------------------------------------------------------------

sub Set_Key_Reporting_Mask
{
	my ($press_mask,$release_mask)=@_;
	$result=sendpkt($ob,genpkt(23,chr($press_mask). chr($release_mask)));
}

#----------------------------------------------------------------
# Sets the Cursor Style
# Parameter: Style
# 0 = no cursor
# 1 = blinking block cursor
# 2 = underscore cursor
# 3 = blinking block cursor with underscore
# 4 = inverting, blinking block
#----------------------------------------------------------------

sub set_cursor_style
{
	my ($style)=@_;

	# Check the Value 
	$style = 0 if (($style > 4) || ($style < 0));

	$result=sendpkt($ob,genpkt(12,chr($style)));
}

#----------------------------------------------------------------
# Toggle the LED's
# Parameter: LED, Value
#----------------------------------------------------------------

sub toggle_led
{
	my ($led, $value)=@_;

	$result=sendpkt($ob,genpkt(34,chr($led).chr($value)));
}
