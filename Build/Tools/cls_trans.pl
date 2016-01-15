# trans .cls to .clsresource

$R_filename=$ARGV[0];
$R_filepath=$ARGV[1];

$TARGET_NAME=$R_filename;
$TARGET_NAME=~s/(\w+).rc/$1/;

if ($R_filepath=~m/NA/){
    $R_filepath=$NANA;
}

open(R_FILE, $R_filename) || die "ERROR:can not open file $R_filename\n";
@array_file = <R_FILE>;
close(R_FILE);

$W_filename="$TARGET_NAME.cpp";

foreach $line(@array_file){
    if ($line=~m/^(\d+)\s+(\w+)\s+(\"?.+\"?)$/){
        $rsc_cls=$line;
        $rsc_cls=~s/^(\d+)\s+(\w+)\s+\"?([^\"\n]+)\"?\n?$/$3/;

        if(!$R_filepath) {
            if(-e $rsc_cls) {
				#@args1 = stat ($rsc_cls);
				#$size1 = $args1[7];
                system("Cls2C $rsc_cls $W_filename");
            }
        }
        else {
            if(-e $rsc_cls) {
                $rsc_path=$rsc_cls;
            }
            else {
                $rsc_path=$R_filepath;
                $rsc_path=~s/(.+)/$1$rsc_cls/;
            }
			#@args2 = stat ($rsc_path);
			#$size2 = $args2[7];

            if(-e $rsc_path) {
                system("Cls2C $rsc_path $W_filename");
            }
        }
    }
}

