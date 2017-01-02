#!/usr/bin/python

import os
import sys

elastosLicenseStyle1 = [
"""//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

"""]

elastosLicenseStyle2 = [
"""#=========================================================================
# Copyright (C) 2012 The Elastos Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

"""]

elastosLicenseStyle3 = [
""";;=========================================================================
;; Copyright (C) 2012 The Elastos Open Source Project
;;
;; Licensed under the Apache License, Version 2.0 (the "License");
;; you may not use this file except in compliance with the License.
;; You may obtain a copy of the License at
;;
;;      http://www.apache.org/licenses/LICENSE-2.0
;;
;; Unless required by applicable law or agreed to in writing, software
;; distributed under the License is distributed on an "AS IS" BASIS,
;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; See the License for the specific language governing permissions and
;; limitations under the License.
;;=========================================================================

"""]

elastosLicenseStyle4 = [
"""<?xml version="1.0" encoding="utf-8"?>
<!--
/*
 * Copyright (C) 2012 The Elastos Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
-->
"""]

elastosLicenseStyle5 = [
"""$;;=========================================================================
$;; Copyright (C) 2012 The Elastos Open Source Project
$;;
$;; Licensed under the Apache License, Version 2.0 (the "License");
$;; you may not use this file except in compliance with the License.
$;; You may obtain a copy of the License at
$;;
$;;      http://www.apache.org/licenses/LICENSE-2.0
$;;
$;; Unless required by applicable law or agreed to in writing, software
$;; distributed under the License is distributed on an "AS IS" BASIS,
$;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
$;; See the License for the specific language governing permissions and
$;; limitations under the License.
$;;=========================================================================
"""]

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

def contain_new_license(lines):
    for line in lines:
        if (line.find("Copyright (C) 2012 The Elastos Open Source Project") != -1):
            return True
    return False

def remove_old_license(lines):
    totalLineNum = len(lines)
    lineNum = 0
    while lineNum < totalLineNum:
        line = lines[lineNum]
        if (line.find("Copyright (c)") != -1 and line.find("Elastos") != -1):
            del lines[lineNum + 1]
            del lines[lineNum]
            del lines[lineNum - 1]
            totalLineNum = totalLineNum - 3
            lineNum = lineNum - 1
        else:
            lineNum = lineNum + 1

def remove_blank_lines(lines):
    if (len(lines) == 0):
        return
    while (lines[0] == "\n"):
        del lines[0]

def add_new_license(file, lines):
    index = file.rfind("/")
    if index == -1:
        fileName = file
    else:
        fileName = file[(index + 1):len(file)]
    if (fileName.rfind(".h") != -1 or fileName.rfind(".cpp") != -1 or fileName.rfind(".c") != -1 \
        or fileName.rfind(".S") != -1 or fileName.rfind(".rc") != -1 or fileName.rfind(".car") != -1):
        return elastosLicenseStyle1 + lines
    elif (fileName == "dirs" or fileName == "sources" or fileName == "ReadMe" or fileName == "readme" \
        or fileName.rfind(".inc") != -1 or fileName.rfind(".txt") != -1):
        return elastosLicenseStyle2 + lines
    elif (fileName.rfind(".asm") != -1 or fileName.rfind(".def") != -1):
        return elastosLicenseStyle3 + lines
    elif (fileName == "AndroidManifest.xml"):
        if (lines[0].find("<?xml") != -1):
            del lines[0]
        return elastosLicenseStyle4 + lines
    elif (fileName.rfind(".lub") != -1):
        return elastosLicenseStyle5 + lines
    else:
        return lines

def update_license(file):
    if (is_not_text_file(file)):
        return
    lines = read_file(file)
    if (contain_new_license(lines)):
        return
    print file
    remove_old_license(lines)
    remove_blank_lines(lines)
    lines = add_new_license(file, lines)
    write_file(file, lines)

def process(path):
    if (os.path.isfile(path)):
        update_license(path)
    else:
        entries = os.listdir(path)
        for entry in entries:
            if (entry == "." or entry == ".." or entry == "res" or entry == "jpk" or entry == "bak"):
                continue
            entryPath = path + "/" + entry
            process(entryPath)

process(sys.argv[1])
