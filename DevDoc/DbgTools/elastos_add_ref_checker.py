# elastos_add_ref_checker.py
# encoding: UTF-8
# usages:
#        sudo chmod a+x elastos_add_ref_checker.py
#        python elastos_add_ref_checker.py scan_path log_filepath

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
    lines.append('end')
    return lines

def isCodeLine(line):
    return (len(line) > 0) and (line.startswith("//") == False)

def find_function_range(lines, index):
    matchCount = 0;
    totalNum = len(lines)
    startLineNum = 0
    endLineNum = 0
    for index in range(index, totalNum):
        line = lines[index]
        if isCodeLine(line) and line.endswith('{'):
            startLineNum = index
            break;

    for index in range(startLineNum , totalNum):
        line = lines[index]
        if isCodeLine(line):
            if line.startswith('}') and line.endswith('{'):     # case: } else {
                pass
            # elif line.find('{') != -1 and line.find('}') != -1 and line.find('{') < line.find('}'): # case if (bval) { do something }
            #     pass
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

def check_add_ref(logFile, firstLog, path, lines, lineNum, match):
    #print 'found {0}** {1} at line {2:d}'.format(paramType, paramName, lineNum + 1)
    paramType = match.group(2)
    paramName = match.group(4)
    funRange = find_function_range(lines, lineNum)
    start = funRange[0]
    end = funRange[1]
    total = len(lines)

    if (end == total - 1):
        firstLog = log_fine_info(logFile, firstLog, path)
        logInfo = ' = warning: find function scope lines({0:d}, {1:d}) for {2}** {3} reached file end. may be wrong!\n'.format(start + 1, end + 1, paramType, paramName)
        logFile.write(logInfo)
        print logInfo

    #print 'function scope lines:({0:d}, {1:d})'.format(start + 1, end + 1)
    checkLineNum = 0
    for index in range(start , end):
        line = lines[index]
        if isCodeLine(line):
            pattern = re.compile(r'(\*'+paramName+'\s*=\s*)(.*)')
            match = pattern.search(line)
            if match and line.startswith('*'):
                value=match.group(2)
                if value.startswith('NULL;') == False and isIgnored(path, paramType, paramName, value) == False:
                    if checkLineNum != 0:
                        firstLog = log_fine_info(logFile, firstLog, path)
                        logInfo = ' > error: assignment at line {0:d} for {1}** {2} needs AddRef!\n'.format(checkLineNum+1, paramType, paramName)
                        logFile.write(logInfo)
                        print logInfo
                        #log_ignored(path, paramType, paramName, value)
                    checkLineNum = index
            elif checkLineNum != 0:
                pattern = re.compile(r'REFCOUNT_ADD\(\*'+paramName+'\)')
                match = pattern.search(line)
                if match:
                    checkLineNum = 0
                else:
                    pattern = re.compile(r'\(\*'+paramName+'\)->AddRef\(\);')
                    match = pattern.search(line)
                    if match:
                        checkLineNum = 0

    if checkLineNum != 0:
        firstLog = log_fine_info(logFile, firstLog, path)
        logInfo = ' > error: assignment at line {0:d} for {1}** {2} needs AddRef!\n'.format(checkLineNum+1, paramType, paramName)
        logFile.write(logInfo)
        print logInfo

    return firstLog

def process_file(path, logFile):
    if path.endswith('.cpp') == False:
        return

    firstLog = True;
    lines = read_file(path)
    lineNum = 0
    for eachLine in lines:
        if isCodeLine(eachLine):
            paramType = ''
            paramName = ''
            pattern = re.compile(r'(/\*\s*\[out\]\s*\*/\s*)(\w*)(\s*\*\*\s*)(\w*)(\s*)([,)])')
            match = pattern.search(eachLine)
            if match == None:
                pattern = re.compile(r'(/\*\s*\[out,\s*callee\]\s*\*/\s*)(ArrayOf<\w*\*>)(\s*\*\*\s*)(\w*)(\s*)([,)])')
                match = pattern.search(eachLine)
            if match:
                firstLog = check_add_ref(logFile, firstLog, path, lines, lineNum, match)
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
                # skip bak dirs
                if filename != 'bak':
                    process_dir(filepath, logFile)
        elif(os.path.isfile(filepath)):
            process_file(filepath, logFile)

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
        logInfo = '\n\nresults: {0:d} errors, {1:d} warnings.'.format(errorCount, warningCount)
        logFile.write(logInfo)
        print logInfo
        logFile.close()

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

def get_finename(path):
    index = path.rfind('/');
    if index != -1:
        index = path.rfind('/', 0, index)
    if index != -1:
        filename = path[index:len(path)]
    else:
        filename = path
    return filename

def log_ignored(path, usedType, param, value):
    key = generate_ignored_key(path, usedType, param, value)
    if key in ignored_list:
        return

    ignored_file = '/home/kesalin/Elastos5/DevDoc/DbgTools/elastos_add_ref_checker.ignored'
    logFile = open(ignored_file, 'a')
    logFile.write('    \'' + key + '\',\n')
    logFile.close()

def generate_ignored_key(path, usedType, param, value):
    filename = get_finename(path)
    key = '{0}#{1}#{2}#{3}'.format(filename, usedType, param, value);
    return key

def isIgnored(path, usedType, param, value):
    key = generate_ignored_key(path, usedType, param, value)
    return key in ignored_list

############################################################################################################
ignored_list = [
    '/elapi/elapi.cpp#void#package#itfPack;',
    '/elapi/elapi.cpp#void#package#itfPack;',
    '/reflection/CClsModule.cpp#TypeDescriptor#orgTypeDesc#mTypeAliasList[typeDype->mIndex].mOrgTypeDesc;',
    '/android_linux/stub.cpp#CIClassInfo#classInfo#mInfo;',
    '/android_linux/proxy.cpp#CIClassInfo#classInfo#&(modInfo->mClasses[m]);',
    '/android_linux/proxy.cpp#IInterface#object#(IInterface *)&(mInterfaces[index].mVTPtr);',
    '/android_linux/proxy.cpp#CIClassInfo#classInfo#mInfo;',
    '/linux_rpc/stub.cpp#CIClassInfo#classInfo#mInfo;',
    '/linux_rpc/stub.cpp#IStub#stub#(IStub *)stubObj;',
    '/linux_rpc/proxy.cpp#CIClassInfo#classInfo#&(modInfo->mClasses[m]);',
    '/linux_rpc/proxy.cpp#IInterface#object#(IInterface *)&(mInterfaces[index].mVTPtr);',
    '/linux_rpc/proxy.cpp#CIClassInfo#classInfo#mInfo;',
    '/linux_rpc/proxy.cpp#IProxy#proxy#(IProxy*)proxyObj;',
    '/marshal/metainfos.cpp#CIClassInfo#classInfo#&(modInfo->mClasses[m]);',
    '/marshal/metainfos.cpp#CIModuleInfo#moduleInfo#modInfo;',
    '/marshal/metainfos.cpp#CIClassInfo#clsInfo#&(modInfo->mClasses[m]);',
    '/elcontainer/elcontainer.cpp#DLinkNode#node#mCurrent;',
    '/elrefbase/elrefbase.cpp#IInterface#objectReference#mObject->Probe(riid);',
    '/clsmodule/reloc.cpp#CLSModule#outDest#destModule;',
    '/regex/Pattern.cpp#RegexPattern#result#regexP;',
    '/regex/CMatcher.cpp#RegexMatcher#result#pattern->matcher(status);',
]
############################################################################################################

#results: 121 errors, 0 warnings.
scan_path = '/home/kesalin/Elastos5/Sources/'                                           # default sacn dir
log_filepath = '/home/kesalin/Elastos5/DevDoc/DbgTools/elastos_add_ref_checker.log'     # default log file path

print "python  :", sys.argv[0]
argc = len(sys.argv)
if argc == 3:
    scan_path = sys.argv[1]
    log_filepath = sys.argv[2]

print "process :", scan_path
print "log to  :", log_filepath
process(scan_path, log_filepath)
