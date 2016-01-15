# trans .def to .sym or .vs

$R_filename=$ARGV[0];
$TARGET_MAME=$R_filename;
$TARGET_MAME=~s/(\w+)\.def/$1/;
$SO_NAME=$TARGET_MAME;

################################################################################
# VS
open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
@array_file = <R_FILE>;
close(R_FILE);

$W_filename="$TARGET_MAME.vs";
open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
select W_FILE;

print "\{\n";
print "global\:\n";
foreach $line(@array_file){
    if ($line=~m/^\s*LIBRARY\s+(\w+).*\n/) {
        $SO_NAME=$line;
        $SO_NAME=~s/\./\_/g;
        $SO_NAME=~s/\_eco//;
        $SO_NAME=~s/\_dso//;
        $SO_NAME=~s/^\s*LIBRARY\s+(\w+).*\n/__$1/;
    }
    $line=~s/;/#/;
    $line=~s/^EXPORT/#EXPORT/;
    $line=~s/^LIBRARY/#LIBRARY/;
    $line=~s/^DESCRIPTION/#DESCRIPTION/;
    $line=~s/^(\s*\w+)/$1\; # /;
    print $line;
}
print "local\: \*\;\n";
print "\}\;\n";
close(W_FILE);

################################################################################
#SYM
open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
@array_file = <R_FILE>;
close(R_FILE);

$W_filename="$TARGET_MAME.sym";
open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
select W_FILE;

foreach $line(@array_file){
    $line=~s/\;\;/#/;
    $line=~s/^EXPORT/#EXPORT/;
    $line=~s/^LIBRARY/#LIBRARY/;
    $line=~s/^DESCRIPTION/#DESCRIPTION/;
    $line=~s/^(\s*\w+)/$1\ # /;
    print $line;
}
close(W_FILE);

################################################################################
#DEFSYM
#open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
#@array_file = <R_FILE>;
#close(R_FILE);
#
#$W_filename="$TARGET_MAME.defsym";
#open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
#select W_FILE;
#
#foreach $line(@array_file){
#    if ($line=~m/DATA/) {
#        if ($line=~m/^\s*(\w+)\s*\=\s*(\w+).*/) {
#            $line=~s/^\s*(\w+)\s*\=\s*(\w+).*/-Wl,-defsym,$1=$2 /;
#            print $line
#        }
#    }
#}
#close(W_FILE);

################################################################################
#DUMMY SO C
#open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
#@array_file = <R_FILE>;
#close(R_FILE);
#
#$W_filename="$TARGET_MAME\_dummy.c";
#open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
#select W_FILE;
#print "#define DEFINE_FUNC_SYM(x) void $SO_NAME\_##x() {}\n";
#print "#define DEFINE_DATA_SYM(x) int x;\n\n";
#foreach $line(@array_file) {
#    $line=~s/;/\/\//;
#    $line=~s/^EXPORT/\/\/EXPORT/;
#    $line=~s/^LIBRARY/\/\/LIBRARY/;
#    $line=~s/^DESCRIPTION/\/\/DESCRIPTION/;
#    if ($line=~m/^\s*(\w+)\s*\=\s*(\w+)\s*.*/) {
#        $line=~s/(\w+)\s*\=\s*(\w+)/DEFINE_FUNC_SYM\($1\)\; \/\/ $1=$2/;
#    } else {
#        $line=~s/^(\s*)(\w+)/$1\DEFINE_FUNC_SYM\($2\)\; \/\/ /;
#    }
#    if ($line=~m/DATA/) {
#        $line=~s/DEFINE_FUNC_SYM/DEFINE_DATA_SYM/;
#    }
#    print $line;
#}
#close(W_FILE);

################################################################################
#IMP C
#open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
#@array_file = <R_FILE>;
#close(R_FILE);

#$W_filename="$TARGET_MAME\_imp.c";
#open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
#select W_FILE;
#print "#define DEFINE_FUNC_SYM(x) \\\n";
#print "    __asm__(\".global \"#x\";\" \\\n";
#print "        \".align;\" \\\n";
#print "        #x\":\" \\\n";
#print "        \"ldr    pc, AddressOf\"#x\";\" \\\n";
#print "        \"AddressOf\"#x\": .long $SO_NAME\_\"#x\"\\n\") \n";
#print "    DEFINE_FUNC_SYM(dummy_function);\n";
#close(W_FILE);

##IMP SH
#$W_filename="$TARGET_MAME\_imp.sh";
#open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
#select W_FILE;
#print "rm -rf $TARGET_MAME\_imp\n";
#print "mkdir -p $TARGET_MAME\_imp\n";
#foreach $line(@array_file) {
#    $line=~s/;/\/\//;
#    $line=~s/^EXPORT/\/\/EXPORT/;
#    $line=~s/^LIBRARY/\/\/LIBRARY/;
#    $line=~s/^DESCRIPTION/\/\/DESCRIPTION/;
#    $line=~s/DllGetClassObject/\/\/DllGetClassObject/;
#    $line=~s/DllCanUnloadNow/\/\/DllCanUnloadNow/;
#    if ($line!~m/DATA/) {
#        if ($line=~m/^\s*(\w+).*/) {
#            $line=~s/^\s*(\w+).*/\$1 --redefine-sym dummy_function=$1 --redefine-sym AddressOfdummy_function=AddressOf$1 --redefine-sym $SO_NAME\_dummy_function=$SO_NAME\_$1 $TARGET_MAME\_imp.o $TARGET_MAME\_imp\/$SO_NAME\_$1\_imp.o/;
#            print $line;
#        }
#    }
#}
#close(W_FILE);

#IMP BAT
#open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
#@array_file = <R_FILE>;
#close(R_FILE);
#
#$W_filename="$TARGET_MAME\_imp.bat";
#open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
#select W_FILE;
#print "\@echo off\n";
#print "rmdir /s/q $TARGET_MAME\_imp > nul 2> nul\n";
#print "mkdir $TARGET_MAME\_imp\n";
#foreach $line(@array_file) {
#    $line=~s/;/\/\//;
#    $line=~s/^EXPORT/\/\/EXPORT/;
#    $line=~s/^LIBRARY/\/\/LIBRARY/;
#    $line=~s/^DESCRIPTION/\/\/DESCRIPTION/;
#    $line=~s/DllGetClassObject/\/\/DllGetClassObject/;
#    $line=~s/DllCanUnloadNow/\/\/DllCanUnloadNow/;
#    if ($line!~m/DATA/) {
#        if ($line=~m/^\s*(\w+).*/) {
#            $line=~s/^\s*(\w+).*/\%1 --redefine-sym dummy_function=$1 --redefine-sym AddressOfdummy_function=AddressOf$1 --redefine-sym $SO_NAME\_dummy_function=$SO_NAME\_$1 $TARGET_MAME\_imp.o $TARGET_MAME\_imp\/$SO_NAME\_$1\_imp.o/;
#            print $line;
#        }
#    }
#}
#close(W_FILE);

################################################################################
#DLL
open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
@array_file = <R_FILE>;
close(R_FILE);

################################################################################
#EXP C
open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
@array_file = <R_FILE>;
close(R_FILE);

$W_filename="$TARGET_MAME\_exp.c";
open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
select W_FILE;
print "#define DEFINE_FUNC_SYM(x) \\\n";
print "    void x(); \\\n";
print "    x();\n";
#print "    __asm__(\".global $SO_NAME\_\"#x\";\" \\\n";
#print "        \".align;\" \\\n";
#print "        \"$SO_NAME\_\"#x\":\" \\\n";
#print "        \"ldr    pc, AddressOf\"#x\";\" \\\n";
#print "        \"AddressOf\"#x\": .long \"#y\"\\n\") \n";
print "#define DEFINE_DATA_SYM(x) extern int x;(void)x\n";
print "void dummy_func()\n\{\n";


foreach $line(@array_file) {
    $line=~s/;/\/\//;
    $line=~s/^EXPORT/\/\/EXPORT/;
    $line=~s/^LIBRARY/\/\/LIBRARY/;
    $line=~s/^DESCRIPTION/\/\/DESCRIPTION/;
    $line=~s/^(\s*)(\w+)/$1\DEFINE_FUNC_SYM\($2\)\; \/\/ /;
    if ($line=~m/DATA/) {
        $line=~s/DEFINE_FUNC_SYM/DEFINE_DATA_SYM/;
    }
    print $line;
}
print "}\n\n";

close(W_FILE);
