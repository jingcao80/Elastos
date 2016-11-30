# trans .rc to .res

$R_filename=$ARGV[0];
$SourcesType=$ARGV[1];
$Sources=$ARGV[2];

################################################################################
#DLL
$W_filename="\_\_dllmain.cpp";
open(W_FILE, ">$W_filename") || die "ERROR:can not open file $W_filename\n";
select W_FILE;

print "#define DLL_PROCESS_ATTACH 1\n";
print "#define DLL_PROCESS_DETACH 0\n\n";
print "extern \"C\" int _DllMainCRTStartup(\n";
print "    void* hDllHandle,\n";
print "    unsigned int dwReason,\n";
print "    void* preserved);\n\n";

print "#if defined(_win32) || defined(_wince)\n";
print "#define WINAPI __stdcall\n";
print "  void _crtinit(void);\n";
print "  void _crtterm(void);\n";
print "  static void (*s_dummy_for_link_mark_c)(void) = _crtinit;\n";
print "#else\n";
print "  #define WINAPI\n";
print "#endif\n\n";
print "#if defined(_win32) && defined(_MSVC)\n";
print "extern void _crt_dll_setpointer();\n";
print "extern void _crt_dllexit();\n";
print "#endif\n\n";
print "extern \"C\" int DllMain(\n";
print "    void* hDllHandle,\n";
print "    unsigned int dwReason,\n";
print "    void* preserved);\n\n";
print "#ifndef _USE_MY_DLLMAIN_\n";
print "#ifndef _win32\n";
print "#define __stdcall\n";
print "#endif\n";
print "extern \"C\" int DllMain(\n";
print "    void* hDllHandle,\n";
print "    unsigned int dwReason,\n";
print "    void* preserved)\n";
print "{\n";
print "    return 1;\n";
print "}\n";
print "#endif // _USE_MY_DLLMAIN_\n\n";
print "int _DllMainCRTStartup(\n";
print "        void*  hDllHandle,\n";
print "        unsigned int   dwReason,\n";
print "        void*  lpreserved)\n";
print "{\n";
print "    int ret;\n\n";
print "   if (dwReason == DLL_PROCESS_ATTACH) {\n";
print "#if defined(_win32) && defined(_MSVC)\n";
print "        _crt_dll_setpointer();\n";
print "#endif\n\n";
print "#if !(defined (_linux) || defined (_openkode))\n";
print "        _crtinit();\n";
print "#endif\n";
print "    }\n\n";
print "    ret = DllMain(hDllHandle, dwReason, lpreserved);\n\n";
print "    if (dwReason == DLL_PROCESS_DETACH) {\n";
print "#if defined(_win32) && defined(_MSVC)\n";
print "        _crt_dllexit();\n";
print "#endif\n\n";
print "#if !(defined (_linux) || defined (_openkode))\n";
print "        _crtterm();\n";
print "#endif\n";
print "    }\n";
print "    return ret;\n";
print "}\n\n";
print "#ifdef _linux \n";
print "static __attribute((constructor)) void DllInitialize(void)\n";
print "{\n";
print "    _DllMainCRTStartup((void*)&DllInitialize, DLL_PROCESS_ATTACH, 0);\n";
print "}\n\n";
print "static __attribute((destructor)) void DllDestory(void)\n";
print "{\n";
print "    _DllMainCRTStartup((void*)&DllInitialize, DLL_PROCESS_DETACH, 0);\n";
print "}\n\n";
print "#endif \n\n";


close(W_FILE);

if ($SourcesType eq "ecx") {
    $BUILD_DATE=time();
    $CAR_CODE=0;
    $URN="com/elastos/$R_filename.ecx";
} elsif ($SourcesType eq "exe") {
    $BUILD_DATE=time();
    $CAR_CODE=0;
    $URN="com/elastos/$R_filename.exe";
} else {
    @array_source = split(/ /, $Sources);
    $tatol=0;
    $currentIndex=0;
    #if def, rc appear in the same sources,
    #produce the cpp at the last of them,
    #so check the current file is  the last of them, then goto EXIT.
    foreach $sourcesFile(@array_source) {
        if ($sourcesFile=~m/\.def/) {
            $defFile=1;
            $tatol=$tatol + 1;
            if ($SourcesType eq "def") {
                $currentIndex = $tatol;
            }
        }
        if ($sourcesFile=~m/\.car/) {
            $carFile=1;
    #        $tatol=$tatol + 1;
    #        if ($SourcesType eq "car") {
    #            $currentIndex = $tatol;
    #        }
        }
        if ($sourcesFile=~m/\.rc/) {
            $rcFile=1;
            $tatol=$tatol + 1;
            if ($SourcesType eq "rc") {
                $currentIndex = $tatol;
            }
        }
    }

    if ($tatol != $currentIndex) {
        goto EXIT;
    }
    ##################################
    if ($SourcesType eq "rc") {
        open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
        @array_file = <R_FILE>;
        close(R_FILE);
        $TARGET_NAME=$R_filename;
        $TARGET_NAME=~s/(\w+).rc/$1/;
        $REC_NAME=$TARGET_NAME;
    } else {
        if ($SourcesType eq "def") {
            $TARGET_NAME=$R_filename;
            $TARGET_NAME=~s/(\w+).car/$1/;
            $REC_NAME=$TARGET_NAME;
        } else {
            $TARGET_NAME=$R_filename;
            $TARGET_NAME=~s/(\w+).def/$1/;
            $REC_NAME=$TARGET_NAME;
        }
    }

    foreach $line(@array_file){
        if ($line=~m/^(\d+)\s+(\w+)\s+(\"?.+\"?)$/) {

            $rsc_cls=$line;
            $rsc_cls=~s/^(\d+)\s+(\w+)\s+\"?([^\"\n]+)\"?\n?$/$3/;
            $rsc_cls=~s/\\\\/\//g;
        }
        if ($rsc_cls=~m/.*\.cls$/) {
            $cls_path=$rsc_cls;
        }
    }

    #get the file we deal with
    if ($SourcesType eq "rc") {
        $SourcesFile=$cls_path;
    }
    if ($SourcesType eq "def") {
        $SourcesFile=$R_filename;
    }

    #get db, fp, urn from cls.
    if (-e $SourcesFile) {
         #get build date from cls
    #     if ($SourcesFile=~m/\/cygdrive/) {
    #         print "-------------------------------------------\n";
    #         system("carcode -b $SourcesFile > carcode.temp");
    #     } else {
    #         print "===========================================",$SourcesFile,"=========\n";
    #         system("echo `pwd`");
    #         system("ls -l");
    #         system("carcode -b $SourcesFile > carcode.temp");
    #     }
    #     open(F_CarCode, "carcode.temp") || die "ERROR can not open file carcode.temp\n";
    #     $carCodeline=<F_CarCode>;
    #     close(F_CarCode);

         #if there is no 'bd:' in line, is illegal.
    #     if ($carCodeline=~m/^(bd:)(.*)\r\n$/) {
    #         $BUILD_DATE=$2;
    #     } else {
    #         die "ERROR: can not find db:\n";
    #     }

         $BUILD_DATE=time();

         #get car code from cls
         if ($SourcesFile=~m/\/cygdrive/) {
             system("carcode -c  $SourcesFile > carcode.temp");
         } else {
             system("carcode -c  $SourcesFile > carcode.temp");
         }
         open(F_CarCode, "carcode.temp") || die "ERROR can not open file carcode.temp\n";
         $carCodeline=<F_CarCode>;
         close(F_CarCode);

         #if there is no 'fp:' in line, is illegal.
         if ($carCodeline=~m/^(fp:)(.*)\n$/) {
             $CAR_CODE=$2;
         } else {
             die "ERROR: can not find fp:\n";
         }

    #     $CAR_CODE=0;

         #get urn from cls
         if ($SourcesType eq "rc") {
             if ($SourcesFile=~m/\/cygdrive/) {
                 system("carcode -u $SourcesFile > carcode.temp");
             } else {
                 system("carcode -u  $SourcesFile > carcode.temp");
             }
             open(F_CarCode, "carcode.temp") || die "ERROR can not open file carcode.temp\n";
             $carCodeline=<F_CarCode>;
             close(F_CarCode);

             #if there is no 'urn:' in line, is illegal.
             if ($carCodeline=~m/^(urn:)(.*)\n$/) {
                 $URN=$2;
                 if (!($URN=~m/(.*)(KD | ecoloader | mobileServicesAdp)(.*)/ )) {
                    $URN =~ s/.dso/.eco/;
                 }
             } else {
                 die "ERROR: can not find urn:\n";
             }
         } else {
             $TARGET_NAME=~s/_*(.*)$/$1/;
             if ("KD" eq $TARGET_NAME or "ecoloader" eq $TARGET_NAME or "mobileServicesAdp" eq $TARGET_NAME) {
                $URN="com/elastos/$TARGET_NAME.dso";
             } else {
                $URN="com/elastos/$TARGET_NAME.eco";
             }
         }
         if (-e "carcode.temp") {
            system("rm carcode.temp");
         }
    }
}


#use for hold all dependence items come from CarDependence.temp, LibDependence.temp, RcDependence.temp.
#CarDependence.temp, LibDependence.temp, RcDependence.temp is come from version.pl.
@g_dep;

#flag for the current depenpdence item is already in the g_dep;
$g_beenExist=0;

#the dependence infomation size.
$dependenceDataSize=0;

#get dependence item from CarDependence.temp, LibDependence.temp, RcDependence.temp, and put these item into @g_dep array.
#calculate lenth of the dependence infomation in.
if (-e "CarDependence.temp") {
    open(CARDEP,"CarDependence.temp") || die "Error: can not find CarDependence.temp\n";
    @arrar_cardep=<CARDEP>;
    $test=<CARDEP>;
    close(CARDEP);

    foreach $cardepline(@arrar_cardep) {
        #trip new line.
        $cardepline=~s/^(.*)\r/$1/;

        #check the dependence item is dll.
        if (!($cardepline=~m/\.dso/)  and !($cardepline=~m/\.eco/)) {
            die "ERROR: $cardepline is not a depence type\n";
        }

        $g_beenExist=0;

#check if the current item is already in g_dep;
CHECKEXIST: foreach my $single(@g_dep) {
                if ($single eq $cardepline) {
                    $g_beenExist=1;
                    last CHECKEXIST;
                }
            }

        next if($g_beenExist);

        #add the current item size.
        $dependenceDataSize=$dependenceDataSize + (length $cardepline);
        $dependenceDataSize=$dependenceDataSize + 1;

        push(@g_dep, $cardepline);
    }
}

if (-e "RcDependence.temp") {
    open(CARDEP,"RcDependence.temp") || die "Error: can not find RcDependence.temp\n";
    @arrar_rcdep=<CARDEP>;
    close(CARDEP);

    foreach $rcdepline(@arrar_rcdep) {
        $rcdepline=~s/^(.*)\r/$1/;
        if (!($rcdepline=~m/\.dso/) and !($rcdepline=~m/\.eco/)) {
            die "ERROR: $rcdepline is not a depence type\n";
        }

        $g_beenExist=0;

CHECKEXIST: foreach my $single(@g_dep) {
                if ($single eq $rcdepline) {
                    $g_beenExist=1;
                    last CHECKEXIST;
                }
            }

        next if($g_beenExist);

        $dependenceDataSize=$dependenceDataSize + (length $rcdepline);
        $dependenceDataSize=$dependenceDataSize + 1;

        push(@g_dep, $rcdepline);
    }
}

#$W_filename="$TARGET_NAME" . "RES.cpp";
if (-e "__section.cpp") {
    die "recreate section.cpp\n"
}
$W_filename="__section.cpp";
open(W_FILE, ">$W_filename") || die "ERROR:can not open $W_filename\n";

select W_FILE;
$dependenceDataSize=$dependenceDataSize + 5;
$URN_LENGTH=length $URN;
$URN_LENGTH=$URN_LENGTH + 1;
    print "typedef struct _CarcodeStruct\{\n";
    print "    unsigned int    uMagic\;\n";
    print "    unsigned int    bDate\;\n";
    print "    unsigned int    cCode\;\n";
    print "    unsigned int    uSize\;\n";
    print "    const char      uRn[$URN_LENGTH]\;\n";
    print "    const char      dependence[$dependenceDataSize]\;\n";
    print "\}CarcodeStruct\;\n";
    print "\n";


    print "const CarcodeStruct DllCarCode __attribute__ ((used,__section__ (\"\.resource\"))) = \{\n";
    print "0x80402010\, ";
    print "0x0\, ";
    print "0x0\, ";
    print "$URN_LENGTH\, ";
    print "\"$URN\", ";
    print "\"";
    foreach $item(@g_dep) {
        print $item;
        print "\\0";
    }
    print "#BAB";
    print "\" ";
    print "\}\;\n";
    print "\n";
    print "int g_pDllCarCode = (int)&DllCarCode;\n";

close(W_FILE);


EXIT:
