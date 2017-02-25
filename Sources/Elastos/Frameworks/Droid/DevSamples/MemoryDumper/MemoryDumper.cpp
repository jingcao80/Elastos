//=========================================================================
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

#include "CMemoryDumper.h"
#include <stdio.h>

using Elastos::MemoryDumper::CMemoryDumper;

void usage()
{
    printf("usage: MemoryDumper [-t | -p pid]\n\n");
    printf("    -t            cause the process which contains heapdump.cpp to dump its own heap\n");
    printf("    -p pid        dump the backtrace of the process which PID is pid");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        exit(0);
    }

    if (!strcmp(argv[1], "-t")) {
        CMemoryDumper::SendDumpRequest();
    }
    else if (!strcmp(argv[1], "-p")) {
        int pid = atoi(argv[1]);
        CMemoryDumper::OutputBackTrace(String("/data/debug/diff.txt"), pid);
    }
    else {
        usage();
    }

    return 0;
}
