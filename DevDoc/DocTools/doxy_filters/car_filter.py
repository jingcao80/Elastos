#!/usr/bin/python
import getopt          # get command-line options
import os.path         # getting extension from file
import string          # string manipulation
import sys             # output and stuff
import re              # for regular expressions

#stream to write output to
outfile = sys.stdout

interfaces = []
class_name = ""

#regular expression
re_class = re.compile(r"(\s*)(class)(\s*)(\S*)")
re_singleton = re.compile(r"(\s*)(singleton)(\s*)(class)(\s*)(\S*)")
re_interface = re.compile(r"(\s*)(interface)([^;]*)(?=;)")
re_module = re.compile(r"(\s*)(module)(\s*)((?={)|(?=\s*$))")
re_semicolon = re.compile(r"(\s*)(})(\s*)")
#filters .car-files

def semicolonAdd(s):
    global re_semicolon
    global class_name
    global interfaces

    match_semicolon = re_semicolon.match(s)
    if match_semicolon is not None:
        #found '}'
        if class_name.strip():
            if not interfaces:
                class_name = ""
                return re_semicolon.sub(r"\1\2;\3",s)
            else:
                s2 = re_semicolon.sub(r"\1\2;\3",s)
                s2 = s2 + "\n/**\n* @class " + class_name + "\n* This class implements these interfaces:\n"
                for k in interfaces:
                    s2 = s2 + "*" + k
                s2 = s2 + "*/\n"
                class_name = ""
                interfaces = []
                return s2
        else:
            interfaces = []
            return re_semicolon.sub(r"\1\2;\3",s)
    else: return s
def filterCAR(filename):
    global outfile
    global class_name
    global re_interface
    global re_module
    global re_class
    global re_singleton
    global interfaces
    f = open(filename)
    r = f.readlines()
    f.close()
    #outfile.write("\n//processed by filterCAR \n")
    for s in r:
        #outfile.write(s+"\n")
        s = semicolonAdd(s)
        match_interface = re_interface.match(s)
        match_module = re_module.match(s)
        match_class = re_class.match(s)
        match_singleton = re_singleton.match(s)
        if match_interface is not None:
            #found interface
            #outfile.write(re_interface.sub(r"\1\3 \2",s))
            if class_name.strip():
                interfaces.append(s)

            outfile.write(s)
        elif match_module is not None:
            #found module
            #outfile.write(match_module.group(0)+" com.elastos {\n")
            outfile.write(re_module.sub(r"\1\2 _",s))
        elif match_class is not None:
            class_name = match_class.group(4)
            #outfile.write("\n[" + match_class.group(0) + "|" + match_class.group(1) + "|" + match_class.group(2) + "]\n")
            outfile.write(s)
        elif match_singleton is not None:
            class_name = match_singleton.group(6)
            #outfile.write("\n[" + match_class.group(0) + "|" + match_class.group(1) + "|" + match_class.group(2) + "]\n")
            outfile.write(s)
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

