#!/usr/bin/perl

# (c) 2005 Alex Bligh / Xara Group Ltd
#
# Combine one or more .xrc files
#
# Licensed under the GNU Public License - see LICENSE for
# more details

sub usage
{
    print STDERR <<EOF;

 bin2cpp.pl - make a binary file into a CPP file

 Usage:

 bin2cpp.pl [options] <inputfile> <outputfile>

 The CPP file has the following function of interest

    void MyFunctionName(void **pPtr, UINT32* pLength);

 This returns (in the void* pointed to by pPtr and the UINT32 pointed to by pLength)
 the address and length of the data. MyFunctionName can be a class name such as
 MyClass::MyFunction().

 Options:
   -f <name> or --function <name>) - use 'name' as the name of the function concerned
   --verbose                       - be very loud about it
   --help                          - display this message

EOF

    my $error = shift @_;

    if ($error)
    {
	die ($error);
    }
    else
    {
	exit (0);
    }
	
}

use strict;
use Getopt::Long;

my $help =0;
my $verbose = 0;
my $function="GetBinaryFileInfo";

GetOptions( "function|f=s" => \$function,
	    "verbose|v+" => \$verbose,
	    "help!" => \$help ) || usage ("Bad option");

if ($help || $#ARGV != 1)
{
    usage();
}

my $ifn=shift @ARGV;
my $ofn=shift @ARGV;


open (OUTPUT, ">$ofn") || die ("Cannot open ouput file $ofn: $!");

open (INPUT, $ifn) || die ("Cannot open input file $ifn: $!");
binmode(INPUT);

# Write the header out.
print OUTPUT "//\n// This file was automatically generated by bin2cpp.pl. Do not edit it\n//\n\n";
print OUTPUT "static BYTE binfile[] = {\n";

my $chunk=16;

my $buf;
my $got=0;

while ($got=read (INPUT, $buf, $chunk))
{
    my @bytes = unpack ("C*", $buf);
    my $b;
    foreach $b (@bytes)
    {
	printf OUTPUT "0x%.2x,",$b;
    }
    print OUTPUT "\n";
}

print OUTPUT "0};\n\n";
print OUTPUT "void $function (void **pPtr, UINT32 *pSize)\n";
print OUTPUT "{ *pPtr=(void *)&(binfile[0]); *pSize=sizeof(binfile)-1; return; }\n\n";

close OUTPUT;
close INPUT;


exit (0);

