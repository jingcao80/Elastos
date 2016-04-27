#!/usr/bin/python
import getopt          # get command-line options
import os.path         # getting extension from file
import string          # string manipulation
import sys             # output and stuff
import re              # for regular expressions

#stream to write output to
outfile = sys.stdout

#regular expression
re_interface = re.compile(r"(\s*)(interface)([^;]*)(?=;)")
re_module = re.compile(r"(\s*)(module)(\s*)((?={)|(?=\s*$))")
re_semicolon = re.compile(r"(.*)(})(.*)")
#filters .car-files

def semicolonAdd(s):
    global re_semicolon
    match_semicolon = re_semicolon.match(s)
    if match_semicolon is not None:
        #found '}'
        return re_semicolon.sub(r"\1\2;\3",s)
    else: return s
def filterCAR(filename):
    global outfile
    global re_interface
    global re_module
    f = open(filename)
    r = f.readlines()
    f.close()
    #outfile.write("\n//processed by filterCAR \n")
    for s in r:
        #outfile.write(s+"\n")
        s = semicolonAdd(s)
        match_intface = re_interface.match(s)
        match_module = re_module.match(s)
        if match_intface is not None:
            #found interface
            #outfile.write(re_interface.sub(r"\1\3 \2",s))
            outfile.write(s)
        elif match_module is not None:
            #found module
            #outfile.write(match_module.group(0)+" com.elastos {\n")
            outfile.write(re_module.sub(r"\1\2 _",s))
        else:
            #if it's no match just output
            outfile.write(s)

# dumps the given file
def dump(filename):
    f = open(filename)
    r = f.readlines()
    f.close()
    for s in r:
        sys.stdout.write(s)

## main filter-function ##
##
## this function decides whether the file is
## (*) car file
##
## and calls the appropriate function
def filter(filename, out=sys.stdout):
    global outfile
    outfile = out
    try:
        root, ext = os.path.splitext(filename)
        if (ext.lower() ==".car"):
            ## if it is a module call filterCAR
            filterCAR(filename)
        else:
            ## if it is an unknown extension, just dump it
            dump(filename)
        sys.stderr.write("OK\n")
    except IOError,e:
        sys.stderr.write(e[1]+"\n")

## main-entry ##
################

if len(sys.argv) != 2:
    print "usage: ", sys.argv[0], " filename"
    sys.exit(1)

# Filter the specified file and print the result to stdout
filename = sys.argv[1]
filter(filename)
sys.exit(0)

