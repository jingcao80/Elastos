
#include "elastos/droid/os/Debug.h"
#include "elastos/droid/os/CDebugMemoryInfo.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Slogger.h>
#include <cutils/debugger.h>
#include <utils/CallStack.h>

#include "utils/misc.h"
#include "cutils/debugger.h"
#include <memtrack/memtrack.h>
#include <cutils/log.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

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

ECode Debug::DumpHeap(
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

ECode Debug::DoDumpHeap(
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

ECode Debug::GetMemoryInfo(
    /* [out] */ IDebugMemoryInfo** memoryInfo)
{
    return GetMemoryInfo(getpid(), memoryInfo);
}

ECode Debug::GetMemoryInfo(
    /* [in] */ Int32 pid,
    /* [out] */ IDebugMemoryInfo** memoryInfo)
{
    VALIDATE_NOT_NULL(memoryInfo)
    *memoryInfo = NULL;

    stats_t stats[_NUM_HEAP];
    memset(&stats, 0, sizeof(stats));

    Load_Maps(pid, stats);

    for (int i = _NUM_CORE_HEAP; i < _NUM_HEAP; i++) {
        stats[HEAP_UNKNOWN].pss += stats[i].pss;
        stats[HEAP_UNKNOWN].privateDirty += stats[i].privateDirty;
        stats[HEAP_UNKNOWN].sharedDirty += stats[i].sharedDirty;
    }

    AutoPtr<IDebugMemoryInfo> info;
    CDebugMemoryInfo::New((IDebugMemoryInfo**)&info);
    info->SetOtherPss(stats[0].pss);
    info->SetOtherPrivateDirty(stats[0].privateDirty);
    info->SetOtherSharedDirty(stats[0].sharedDirty);

    info->SetDalvikPss(stats[1].pss);
    info->SetDalvikPrivateDirty(stats[1].privateDirty);
    info->SetDalvikSharedDirty(stats[1].sharedDirty);

    info->SetNativePss(stats[2].pss);
    info->SetNativePrivateDirty(stats[2].privateDirty);
    info->SetNativeSharedDirty(stats[2].sharedDirty);

    AutoPtr<ArrayOf<Int32> > otherStats;
    CDebugMemoryInfo* cinfo = (CDebugMemoryInfo*)info.Get();
    cinfo->GetOtherStats((ArrayOf<Int32>**)&otherStats);
    if(otherStats == NULL) return NOERROR;

    int j = 0;
    for (int i = _NUM_CORE_HEAP; i < _NUM_HEAP; i++) {
        cinfo->SetOtherStats(j++, stats[i].pss);
        cinfo->SetOtherStats(j++, stats[i].privateDirty);
        cinfo->SetOtherStats(j++, stats[i].sharedDirty);
    }

    *memoryInfo = info;
    REFCOUNT_ADD(*memoryInfo)
    return NOERROR;
}

enum {
    MEMINFO_TOTAL,
    MEMINFO_FREE,
    MEMINFO_BUFFERS,
    MEMINFO_CACHED,
    MEMINFO_SHMEM,
    MEMINFO_SLAB,
    MEMINFO_SWAP_TOTAL,
    MEMINFO_SWAP_FREE,
    MEMINFO_ZRAM_TOTAL,
    MEMINFO_COUNT
};

ECode Debug::GetMemInfo(
    /* [out] */ ArrayOf<Int64>* outSizes)
{
    VALIDATE_NOT_NULL(outSizes)

    char buffer[1024];
    int numFound = 0;

    int fd = open("/proc/meminfo", O_RDONLY);

    if (fd < 0) {
        Slogger::W("Debug", "Unable to open /proc/meminfo: %s\n", strerror(errno));
        return NOERROR;
    }

    int len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);

    if (len < 0) {
        Slogger::W("Debug", "Empty /proc/meminfo");
        return NOERROR;
    }
    buffer[len] = 0;

    static const char* const tags[] = {
            "MemTotal:",
            "MemFree:",
            "Buffers:",
            "Cached:",
            "Shmem:",
            "Slab:",
            "SwapTotal:",
            "SwapFree:",
            NULL
    };
    static const int tagsLen[] = {
            9,
            8,
            8,
            7,
            6,
            5,
            10,
            9,
            0
    };
    long mem[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    char* p = buffer;
    while (*p && numFound < 8) {
        int i = 0;
        while (tags[i]) {
            if (strncmp(p, tags[i], tagsLen[i]) == 0) {
                p += tagsLen[i];
                while (*p == ' ') p++;
                char* num = p;
                while (*p >= '0' && *p <= '9') p++;
                if (*p != 0) {
                    *p = 0;
                    p++;
                }
                mem[i] = atoll(num);
                numFound++;
                break;
            }
            i++;
        }
        while (*p && *p != '\n') {
            p++;
        }
        if (*p) p++;
    }

    fd = open("/sys/block/zram0/mem_used_total", O_RDONLY);
    if (fd >= 0) {
        len = read(fd, buffer, sizeof(buffer)-1);
        close(fd);
        if (len > 0) {
            buffer[len] = 0;
            mem[MEMINFO_ZRAM_TOTAL] = atoll(buffer)/1024;
        }
    }

    for (int i = 0; i < MEMINFO_COUNT; i++) {
        (*outSizes)[i] = mem[i];
    }

    return NOERROR;
}

// Container used to retrieve graphics memory pss
struct graphics_memory_pss
{
    int graphics;
    int gl;
    int other;
};

/*
 * Uses libmemtrack to retrieve graphics memory that the process is using.
 * Any graphics memory reported in /proc/pid/smaps is not included here.
 */
static int read_memtrack_memory(struct memtrack_proc* p, int pid,
        struct graphics_memory_pss* graphics_mem)
{
    int err = memtrack_proc_get(p, pid);
    if (err != 0) {
        Slogger::W("Debug", "failed to get memory consumption info: %d", err);
        return err;
    }

    ssize_t pss = memtrack_proc_graphics_pss(p);
    if (pss < 0) {
        Slogger::W("Debug", "failed to get graphics pss: %zd", pss);
        return pss;
    }
    graphics_mem->graphics = pss / 1024;

    pss = memtrack_proc_gl_pss(p);
    if (pss < 0) {
        Slogger::W("Debug", "failed to get gl pss: %zd", pss);
        return pss;
    }
    graphics_mem->gl = pss / 1024;

    pss = memtrack_proc_other_pss(p);
    if (pss < 0) {
        Slogger::W("Debug", "failed to get other pss: %zd", pss);
        return pss;
    }
    graphics_mem->other = pss / 1024;

    return 0;
}

static bool memtrackLoaded = false;

/*
 * Retrieves the graphics memory that is unaccounted for in /proc/pid/smaps.
 */
static int read_memtrack_memory(int pid, struct graphics_memory_pss* graphics_mem)
{
    if (!memtrackLoaded) {
        int err = memtrack_init();
        if (err != 0) {
            memtrackLoaded = false;
            Slogger::E("Debug", "failed to load memtrack module: %d", err);
            return -1;
        } else {
            memtrackLoaded = true;
        }
    }

    struct memtrack_proc* p = memtrack_proc_new();
    if (p == NULL) {
        Slogger::W("Debug", "failed to create memtrack_proc");
        return -1;
    }

    int err = read_memtrack_memory(p, pid, graphics_mem);
    memtrack_proc_destroy(p);
    return err;
}

ECode Debug::GetPss(
    /* [out] */ Int64* pss)
{
    return GetPss(getpid(), NULL, pss);
}

ECode Debug::GetPss(
    /* [in] */ Int32 pid,
    /* [out, callee] */ ArrayOf<Int64>** outUss,
    /* [out] */ Int64* _pss)
{
    VALIDATE_NOT_NULL(_pss);
    *_pss = 0;

    char line[1024];
    Int64 pss = 0;
    Int64 uss = 0;

    char tmp[128];
    FILE *fp;

    struct graphics_memory_pss graphics_mem;
    if (read_memtrack_memory(pid, &graphics_mem) == 0) {
        pss = uss = graphics_mem.graphics + graphics_mem.gl + graphics_mem.other;
    }

    sprintf(tmp, "/proc/%d/smaps", pid);
    fp = fopen(tmp, "r");

    if (fp != 0) {
        while (true) {
            if (fgets(line, 1024, fp) == NULL) {
                break;
            }

            if (line[0] == 'P') {
                if (strncmp(line, "Pss:", 4) == 0) {
                    char* c = line + 4;
                    while (*c != 0 && (*c < '0' || *c > '9')) {
                        c++;
                    }
                    pss += atoi(c);
                } else if (strncmp(line, "Private_Clean:", 14)
                        || strncmp(line, "Private_Dirty:", 14)) {
                    char* c = line + 14;
                    while (*c != 0 && (*c < '0' || *c > '9')) {
                        c++;
                    }
                    uss += atoi(c);
                }
            }
        }

        fclose(fp);
    }

    if (outUss != NULL) {
        AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(1);
        array->Set(0, uss);
        *outUss = array;
        REFCOUNT_ADD(*outUss)
    }

    *_pss = pss;
    return NOERROR;
}

ECode Debug::DumpNativeBacktraceToFile(
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

ECode Debug::DumpBacktrace()
{
    android::CallStack stack;
    stack.update();
    Slogger::I("Debug", ">> backtrace:\n%s", stack.toString("").string());
    return NOERROR;
}

}
}
}

