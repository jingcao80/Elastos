#!/usr/bin/python

import os
import sys

elastosLicense = """/*
                 * Copyright (C) 2012 The Elastos Open Source Project
                 *
                 * Licensed under the Apache License, Version 2.0 (the "License");
                 * you may not use this file except in compliance with the License.
                 * You may obtain a copy of the License at
                 *
                 *      http://www.apache.org/licenses/LICENSE-2.0
                 *
                 * Unless required by applicable law or agreed to in writing, software
                 * distributed under the License is distributed on an "AS IS" BASIS,
                 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
                 * See the License for the specific language governing permissions and
                 * limitations under the License.
                 */"""

def read_file(file):
    lines = []
    fd = open(file, 'r')
    for line in fd:
        lines.append(line)
    fd.close()
    return lines

def write_file(file, lines):
    fd = open(file, 'w')
    for line in lines:
        fd.write(line)
    fd.close()

def is_not_text_file(file):
    return False

def remove_old_license(lines):
    totalLineNum = len(lines)
    lineNum = 0
    while lineNum < totalLineNum:
        line = lines[lineNum]
        if (line.find("Copyright (c)") != -1 and line.find("Elastos") != -1):
            del lines[lineNum - 1]
            del lines[lineNum - 1]
            del lines[lineNum - 1]
            totalLineNum = totalLineNum - 3
            lineNum = lineNum - 1
        else:
            lineNum = lineNum + 1

def update_license(file):
    if (is_not_text_file(file)):
        pass
    print file
    lines = read_file(file)
    remove_old_license(lines)
    write_file(file, lines)

def process(path):
    if (os.path.isfile(path)):
        update_license(path)
    else:
        entries = os.listdir(path)
        for entry in entries:
            if (entry == "." or entry == ".."):
                pass
            entryPath = path + "/" + entry
            process(entryPath)

process(sys.argv[1])
