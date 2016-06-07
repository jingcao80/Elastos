# car_reuse_variable.py
# encoding: UTF-8
# usages:
#        sudo chmod a+x car_reuse_variable.py
#        python car_reuse_variable.py scan_path log_filepath

import re
import os
import sys


def read_file(path):
    lines = []
    if(os.path.isfile(path)):
        handle = open(path, 'r')
        for line in handle:
            lines.append(line.strip())
        handle.close()
    return lines

def isCodeLine(line):
    return (len(line) > 0) and (line.startswith("//") == False)

def log_fine_info(logFile, firstLog, path):
    if firstLog:
        firstLog = False
        logInfo ='\n\n>> process file: ' + path
        logFile.write(logInfo)
        print logInfo
    return firstLog

def summarize_log(logPath):
    if(os.path.isfile(logPath)):
        errorCount = 0
        warningCount = 0

        # summarize
        logFile = open(logPath, 'r')
        for line in logFile:
            line = line.strip()
            if line.startswith('> error:') == True:
                errorCount = errorCount + 1
        logFile.close()

        # log
        logFile = open(logPath, 'a')
        logInfo = '\n\nresults: {0:d} errors, {1:d} warnings.\n'.format(errorCount, warningCount)
        logFile.write(logInfo)
        print logInfo
        logFile.close()


def process_file(path, logFile):
    if path.endswith('.cpp') == False:
        return

    lines = read_file(path)
    totalNum = len(lines)
    lineNum = 0
    firstLog = True
    isCarClass = False
    isCompleted = False
    while lineNum < totalNum:
        line = lines[lineNum].strip()
        if isCodeLine(line):
            pattern = re.compile(r'(.*)(\*\*\s*\)\s*\&)([a-zA-Z]\w*)(\s*\))')
            match = pattern.search(line)
            if match:
                param = match.group(3)
                #if line.startswith(param):
                if len(param) > 1:
                    pattern = re.compile(r''+param+'[, ](.*)(\*\*\s*\)\s*\&)'+param+'(\s*\))')
                    match = pattern.search(line)
                    if match:
                        firstLog = log_fine_info(logFile, firstLog, path)
                        logInfo = '\n   > error: reused variable {0} at line {1:d}'.format(param, lineNum + 1)
                        logFile.write(logInfo)
                        print logInfo

        lineNum = lineNum + 1

def process_dir(path, logFile):
    listfile = os.listdir(path)
    fileNum = len(listfile)
    for filename in listfile:
        filepath = path + '/' + filename
        if(os.path.isdir(filepath)):
            # exclude hidden dirs
            if(filename[0] == '.'):
                pass
            elif filename.startswith("bak"):
                pass
            else:
                process_dir(filepath, logFile)
        elif(os.path.isfile(filepath)):
            process_file(filepath, logFile)

def process(path, logPath):
    if(os.path.isfile(logPath)):
        os.remove(logPath)
    logFile = open(logPath, 'a')
    if(os.path.isdir(path)):
        process_dir(path, logFile)
    elif(os.path.isfile(path)):
        process_file(path, logFile)
    else:
        print 'invalid path:', path
    logFile.close()
    summarize_log(logPath)

##########################################################
scan_path = '/home/kesalin/Elastos5/Sources/'
log_filepath = 'car_reuse_variable.log'

scan_path = 'test.cpp'

print "python  :", sys.argv[0]
argc = len(sys.argv)
if argc == 3:
    scan_path = sys.argv[1]
    log_filepath = sys.argv[2]

print "process :", scan_path
print "log to  :", log_filepath
process(scan_path, log_filepath)
