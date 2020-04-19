#!/usr/bin/env perl
#
# vim:set ts=4 sw=4:

use strict;
my $dll="";

my @dll=<*.dll>;
my @date=localtime();
my $date=sprintf "%04d-%02d-%02d",$date[5]+1900,$date[4]+1,$date[3];

while (<>){
	s/%VER%/$date/;
	/%DLL/ && do {
		my $template=$_;
		my $out;
		$_=join("",map {($out=$template)=~s/%DLL%/$_/;$out} @dll);
	};
	print;
}
