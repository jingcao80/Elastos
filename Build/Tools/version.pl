#deal with *.rc file
if ($ARGV[0] eq "__Elastos_RC") {
    $VS_MIRORR_PATH = $ARGV[1];
    $VS_XDK_TARGETS = $ARGV[2];
    $VS_RC_NAME = $ARGV[3];
    $VS_XDK_RUNTIME_PLATFORM = $ARGV[4];

    #create a file to hold dependence dll's urn;
    open(RCDEP,">RcDependence.temp") || die "ERROR: can not open CRcDependence.temp\n";

    #open rc file to prase.
    open(TF, $VS_MIRORR_PATH . "/$VS_RC_NAME") || die "ERROR: can not open $VS_MIRORR_PATH/$VS_RC_NAME\n";
    my @array_file=<TF>;
    close(TF);

    #fild cls file path include rc file.
    foreach my $line(@array_file){
        if ($line=~m/^(\d+)\s+(\w+)\s+(\"?.+\"?)$/){
            $rc_path=$line;
            $rc_path=~s/^(\d+)\s+(\w+)\s+\"?([^\"\n]+)\"?\n?$/$3/;
            $rc_path=~s/\\\\/\//g;
        }
        if ($rc_path=~m/.*\.cls$/) {
            $cls_path=$rc_path;
        }
    }

    if (-e $cls_path) {
        #using carcode.exe to get dependce item for cls file, and put these item into cardependence.dep file..
        if ($cls_path=~m/\/cygdrive/) {
            system("carcode -d $cls_path > $VS_MIRORR_PATH/cardependence.dep");
        } else {
            system("carcode -d $cls_path > $VS_MIRORR_PATH/cardependence.dep");
        }

        #get dependence item from cardependence.dep file.
        open(TF,$VS_MIRORR_PATH . "/cardependence.dep") || die "ERROR can not find cardependence.dep\n";
        my @array_file=<TF>;
        close(TF);
        my $firstline = 0;
        foreach my $line(@array_file){
            if (0 == $firstline) {
                $firstline=1;
                #if the first line is "Dependences:", the result of carcode.exe is OK.
                next if ($line=~m/Dependences:/);
                die "ERROR: the first line in cardependence.dep is not Dependences:\n";
            }

            #trip new line.
            $line=~s/(.*)\r\n/$1/;

            #if dependence item is exist in the path of VS_XDK_TARGETS , get the dll's urn, and put into RcDependence.temp
            if (-e "$VS_XDK_TARGETS/$line") {
                $VS_FINDED="$VS_XDK_TARGETS/$line";
            } elsif (-e "$VS_XDK_RUNTIME_PLATFORM/$line") {
                $VS_FINDED="$VS_XDK_RUNTIME_PLATFORM/$line";
            } else {
                die "ERROR: can not find $VS_XDK_TARGETS/$line\n";
            }

            getURNAndWriteToTempFile("$VS_FINDED", \*RCDEP, 0);

        }
    }
    close(RCDEP);
}

#deal with *.car file
if ($ARGV[0] eq "__Elastos_CAR") {
    $VS_MIRORR_PATH = $ARGV[1];
    $VS_XDK_TARGETS = $ARGV[2];
    $VS_CLS_NAME = $ARGV[3];
    $VS_XDK_RUNTIME_PLATFORM = $ARGV[4];

    #get the corresponding cls file path.
    $VS_CLS_NAME=~m/^(.*)\/(.*)$/;
    $VS_CLS_NAME=$2;
    $VS_CLS_NAME=~s/\.car/\.cls/;

    $VS_RUNTIME_CLS="$VS_XDK_RUNTIME_PLATFORM/$VS_CLS_NAME";
    $VS_TARGET_CLS="$VS_CLS_NAME";
    $VS_RUNTIME_CLS=~s/\\/\//g;
    $VS_TARGET_CLS=~s/\\/\//g;

    if (-e $VS_RUNTIME_CLS) {
        $VS_CLS_NAME=$VS_RUNTIME_CLS;
    } elsif (-e $VS_TARGET_CLS) {
        $VS_CLS_NAME=$VS_TARGET_CLS;
    } else {
        die "ERROR: can not find file $VS_CLS_NAME\n";
    }


    #create a file to hold dependence dll's urn;
    open(CARDEP,">CarDependence.temp") || die "ERROR: can not open CarDependence.temp\n";


    #using carcode.exe to get dependence item from cls file, and put it into cardependence.dep;
    if ($VS_CLS_NAME=~m/\/cygdrive/) {
        system("carcode -d $VS_CLS_NAME > $VS_MIRORR_PATH/cardependence.dep");
    } else {
        system("carcode -d $VS_CLS_NAME > $VS_MIRORR_PATH/cardependence.dep");
    }

    #get dependence item from cardependence.dep.
    open(TF,$VS_MIRORR_PATH . "/cardependence.dep") || die "ERROR can not find cardependence.dep\n";
    my @array_file=<TF>;
    close(TF);
    my $firstline = 0;

    foreach my $line(@array_file){
        if (0 == $firstline) {
            $firstline=1;
            #if the first line is "Dependences:", the result of carcode.exe is OK.
            next if ($line=~m/Dependences:/);
            die "ERROR: the first line in cardependence.dep is not Dependences:\n"
        }

        #trip new line.
        #print "$line\n";
        #$line=~s/(.*)\r\n/$1/;
        #print "$line\n";
        #print "$VS_XDK_TARGETS/$line\n";
        #
        #if (-e "sources") {
        #    print "++++++++++\n";
        #}
        #
        ##if dependence item is exist in the path of VS_XDK_TARGETS , get the dll's urn, and put into RcDependence.temp
        #if (-e "$VS_XDK_TARGETS/$line") {
        #    $VS_FINDED = "$VS_XDK_TARGETS/$line";
        #} elsif (-e "$VS_XDK_RUNTIME_PLATFORM/$line") {
        #    $VS_FINDED = "$VS_XDK_RUNTIME_PLATFORM/$line";
        #} else {
        #    print "===========\n";
        #    die "ERROR: h can not find $line";
        #}

        #getURNAndWriteToTempFile("$VS_FINDED", \*CARDEP, 0);
    }



    close(CARDEP);
}


use File::Find;
$g_libFileFinded = 0;
sub checkCaseSensitive()
{
    my $libName = $_;
    if ($libName=~m/$g_currentLibFile/) {
        $g_libFileFinded = 1;
    }
}

####################################################
sub getURNAndWriteToTempFile ($$$)
####################################################
{
    my ($dependedDll, $w_file, $is_lib) = @_;
    my $line;
    if ($is_lib) {
        $line = "urn:com/elastos/" . $dependedDll . "\r\n";
    } else {
        printf "CInfoReader -u $dependedDll > temp.urn";
        system("CInfoReader -u $dependedDll > temp.urn");
        open(TF, "temp.urn");
        $line=<TF>;
        close(TF);
    }

    if ($line=~m/^urn:/) {
       $line=~s/^(urn:)(.*)$/$2/;
       select $w_file;
       print $line;
       select STDOUT;
    } else {
        die "ERROR: can not match 'urn:' in dependence\n";
    }
    if (!$is_lib){
        system("rm temp.urn");
    }
}

