
#include <unistd.h>
#include "CMemoryDumper.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

extern "C" void get_malloc_leak_info(uint8_t** info, size_t* overallSize,
        size_t* infoSize, size_t* totalMemory, size_t* backtraceSize);
extern "C" void free_malloc_leak_info(uint8_t* info);

#define MAX_BACKTRACE_DEPTH 50

#define NULL_RETURN(x, y) \
    if (x == NULL) { \
        return y; \
    }

static const String TAG("CMemoryDumper");
static const Int32 LINE_SIZE_LIMIT = 1024;

namespace Elastos {
namespace MemoryDumper {

ECode CMemoryDumper::DumpMemory(
    /* [in] */ Int32 fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    StringBuilder result;

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
        uint8_t *ptr = info;
        size_t count = overallSize / infoSize;

        snprintf(buffer, SIZE, " Allocation count %i\n", count);
        result.Append(buffer);
        snprintf(buffer, SIZE, " Total memory %i\n", totalMemory);
        result.Append(buffer);

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
        do {
            moved = false;
            for (size_t i = 0; i < (count - 1); i++) {
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
            result.Append(buffer);
            for (size_t ct = 0; (ct < backtraceSize) && e->backtrace[ct]; ct++) {
                if (ct) {
                    result.Append(", ");
                }
                snprintf(buffer, SIZE, "0x%08x", e->backtrace[ct]);
                result.Append(buffer);
            }
            result.Append("\n");
        }

        delete[] entries;
        free_malloc_leak_info(info);
    }
    String content;
    result.ToString(&content);
    write(fd, (const char*)content, content.GetByteLength());
    return NOERROR;
}

ECode CMemoryDumper::Print(
    /* [in] */ List<String>* list,
    /* [in] */ Int32 pid)
{
    List<String>::Iterator it = list->Begin();
    for (Int32 cnt = 0; it != list->End() && cnt < MAX_BACKTRACE_DEPTH; ++it, ++cnt) {
        Slogger::V(TAG, "pc = %s", it->string());
        AutoPtr<MapInfoManager> mapInfo = new MapInfoManager(pid);
        unsigned pc = strtoul((*it).string(), NULL, 16);
        unsigned rpc;
        const MapInfoManager::MapInfo* mi = mapInfo->Pc2Mapinfo(pc, &rpc);
        Slogger::V(TAG, "\t#%02d  pc %08x  %s", cnt,
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
    Slogger::V(TAG, "---------------------------------------");
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

            Slogger::V(TAG, "-----------------Line:%d, %s, size:%d, num:%d--------------------", ++i, flag.string(), size, num);
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
    // Slogger::V(TAG, "map %s, address (%08x, %08x)", mi->name, mi->start, mi->end);
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


