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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#if 1
#include <elastos/utility/logging/Slogger.h>
#include "CMemoryDumper.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::MemoryDumper::CMemoryDumper;
using Elastos::String;

#else
extern "C" void get_malloc_leak_info(uint8_t** info, size_t* overallSize,
        size_t* infoSize, size_t* totalMemory, size_t* backtraceSize);
extern "C" void free_malloc_leak_info(uint8_t* info);

void DumpMemory(
    /* [in] */ int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    //StringBuilder result;
    char* result = (char*)malloc(256 * 1024);

    typedef struct {
        size_t size;
        size_t dups;
        intptr_t * backtrace;
    } AllocEntry;

    uint8_t *info = NULL;
    size_t overallSize = 0;
    size_t infoSize = 0;
    size_t totalMemory = 0;
    size_t backtraceSize = 0;

    get_malloc_leak_info(&info, &overallSize, &infoSize, &totalMemory, &backtraceSize);
    if (info) {
        //Slogger::V("fuck_", "enter into");
        uint8_t *ptr = info;
        size_t count = overallSize / infoSize;

        snprintf(buffer, SIZE, " Allocation count %i\n", count);
        //result.Append(buffer);
        strcat(result, buffer);
        snprintf(buffer, SIZE, " Total memory %i\n", totalMemory);
        //result.Append(buffer);
        strcat(result, buffer);

        AllocEntry * entries = new AllocEntry[count];

        for (size_t i = 0; i < count; i++) {
            // Each entry should be size_t, size_t, intptr_t[backtraceSize]
            AllocEntry *e = &entries[i];

            e->size = *reinterpret_cast<size_t *>(ptr);
            ptr += sizeof(size_t);

            e->dups = *reinterpret_cast<size_t *>(ptr);
            ptr += sizeof(size_t);

            e->backtrace = reinterpret_cast<intptr_t *>(ptr);
            ptr += sizeof(intptr_t) * backtraceSize;
        }

        // Now we need to sort the entries.  They come sorted by size but
        // not by stack trace which causes problems using diff.
        bool moved;
        size_t remaining = count;
        //Slogger::V("fuck_", "count = %d", remaining);
        do {
            moved = false;
            for (size_t i = 0; i < (remaining-- - 1); i++) {
                AllocEntry *e1 = &entries[i];
                AllocEntry *e2 = &entries[i+1];

                bool swap = e1->size < e2->size;
                if (e1->size == e2->size) {
                    for(size_t j = 0; j < backtraceSize; j++) {
                        if (e1->backtrace[j] == e2->backtrace[j]) {
                            continue;
                        }
                        swap = e1->backtrace[j] < e2->backtrace[j];
                        break;
                    }
                }
                if (swap) {
                    AllocEntry t = entries[i];
                    entries[i] = entries[i+1];
                    entries[i+1] = t;
                    moved = true;
                }
            }
        } while (moved);

        for (size_t i = 0; i < count; i++) {
            AllocEntry *e = &entries[i];

            snprintf(buffer, SIZE, "size %8i, dups %4i, ", e->size, e->dups);
            //result.Append(buffer);
            strcat(result, buffer);
            for (size_t ct = 0; (ct < backtraceSize) && e->backtrace[ct]; ct++) {
                if (ct) {
                    //result.Append(", ");
                    strcat(result, ", ");
                }
                snprintf(buffer, SIZE, "0x%08x", e->backtrace[ct]);
                //result.Append(buffer);
                strcat(result, buffer);
            }
            //result.Append("\n");
            strcat(result, "\n");
        }

        delete[] entries;
        free_malloc_leak_info(info);
    }
    write(fd, (const char*)result, strlen(result));
}

#endif

int main(int argc, char *argv[])
{
    int pid;

    if (argc < 2) {
        pid = getpid();
    }
    else {
        pid = atoi(argv[1]);
        Slogger::D("MemoryDumper", "pid = %d", pid);
    }

#if 0
    printf("####hello world!####\n");
    AutoPtr<IMemoryDumper> dumper;
    CMemoryDumper::AcquireSingleton((IMemoryDumper**)&dumper);
    Slogger::V("fuck11", "begin to dump previous");
    int fd = open("/data/log/previous.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IWOTH | S_IWGRP);
    dumper->DumpMemory(fd);
    Slogger::V("fuck", "end to dump previous");
    void *p = (void*)malloc(1999);
    Slogger::V("fuck11", "begin to dump late");
    close(fd);
    fd = open("/data/log/late.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IWOTH | S_IWGRP);
    dumper->DumpMemory(fd);
    close(fd);
    Slogger::V("fuck", "end to dump late");
#else
    //memory dump
/*
    int fd = open("/data/log/previous.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IWOTH | S_IWGRP);
    if (fd)
    {
        DumpMemory(fd);
        close(fd);
    }
    void *p = (void*)malloc(1999);
    fd = open("/data/log/late.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IWOTH | S_IWGRP);
    DumpMemory(fd);
    close(fd);
    printf("current pid = %d\n", getpid());
    sleep(1800);
*/
#endif
    //parse diff
    CMemoryDumper::OutputBackTrace(String("/data/debug/diff.txt"), pid);
    return 0;
}

