
#include "elastos/droid/os/CDebug.h"
#include "elastos/droid/os/CDebugMemoryInfo.h"
#include <elastos/utility/logging/Slogger.h>
#include <cutils/debugger.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CDebugMemoryInfo;

/* pulled out of bionic */
extern "C" void get_malloc_leak_info(uint8_t** info, size_t* overallSize,
    size_t* infoSize, size_t* totalMemory, size_t* backtraceSize);
extern "C" void free_malloc_leak_info(uint8_t* info);

extern "C" void qsort(void *, size_t, size_t, int (*)(const void *, const void *));

namespace Elastos {
namespace Droid {
namespace Os {

#define SIZE_FLAG_ZYGOTE_CHILD  (1<<31)
#define BACKTRACE_SIZE          32

struct stats_t {
    int pss;
    int privateDirty;
    int sharedDirty;
};

enum {
    HEAP_UNKNOWN,
    HEAP_DALVIK,
    HEAP_NATIVE,
    HEAP_CURSOR,
    HEAP_ASHMEM,
    HEAP_UNKNOWN_DEV,
    HEAP_SO,
    HEAP_JAR,
    HEAP_APK,
    HEAP_TTF,
    HEAP_DEX,
    HEAP_UNKNOWN_MAP,

    _NUM_HEAP,
    _NUM_CORE_HEAP = HEAP_NATIVE+1
};

static const String TAG("Debug");

/*
 * This is a qsort() callback.
 *
 * See dumpNativeHeap() for comments about the data format and sort order.
 */
static int CompareHeapRecords(const void* vrec1, const void* vrec2)
{
    const size_t* rec1 = (const size_t*) vrec1;
    const size_t* rec2 = (const size_t*) vrec2;
    size_t size1 = *rec1;
    size_t size2 = *rec2;

    if (size1 < size2) {
        return 1;
    } else if (size1 > size2) {
        return -1;
    }

    intptr_t* bt1 = (intptr_t*)(rec1 + 2);
    intptr_t* bt2 = (intptr_t*)(rec2 + 2);
    for (size_t idx = 0; idx < BACKTRACE_SIZE; idx++) {
        intptr_t addr1 = bt1[idx];
        intptr_t addr2 = bt2[idx];
        if (addr1 == addr2) {
            if (addr1 == 0)
                break;
            continue;
        }
        if (addr1 < addr2) {
            return -1;
        } else if (addr1 > addr2) {
            return 1;
        }
    }

    return 0;
}

CAR_INTERFACE_IMPL(CDebug, Object, IDebug)

CAR_OBJECT_IMPL(CDebug)

ECode CDebug::DumpHeap(
    /* [in] */ IFileDescriptor* fd)
{
    /* dup() the descriptor so we don't close the original with fclose() */
    Int32 origFd;
    fd->GetDescriptor(&origFd);
    Int32 FD = dup(origFd);
    if (FD < 0) {
        Slogger::W(TAG, "dup(%d) failed: %s\n", origFd, strerror(errno));
        return E_RUNTIME_EXCEPTION;
    }

    FILE* fp = fdopen(FD, "w");
    if (fp == NULL) {
        Slogger::W(TAG, "fdopen(%d) failed: %s\n", FD, strerror(errno));
        close(FD);
        return E_RUNTIME_EXCEPTION;
    }
    Slogger::D(TAG, "Native heap dump starting...");
    DoDumpHeap(fp);
    fclose(fp);
    Slogger::D(TAG, "Native heap dump complete.");
    return NOERROR;
}

ECode CDebug::DoDumpHeap(
    /* [in] */ FILE* fp)
{
    uint8_t* info = NULL;
    size_t overallSize, infoSize, totalMemory, backtraceSize;

    get_malloc_leak_info(&info, &overallSize, &infoSize, &totalMemory,
        &backtraceSize);
    if (info == NULL) {
        fprintf(fp, "Native heap dump not available. To enable, run these"
                    " commands (requires root):\n");
        fprintf(fp, "$ adb shell setprop libc.debug.malloc 1\n");
        fprintf(fp, "$ adb shell stop\n");
        fprintf(fp, "$ adb shell start\n");
        return NOERROR;
    }
    ASSERT_TRUE(infoSize != 0);
    ASSERT_TRUE(overallSize % infoSize == 0);

    fprintf(fp, "Android Native Heap Dump v1.0\n\n");

    size_t recordCount = overallSize / infoSize;
    fprintf(fp, "Total memory: %zu\n", totalMemory);
    fprintf(fp, "Allocation records: %zd\n", recordCount);
    if (backtraceSize != BACKTRACE_SIZE) {
        fprintf(fp, "WARNING: mismatched backtrace sizes (%d vs. %d)\n",
            backtraceSize, BACKTRACE_SIZE);
    }
    fprintf(fp, "\n");

    /* re-sort the entries */
    qsort(info, recordCount, infoSize, CompareHeapRecords);

    /* dump the entries to the file */
    const uint8_t* ptr = info;
    for (size_t idx = 0; idx < recordCount; idx++) {
        size_t size = *(size_t*) ptr;
        size_t allocations = *(size_t*) (ptr + sizeof(size_t));
        intptr_t* backtrace = (intptr_t*) (ptr + sizeof(size_t) * 2);

        fprintf(fp, "z %d  sz %8zu,  num %4zu",
                (size & SIZE_FLAG_ZYGOTE_CHILD) != 0,
                size & ~SIZE_FLAG_ZYGOTE_CHILD,
                allocations);
        for (size_t bt = 0; bt < backtraceSize; bt++) {
            if (backtrace[bt] == 0) {
                break;
            } else {
                fprintf(fp, ", 0x%08x", backtrace[bt]);
            }
        }
        fprintf(fp, "\n");
        ptr += infoSize;
    }
    free_malloc_leak_info(info);
    fprintf(fp, "MAPS\n");
    const char* maps = "/proc/self/maps";
    FILE* in = fopen(maps, "r");
    if (in == NULL) {
        fprintf(fp, "Could not open %s\n", maps);
        return NOERROR;
    }
    char buf[BUFSIZ];
    while (size_t n = fread(buf, sizeof(char), BUFSIZ, in)) {
        fwrite(buf, sizeof(char), n, fp);
    }
    fclose(in);
    fprintf(fp, "END\n");
    return NOERROR;
}

void Read_Mapinfo(
    /* [in] */ FILE* fp,
    /* [in] */ stats_t* stats)
{
    char line[1024];
    int len, nameLen;
    bool skip, done = false;

    unsigned size = 0, resident = 0, pss = 0;
    unsigned shared_clean = 0, shared_dirty = 0;
    unsigned private_clean = 0, private_dirty = 0;
    unsigned referenced = 0;
    unsigned temp;

    unsigned long int start;
    unsigned long int end = 0;
    unsigned long int prevEnd = 0;
    char* name;
    int name_pos;

    int whichHeap = HEAP_UNKNOWN;
    int prevHeap = HEAP_UNKNOWN;

    if(fgets(line, sizeof(line), fp) == 0) return;

    while (!done) {
        prevHeap = whichHeap;
        prevEnd = end;
        whichHeap = HEAP_UNKNOWN;
        skip = FALSE;

        len = strlen(line);
        if (len < 1) return;
        line[--len] = 0;

        if (sscanf(line, "%lx-%lx %*s %*x %*x:%*x %*d%n", &start, &end, &name_pos) != 2) {
            skip = TRUE;
        } else {
            while (isspace(line[name_pos])) {
                name_pos += 1;
            }
            name = line + name_pos;
            nameLen = strlen(name);

            if (strstr(name, "[heap]") == name) {
                whichHeap = HEAP_NATIVE;
            } else if (strstr(name, "/dev/ashmem/dalvik-") == name) {
                whichHeap = HEAP_DALVIK;
            } else if (strstr(name, "/dev/ashmem/CursorWindow") == name) {
                whichHeap = HEAP_CURSOR;
            } else if (strstr(name, "/dev/ashmem/") == name) {
                whichHeap = HEAP_ASHMEM;
            } else if (strstr(name, "/dev/") == name) {
                whichHeap = HEAP_UNKNOWN_DEV;
            } else if (nameLen > 3 && strcmp(name+nameLen-3, ".so") == 0) {
                whichHeap = HEAP_SO;
            } else if (nameLen > 4 && strcmp(name+nameLen-4, ".jar") == 0) {
                whichHeap = HEAP_JAR;
            } else if (nameLen > 4 && strcmp(name+nameLen-4, ".apk") == 0) {
                whichHeap = HEAP_APK;
            } else if (nameLen > 4 && strcmp(name+nameLen-4, ".ttf") == 0) {
                whichHeap = HEAP_TTF;
            } else if (nameLen > 4 && strcmp(name+nameLen-4, ".dex") == 0) {
                whichHeap = HEAP_DEX;
            } else if (nameLen > 0) {
                whichHeap = HEAP_UNKNOWN_MAP;
            } else if (start == prevEnd && prevHeap == HEAP_SO) {
                // bss section of a shared library.
                whichHeap = HEAP_SO;
            }
        }

        //ALOGI("native=%d dalvik=%d sqlite=%d: %s\n", isNativeHeap, isDalvikHeap,
        //    isSqliteHeap, line);

        while (TRUE) {
            if (fgets(line, 1024, fp) == 0) {
                done = TRUE;
                break;
            }

            if (sscanf(line, "Size: %d kB", &temp) == 1) {
                size = temp;
            } else if (sscanf(line, "Rss: %d kB", &temp) == 1) {
                resident = temp;
            } else if (sscanf(line, "Pss: %d kB", &temp) == 1) {
                pss = temp;
            } else if (sscanf(line, "Shared_Clean: %d kB", &temp) == 1) {
                shared_clean = temp;
            } else if (sscanf(line, "Shared_Dirty: %d kB", &temp) == 1) {
                shared_dirty = temp;
            } else if (sscanf(line, "Private_Clean: %d kB", &temp) == 1) {
                private_clean = temp;
            } else if (sscanf(line, "Private_Dirty: %d kB", &temp) == 1) {
                private_dirty = temp;
            } else if (sscanf(line, "Referenced: %d kB", &temp) == 1) {
                referenced = temp;
            } else if (strlen(line) > 30 && line[8] == '-' && line[17] == ' ') {
                // looks like a new mapping
                // example: "10000000-10001000 ---p 10000000 00:00 0"
                break;
            }
        }

        if (!skip) {
            stats[whichHeap].pss += pss;
            stats[whichHeap].privateDirty += private_dirty;
            stats[whichHeap].sharedDirty += shared_dirty;
        }
    }
    return;
}

void Load_Maps(
    /* [in] */ Int32 pid,
    /* [in] */ stats_t* stats)
{
    char tmp[128];
    FILE *fp;

    sprintf(tmp, "/proc/%d/smaps", pid);
    fp = fopen(tmp, "r");
    if (fp == 0) return;

    Read_Mapinfo(fp, stats);
    fclose(fp);
    return;
}

ECode CDebug::GetMemoryInfo(
    /* [in] */ IDebugMemoryInfo* memoryInfo)
{
    return GetMemoryInfo(getpid(), memoryInfo);
}

ECode CDebug::GetMemoryInfo(
    /* [in] */ Int32 pid,
    /* [in] */ IDebugMemoryInfo* memoryInfo)
{
    stats_t stats[_NUM_HEAP];
    memset(&stats, 0, sizeof(stats));

    Load_Maps(pid, stats);

    for (int i = _NUM_CORE_HEAP; i < _NUM_HEAP; i++) {
        stats[HEAP_UNKNOWN].pss += stats[i].pss;
        stats[HEAP_UNKNOWN].privateDirty += stats[i].privateDirty;
        stats[HEAP_UNKNOWN].sharedDirty += stats[i].sharedDirty;
    }

    memoryInfo->SetOtherPss(stats[0].pss);
    memoryInfo->SetOtherPrivateDirty(stats[0].privateDirty);
    memoryInfo->SetOtherSharedDirty(stats[0].sharedDirty);

    memoryInfo->SetDalvikPss(stats[1].pss);
    memoryInfo->SetDalvikPrivateDirty(stats[1].privateDirty);
    memoryInfo->SetDalvikSharedDirty(stats[1].sharedDirty);

    memoryInfo->SetNativePss(stats[2].pss);
    memoryInfo->SetNativePrivateDirty(stats[2].privateDirty);
    memoryInfo->SetNativeSharedDirty(stats[2].sharedDirty);

    AutoPtr<ArrayOf<Int32> > otherStats;
    CDebugMemoryInfo* cinfo = (CDebugMemoryInfo*)memoryInfo;
    cinfo->GetOtherStats((ArrayOf<Int32>**)&otherStats);
    if(otherStats == NULL) return NOERROR;

    int j = 0;
    for (int i = _NUM_CORE_HEAP; i < _NUM_HEAP; i++) {
        cinfo->SetOtherStats(j++, stats[i].pss);
        cinfo->SetOtherStats(j++, stats[i].privateDirty);
        cinfo->SetOtherStats(j++, stats[i].sharedDirty);
    }

    return NOERROR;
}

ECode CDebug::GetPss(
    /* [out] */ Int64* pss)
{
    return GetPss(getpid(), pss);
}

ECode CDebug::GetPss(
    /* [in] */ Int32 pid,
    /* [out] */ Int64* _pss)
{
    VALIDATE_NOT_NULL(_pss);
    *_pss = 0;

    char line[1024];
    long pss = 0;
    unsigned temp;

    char tmp[128];
    FILE *fp;

    sprintf(tmp, "/proc/%d/smaps", pid);
    fp = fopen(tmp, "r");
    if (fp == 0) {
        *_pss = 0;
        return NOERROR;
    }

    while (TRUE) {
        if (fgets(line, 1024, fp) == 0) {
            break;
        }

        if (sscanf(line, "Pss: %d kB", &temp) == 1) {
            pss += temp;
        }
    }

    fclose(fp);

    *_pss = pss;
    return NOERROR;
}

ECode CDebug::DumpNativeBacktraceToFile(
    /* [in] */ Int32 pid,
    /* [in] */ const String& file)
{
    if (file.IsNull()) {
        //jniThrowNullPointerException(env, NULL);
        return E_NULL_POINTER_EXCEPTION;
    }

    int fd = open(file.string(), O_CREAT | O_WRONLY | O_NOFOLLOW, 0666);  /* -rw-rw-rw- */
    if (fd < 0) {
        fprintf(stderr, "Can't open %s: %s\n", file.string(), strerror(errno));
        return NOERROR;
    }

    if (lseek(fd, 0, SEEK_END) < 0) {
        fprintf(stderr, "lseek: %s\n", strerror(errno));
    } else {
        dump_backtrace_to_file(pid, fd);
    }

    close(fd);
    return NOERROR;
}

}
}
}

