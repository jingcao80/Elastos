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
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <cutils/sockets.h>
#include <sys/socket.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

#define MAX_BACKTRACE_DEPTH 50

#define NULL_RETURN(x, y) \
    if (x == NULL) { \
        return y; \
    }

#define MESSAGE "dump"

static const String TAG("CMemoryDumper");
static const Int32 LINE_SIZE_LIMIT = 1024;

namespace Elastos {
namespace MemoryDumper {

ECode CMemoryDumper::SendDumpRequest()
{
    int hdsocket = socket_local_client("elastos:heapdump", ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (hdsocket < 0) {
        hdsocket = -1;
        Logger::W(TAG, "fail to connect to heapdump.");
        return NOERROR;
    }
    else {
        fcntl(hdsocket, F_SETFD, FD_CLOEXEC);
    }

    if (write(hdsocket, MESSAGE, strlen(MESSAGE) + 1) < 0) {
        Logger::W(TAG, "fail to send message.");
    }
    return NOERROR;
}

ECode CMemoryDumper::Print(
    /* [in] */ List<String>* list,
    /* [in] */ Int32 pid)
{
    List<String>::Iterator it = list->Begin();
    for (Int32 cnt = 0; it != list->End() && cnt < MAX_BACKTRACE_DEPTH; ++it, ++cnt) {
        Logger::V(TAG, "pc = %s", it->string());
        AutoPtr<MapInfoManager> mapInfo = new MapInfoManager(pid);
        unsigned pc = strtoul((*it).string(), NULL, 16);
        unsigned rpc;
        const MapInfoManager::MapInfo* mi = mapInfo->Pc2Mapinfo(pc, &rpc);
        Logger::V(TAG, "\t#%02d  pc %08x  %s", cnt,
            mi ? rpc : pc,
            mi ? mi->name : "(unknown)");
    }
    return NOERROR;
}

ECode CMemoryDumper::ProcessAllocEntry(
    /* [in] */ char* oneLine,
    /* [in] */ Int32 pid)
{
    List<String> list;
    uint8_t dropNum = 0;
    char* pFunc = strtok(oneLine, ",");
    while (pFunc) {
        if (dropNum++ >= 2) {
            //we could reverse it here.
            while(*pFunc == ' ') {
                pFunc++;
            }
            if (strncmp(pFunc, "0x", 2)) {
                break;
            }
            pFunc += 2;
            list.PushBack(String(pFunc));
        }
        pFunc = strtok(NULL, ",");
    }
    Print(&list, pid);
    Logger::V(TAG, "---------------------------------------");
    return NOERROR;
}

ECode CMemoryDumper::OutputBackTrace(
    /* [in] */ const String& diffFile,
    /* [in] */ Int32 pid)
{
    FILE *fd = fopen(diffFile.string(), "r");
    char data[LINE_SIZE_LIMIT];
    if (fd) {
        Int32 i = 0;
        Int32 num, size, start, end;
        String numStr, flag;
        while (fgets(data, LINE_SIZE_LIMIT, fd)) {
            num = 0;
            size = 0;
            flag = String("");

            String info(data);
            start = info.IndexOf("sz");

            if (start >= 0) {
                flag = info.Substring(0, start);
                flag = flag.Trim();

                end = info.IndexOf(",", start + 2);
                numStr = info.Substring(start + 2, end);
                size = StringUtils::ParseInt32(numStr.Trim());
            }

            start = info.IndexOf("num");
            if (start >= 0) {
                end = info.IndexOf(",", start + 3);
                numStr = info.Substring(start + 3, end);
                num = StringUtils::ParseInt32(numStr.Trim());
            }

            Logger::V(TAG, "-----------------Line:%d, %s, size:%d, num:%d--------------------", ++i, flag.string(), size, num);
            if (FAILED(ProcessAllocEntry(data, pid))) {
                break;
            }
        }
        fclose(fd);
    }

    return NOERROR;
}

CMemoryDumper::MapInfoManager::MapInfoManager(
    /* [in] */ Int32 pid)
    : miList(NULL)
{
    InitMapInfo(pid);
}

CMemoryDumper::MapInfoManager::~MapInfoManager()
{
    DeinitMapInfo();
}

CMemoryDumper::MapInfoManager::MapInfo* CMemoryDumper::MapInfoManager::ParseMapsLine(
    /* [in] */ char* line)
{
    Int32 len = strlen(line);

    if (len < 1) return 0;
    line[--len] = 0;

    if (len < 50) return 0;
    if (line[20] != 'x') return 0;

    MapInfo* mi = static_cast<MapInfo*>(malloc(sizeof(MapInfo) + (len - 47)));
    if (mi == 0) return 0;

    mi->start = strtoul(line, 0, 16);
    mi->end = strtoul(line + 9, 0, 16);
    /* To be filled in parse_elf_info if the mapped section starts with
     * elf_header
     */
    mi->next = 0;
    strcpy(mi->name, line + 49);
    // Logger::V(TAG, "map %s, address (%08x, %08x)", mi->name, mi->start, mi->end);
    return mi;
}

ECode CMemoryDumper::MapInfoManager::InitMapInfo(
    /* [in] */ Int32 pid)
{
    char data[LINE_SIZE_LIMIT];
    snprintf(data, sizeof(data), "/proc/%d/maps", pid);
    FILE *fp = fopen(data, "r");
    if (fp) {
        while (fgets(data, sizeof(data), fp)) {
            MapInfo *mi = ParseMapsLine(data);
            if (mi) {
                mi->next = miList;
                miList = mi;
            }
        }
        fclose(fp);
    }
    return NOERROR;
}

void CMemoryDumper::MapInfoManager::DeinitMapInfo()
{
    MapInfo *del;
    while (miList) {
        del = miList;
        miList = miList->next;
        free(del);
    }
}

const char* CMemoryDumper::MapInfoManager::Map2Name(
    /* [in] */ unsigned pc,
    /* [in] */ const char* def)
{
    MapInfo *mi = miList;
    while (mi) {
        if ((pc >= mi->start) && (pc < mi->end)) {
            return mi->name;
        }
        mi = mi->next;
    }
    return def;
}

const CMemoryDumper::MapInfoManager::MapInfo* CMemoryDumper::MapInfoManager::Pc2Mapinfo(
    /* [in] */ unsigned pc,
    /* [out] */ unsigned *rel_pc)
{
    NULL_RETURN(rel_pc, NULL)
    *rel_pc = pc;
    MapInfo *mi = miList;
    while (mi) {
        if ((pc >= mi->start) && (pc < mi->end)) {
            // Only calculate the relative offset for shared libraries
            if (strstr(mi->name, ".so") || strstr(mi->name, ".eco")) {
                *rel_pc -= mi->start;
            }
            return mi;
        }
        mi = mi->next;
    }
    return NULL;
}

}
}


