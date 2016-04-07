# elastos_cast_checker.py
# encoding: UTF-8

import re
import os
import sys

def read_file(path):
    lines = []
    if (path.endswith('.cpp') or path.endswith('.h')):
        if(os.path.isfile(path)):
            handle = open(path, 'r')
            for line in handle:
                lines.append(line.strip())
            handle.close()
    return lines

def find_declare_line(usedType, param, lines, lineIndex):
    if len(lines) == 0:
        return -1

    for i in range(lineIndex, 0, -1):
        eachLine = lines[i]
        if (len(eachLine) > 1) and (eachLine.startswith("//") == False):

            pattern = re.compile(r'AutoPtr<'+usedType+'>\s*(.*)'+param+'[; ,]')
            match = pattern.search(eachLine)
            if match == None:
                pattern = re.compile(r''+usedType+'\s*\*(.*)'+param+'[; ,]')
                match = pattern.search(eachLine)
            if match:
                #print eachLine, 'match', match.group()
                return i;

    for i in range(lineIndex, 0, -1):
        eachLine = lines[i]
        if (len(eachLine) > 1) and (eachLine.startswith("//") == False):
            pattern = re.compile(r'[>]\s\s*'+param+'[; ,]')
            match = pattern.search(eachLine)
            if match == None:
                pattern = re.compile(r'[,*]\s*'+param+'[; ,]')
                match = pattern.search(eachLine)
            if match == None:
                pattern = re.compile(r'[,*]'+param+'[; ,]')
                match = pattern.search(eachLine)
            if match:
                #print eachLine, 'match', match.group()
                return i;
    return -1

def process_declare_line_in_header(filepath, logFile, firstLog, match, lines, lineNum):
    headerFilepath = filepath.replace("/src/", "/inc/").replace(".cpp", ".h")
    headerLines = read_file(headerFilepath)

    usedType = match.group(2)
    paramName = match.group(4)
    matchInfo = match.group()
    declLineNum = find_declare_line(usedType, paramName, headerLines, len(headerLines)-1)
    if (declLineNum != -1):
        declLine = headerLines[declLineNum]

        pattern = re.compile(r'AutoPtr<'+usedType+'>\s*(.*)'+paramName+'[; ,]')
        match = pattern.search(declLine)
        if match == None:
            pattern = re.compile(r''+usedType+'\s*\*(.*)'+paramName+'[; ,]')
            match = pattern.search(declLine)

        if match == None:
            if firstLog:
                firstLog = False
                logInfo ='\n>> process file: ' + filepath + '\n'
                logFile.write(logInfo)
                print logInfo
            logInfo = "   > error: invalid using of {0} at line {1:d}, it is declared as {2} in .h file at line {3:d}.\n".format(matchInfo, lineNum + 1, declLine, declLineNum + 1)
            logFile.write(logInfo)
            print logInfo
        else:
            #print 'match ', matchInfo, declLine
            return
    else:
        if firstLog:
            firstLog = False
            logInfo ='\n>> process file: ' + filepath + '\n'
            logFile.write(logInfo)
            print logInfo
        logInfo = "   = warning: declaration for {0} at line {1:d} not found!\n".format(matchInfo, lineNum + 1)
        logFile.write(logInfo)
        print logInfo

def process_file(path, logFile):
    firstLog = True;
    lines = read_file(path)
    lineNum = 0
    for eachLine in lines:
        if (len(eachLine) > 1) and (eachLine.startswith("//") == False):
            pattern = re.compile(r'(\()(I\w*)(\*\*\)&)([a-zA-Z]\w*)(\))')
            match = pattern.search(eachLine)
            if match:
                #print match.group() match.groups()
                #print match.group(2), match.group(4)
                usedType = match.group(2)
                paramName = match.group(4)
                matchInfo = match.group()
                declLineNum = find_declare_line(usedType, paramName, lines, lineNum)
                if (declLineNum != -1):
                    declLine = lines[declLineNum]
                    #print 'declLine', declLine

                    pattern = re.compile(r'AutoPtr<'+usedType+'>\s*(.*)'+paramName+'[; ,]')
                    match = pattern.search(declLine)
                    if match == None:
                        pattern = re.compile(r''+usedType+'\s*\*(.*)'+paramName+'[; ,]')
                        match = pattern.search(declLine)

                    if match == None:
                        if firstLog:
                            firstLog = False
                            logInfo ='\n>> process file: ' + path + '\n'
                            logFile.write(logInfo)
                            print logInfo
                        logInfo = "   > error: invalid using of {0} at line {1:d}, it is declared as {2} at line {3:d}.\n".format(matchInfo, lineNum + 1, declLine, declLineNum + 1)
                        logFile.write(logInfo)
                        print logInfo
                    else:
                        #print 'match ', matchInfo, declLine
                        pass
                else:
                    process_declare_line_in_header(path, logFile, firstLog, match, lines, lineNum)
        lineNum = lineNum +1

def process_dir(path, logFile):
    listfile = os.listdir(path)
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
    print 'output to', logPath
    if(os.path.isdir(path)):
        process_dir(path, logFile)
    elif(os.path.isfile(path)):
        process_file(path, logFile)
    else:
        print 'invalid path:', path
    logFile.close()

#process('/home/kesalin/test/python/test.cpp', 'elastos_cast_checker.log')
#process('/home/kesalin/Elastos5/Sources/Elastos/Frameworks/Droid/Base/Core/src/', '/home/kesalin/elastos_cast_checker.log')
process('/home/kesalin/Elastos5/Sources/Elastos/Frameworks/Droid/Base/Core/src/elastos/droid/app', '/home/kesalin/elastos_cast_checker.log')
