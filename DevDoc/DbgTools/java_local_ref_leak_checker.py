#!/bin/bash

import os
import sys

FILE_NAME = ""
KEYWORDS = ["jclass"]
DELETE_KEYWORD = "env->DeleteLocalRef("
RETURN_KEYWORD = "return"

def read_file(file):
    lines = []
    fd = open(file, 'r')
    for line in fd:
        lines.append(line)
    fd.close()
    return lines

# def unfold_block(lines):
#     currentLineNumber = 0
#     while currentLineNumber < len(lines):
#         line = lines[currentLineNumber]
#         index = line.find("{")
#         if (index == -1):
#             index = line.find("}")
#         if (index != -1 and index < len(line) - 2):
#             lines.insert(currentLineNumber + 1, line[(index + 1):len(line)])
#             line = line[0:(index + 1)]
#             lines[currentLineNumber] = line
#         currentLineNumber = currentLineNumber + 1

def dump_local_ref_leak(leaks):
    for localRefLeak in leaks:
        print localRefLeak
    return

def check_define_local_ref_(lines, lineNumber, leaks):
    global FILE_NAME
    global KEYWORDS
    global DEBUG
    line = lines[lineNumber]
    for keyWord in KEYWORDS:
        index = line.find(keyWord)
        if (index == -1):
            continue
        if (line.find("[in]") != -1 or line.find("[out]") != -1 or \
            lines[lineNumber + 1].find("[in]") != -1 or lines[lineNumber + 1].find("[out]") != -1):
            return True
        if (line[index + len(keyWord)] == " "):
            origLine = line
            line = line[(index + len(keyWord)):len(line)]
            line = line.lstrip()
            index = line.find(" ")
            if (index == -1):
                index = line.find(";")
            variable = line[0:index]
            leaks.append((FILE_NAME, lineNumber + 1, variable, origLine))
            return True
    return False

def check_return_local_ref(lines, lineNumber, leaks):
    line = lines[lineNumber]
    index = line.find(RETURN_KEYWORD)
    if (index == -1):
        return False
    line = line[index + len(RETURN_KEYWORD):len(line)]
    line = line.lstrip()
    variable = line[0:line.find(";")]
    leakNumber = len(leaks) - 1
    while (leakNumber >= 0):
        refDef = leaks[leakNumber]
        if (refDef[2] == variable):
            del leaks[leakNumber]
            return True
        leakNumber =  leakNumber - 1
    return True

def check_delete_local_ref(lines, lineNumber, leaks):
    line = lines[lineNumber]
    index = line.find(DELETE_KEYWORD)
    if (index == -1):
        return False
    line = line[index + len(DELETE_KEYWORD):len(line)]
    line = line.lstrip()
    variable = line[0:line.find(")")]
    leakNumber = len(leaks) - 1
    while (leakNumber >= 0):
        refDef = leaks[leakNumber]
        if (refDef[2] == variable):
            del leaks[leakNumber]
            return True
        leakNumber =  leakNumber - 1
    return True

def check_use_local_ref(lines, lineNumber, leaks):
    return False

def check_line(lines, lineNumber, leaks):
    if (check_define_local_ref_(lines, lineNumber, leaks)):
        return
    elif (check_delete_local_ref(lines, lineNumber, leaks)):
        return
    elif (check_use_local_ref(lines, lineNumber, leaks)):
        return
    else :
        check_return_local_ref(lines, lineNumber, leaks)

def check_block(lines, currentLineNumber):
    totalLineNumber = len(lines)
    currentLineNumber = currentLineNumber + 1
    leaks = []
    while (currentLineNumber < totalLineNumber):
        line = lines[currentLineNumber]
        if (line.find("{") != -1):
            currentLineNumber = check_block(lines, currentLineNumber)
        elif (line.find("}") != -1):
            dump_local_ref_leak(leaks)
            return currentLineNumber
        elif (line.find("//") == -1):
            check_line(lines, currentLineNumber, leaks)
        currentLineNumber = currentLineNumber + 1
    return currentLineNumber

def check_leak(lines):
    if (len(lines) <= 0):
        return
    currentLineNumber = 0
    for line in lines:
        if (line.find("{") != -1):
            break
        currentLineNumber = currentLineNumber + 1
    check_block(lines, currentLineNumber)

def check_file(path):
    global FILE_NAME
    FILE_NAME = path
    lines = read_file(path)
    # unfold_block(lines)
    check_leak(lines)

def process(path):
    if (os.path.isfile(path)):
        index = path.rfind("/")
        if index == -1:
            fileName = path
        else:
            fileName = path[(index + 1):len(path)]
        if (fileName.rfind(".cpp") != -1):
            check_file(path)
    else:
        entries = os.listdir(path)
        for entry in entries:
            if (entry == "." or entry == ".."):
                continue
            entryPath = path + "/" + entry
            process(entryPath)

process(sys.argv[1])
