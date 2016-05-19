# synchronized_checker.py
# encoding: UTF-8
# usages:
#        sudo chmod a+x synchronized_checker.py
#        python synchronized_checker.py scan_path log_filepath

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
    return (len(line) > 0) and (line.startswith("//") == False) \
    and (line.startswith("*") == False) and (line.startswith("/*") == False)

def find_for_range(lines, index):
    matchCount = 0;
    totalNum = len(lines)
    startLineNum = 0
    endLineNum = 0
    for index in range(index, totalNum):
        line = lines[index]
        if isCodeLine(line) and line.endswith('{'):
            startLineNum = index
            break;

    if startLineNum - index > 10:
        return [0, 0]

    for index in range(startLineNum , totalNum):
        line = lines[index]
        if isCodeLine(line):
            if line.startswith('}') and line.endswith('{'):     # case: } else {
                pass
            elif line.find('{') != -1 and line.find('}') != -1 and line.find('{') < line.find('}'): # case if (bval) { do something }
                pass
            elif line.endswith('{'):
                matchCount = matchCount + 1
            elif line.endswith("}"):
                matchCount = matchCount - 1
                if matchCount == 0:
                    endLineNum = index
                    break

    return [startLineNum, endLineNum]

def log_fine_info(logFile, firstLog, path):
    if firstLog:
        firstLog = False
        logInfo ='\n>> process file: ' + path + '\n'
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
            elif line.startswith('= warning:') == True:
                warningCount = warningCount + 1
        logFile.close()

        # log
        logFile = open(logPath, 'a')
        logInfo = '\nresults: {0:d} errors, {1:d} warnings.'.format(errorCount, warningCount)
        logFile.write(logInfo)
        print logInfo
        logFile.close()


def process_file(path, logFile):
    if path.endswith('.cpp') == False:
        return

    lines = read_file(path)
    totalNum = len(lines)
    lineNum = 0;
    firstLog = True
    while lineNum < totalNum:
        line = lines[lineNum]
        if isCodeLine(line):
            pattern = re.compile(r'(for\s*\()(.*;.*)')
            match = pattern.search(line)
            if match == None:
                pattern = re.compile(r'( while\s*\()(.*\)\s*{)')
                match = pattern.search(line)
            if match == None:
                pattern = re.compile(r' do\s*{')
                match = pattern.search(line)
            if match == None:
                pattern = re.compile(r' switch\s*\(.*\)')
                match = pattern.search(line)
            if match:
                forRange = find_for_range(lines, lineNum)
                start = forRange[0]
                end = forRange[1]
                if start == 0 or end == 0:
                    lineNum = lineNum + 1
                else:
                    for num in range(start, end):
                        forLine = lines[num]
                        if isCodeLine(forLine) and forLine.startswith('synchronized'):
                            firstLog = log_fine_info(logFile, firstLog, path)
                            logInfo ='   > error: invalid using of synchronized inside for block at line {0:d} : {1}\n'.format(num + 1, forLine)
                            logFile.write(logInfo)
                            print logInfo
                        num = num + 1
                    lineNum = end
            else:
                lineNum = lineNum + 1
        else:
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
log_filepath = 'synchronized_checker.log'

print "python  :", sys.argv[0]
argc = len(sys.argv)
if argc == 3:
    scan_path = sys.argv[1]
    log_filepath = sys.argv[2]

print "process :", scan_path
print "log to  :", log_filepath
process(scan_path, log_filepath)
