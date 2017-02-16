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
re_module = re.compile(r"(\s*)(module)(\s*)({?)$")
re_class = re.compile(r"(\s*)(class)(\s*)(\w*)")
re_singleton = re.compile(r"(\s*)(singleton)(\s*)(class)(\s*)(\w*)")
re_interface = re.compile(r"(\s*)(interface)(\s*)(\w*)(;)")
re_lbracket = re.compile(r"(\s*)({)(\s*)$")
re_rbracket = re.compile(r"(\s*)(})(\s*)$")
#filters .car-files

def handleClass(line):
    global class_name
    global interfaces
    ret = line + "\n/**\n* @class " + class_name + "\n* This class implements these interfaces:\n"
    for itf in interfaces:
        ret = ret + "*" + itf
    ret = ret + "*/\n"
    class_name = ""
    interfaces = []
    return ret

def filterCAR(filename):
    global outfile
    global class_name
    global re_interface
    global re_module
    global re_class
    global re_singleton
    global interfaces
    STATE_NONE = 0
    STATE_MODULE = 1
    STATE_LBRACKET = 2
    STATE_RBRACKET = 3
    STATE_CLASS = 4
    f = open(filename)
    lines = f.readlines()
    f.close()
    # remove the last line which contains "}"
    total_line_number = len(lines)
    line_number = total_line_number - 1
    while (line_number >= 0):
        line = lines[line_number]
        match_rbracket = re_rbracket.match(line)
        if match_rbracket is not None:
            del lines[line_number]
            break
        else:
            line_number = line_number - 1
    # remove module and {
    interfaces = []
    state = STATE_NONE
    for line in lines:
        match_module = re_module.match(line)
        match_class = re_class.match(line)
        match_singleton = re_singleton.match(line)
        match_interface = re_interface.match(line)
        match_lbracket = re_lbracket.match(line)
        match_rbracket = re_rbracket.match(line)
        if match_module is not None:
            state = STATE_MODULE
            if (match_module.group(4) == "{"):
                state = STATE_LBRACKET
        elif match_class is not None:
            state = STATE_CLASS
            class_name = match_class.group(4)
            class_name.strip()
            outfile.write(line)
        elif match_singleton is not None:
            state = STATE_CLASS
            class_name = match_singleton.group(6)
            class_name.strip()
            outfile.write(line)
        elif match_interface is not None:
            if (class_name != ""):
                interfaces.append(line)
            outfile.write(line)
        elif match_lbracket is not None:
            if (state == STATE_MODULE):
                state = STATE_LBRACKET
            else:
                outfile.write(line)
        elif match_rbracket is not None:
            if (state == STATE_CLASS):
                line = handleClass(line)
                state = STATE_NONE
            outfile.write(line)
        else:
            outfile.write(line)

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

