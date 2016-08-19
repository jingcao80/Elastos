
#ifndef __ELASTOS_DROID_OS_CDEBUG_H__
#define __ELASTOS_DROID_OS_CDEBUG_H__

#include "_Elastos_Droid_Os_CDebug.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CDebug)
    , public Singleton
    , public IDebug
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI DumpHeap(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Retrieves /proc/meminfo.  outSizes is filled with fields
     * as defined by MEMINFO_* offsets.
     * @hide
     */
    CARAPI GetMemInfo(
        /* [out] */ ArrayOf<Int64>** outSizes);

    /**
     * Retrieves information about this processes memory usages. This information is broken down by
     * how much is in use by dalivk, the native heap, and everything else.
     */
    CARAPI GetMemoryInfo(
        /* [out] */ IDebugMemoryInfo** memoryInfo);

    /**
     * Note: currently only works when the requested pid has the same UID
     * as the caller.
     * @hide
     */
    CARAPI GetMemoryInfo(
        /* [in] */ Int32 pid,
        /* [out] */ IDebugMemoryInfo** memoryInfo);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps.
     */
    CARAPI GetPss(
        /* [out] */ Int64* pss);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps.  Optionally supply a long array of 1 entry to also
     * receive the uss of the process.  @hide
     */
    CARAPI GetPss(
        /* [in] */ Int32 pid,
        /* [out, callee] */ ArrayOf<Int64>** outUss,
        /* [out] */ Int64* pss);

    /**
     * Have the stack traces of the given native process dumped to the
     * specified file.  Will be appended to the file.
     * @hide
     */
    CARAPI DumpNativeBacktraceToFile(
        /* [in] */ Int32 pid,
        /* [in] */ const String& file);

    /**
     * Wait until a debugger attaches.  As soon as the debugger attaches,
     * this returns, so you will need to place a breakpoint after the
     * waitForDebugger() call if you want to start tracing immediately.
     */
    CARAPI WaitForDebugger();

    /**
     * Returns "true" if one or more threads is waiting for a debugger
     * to attach.
     */
    CARAPI WaitForDebugger(
        /* [out] */ Boolean* result);

    /**
     * Determine if a debugger is currently attached.
     */
    CARAPI IsDebuggerConnected(
        /* [out] */ Boolean* result);

    CARAPI ChangeDebugPort(
        /* [in] */ Int32 port);

    CARAPI StartNativeTracing();

    CARAPI StopNativeTracing();

    CARAPI EnableEmulatorTraceOutput();

    CARAPI StartMethodTracing();

    CARAPI StartMethodTracing(
        /* [in] */ const String& traceName);

    CARAPI StartMethodTracing(
        /* [in] */ const String& traceName,
        /* [in] */ Int32 bufferSize);

    CARAPI StartMethodTracing(
        /* [in] */ const String& traceName,
        /* [in] */ Int32 bufferSize,
        /* [in] */ Int32 flags);

    CARAPI StartMethodTracingSampling(
        /* [in] */ const String& traceName,
        /* [in] */ Int32 bufferSize,
        /* [in] */ Int32 intervalUs);

    CARAPI StartMethodTracing(
        /* [in] */ const String& traceName,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 bufferSize,
        /* [in] */ Int32 flags);

    CARAPI StopMethodTracing();

    CARAPI ThreadCpuTimeNanos(
        /* [out] */ Int64* result);

    CARAPI StartAllocCounting();

    CARAPI StopAllocCounting();

    CARAPI GetGlobalAllocCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalAllocCount();

    CARAPI GetGlobalAllocSize(
        /* [out] */ Int32* size);

    CARAPI ReSetGlobalAllocSize();

    CARAPI GetGlobalFreedCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalFreedCount();

    CARAPI GetGlobalFreedSize(
        /* [out] */ Int32* size);

    CARAPI ReSetGlobalFreedSize();

    CARAPI GetGlobalGcInvocationCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalGcInvocationCount();

    CARAPI GetGlobalClassInitCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalClassInitCount();

    CARAPI GetGlobalClassInitTime(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalClassInitTime();

    CARAPI GetGlobalExternalAllocCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalExternalAllocCount();

    CARAPI GetGlobalExternalAllocSize(
        /* [out] */ Int32* size);

    CARAPI ReSetGlobalExternalAllocSize();

    CARAPI GetGlobalExternalFreedCount(
        /* [out] */ Int32* count);

    CARAPI ReSetGlobalExternalFreedCount();

    CARAPI GetGlobalExternalFreedSize(
        /* [out] */ Int32* size);

    CARAPI ReSetGlobalExternalFreedSize();

    CARAPI GetThreadAllocCount(
        /* [out] */ Int32* count);

    CARAPI ReSetThreadAllocCount();

    CARAPI GetThreadAllocSize(
        /* [out] */ Int32* size);

    CARAPI ReSetThreadAllocSize();

    CARAPI GetThreadExternalAllocCount(
        /* [out] */ Int32* count);

    CARAPI ReSetThreadExternalAllocCount();

    CARAPI GetThreadExternalAllocSize(
        /* [out] */ Int32* size);

    CARAPI ReSetThreadExternalAllocSize();

    CARAPI GetThreadGcInvocationCount(
        /* [out] */ Int32* count);

    CARAPI ReSetThreadGcInvocationCount();

    CARAPI GetNativeHeapSize();

    CARAPI GetNativeHeapAllocatedSize();

    CARAPI GetNativeHeapFreeSize();

    CARAPI SetAllocationLimit(
        /* [in] */ Int32 limit,
        /* [out] */ Int32* result);

    CARAPI SetGlobalAllocationLimit(
        /* [in] */ Int32 limit,
        /* [out] */ Int32* result);

    CARAPI PrintLoadedClasses(
        /* [in] */ Int32 flags);

    CARAPI GetLoadedClassCount(
        /* [out] */ Int32* result);

    CARAPI DumpHprofData(
        /* [in] */ const String& fileName);

    CARAPI DumpHprofData(
        /* [in] */ const String& fileName,
        /* [in] */ IFileDescriptor* fd);

    CARAPI GetBinderSentTransactions(
        /* [out] */ Int32* result);

    CARAPI GetBinderReceivedTransactions(
        /* [out] */ Int32* result);

    CARAPI GetBinderLocalObjectCount(
        /* [out] */ Int32* result);

    CARAPI GetBinderProxyObjectCount(
        /* [out] */ Int32* result);

    CARAPI GetBinderDeathObjectCount(
        /* [out] */ Int32* result);

    CARAPI SetFieldsOn(
        /* [in] */ const InterfaceID& cl,
        /* [in] */ Boolean partial);

    CARAPI DumpService(
        /* [in] */ const String& name,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<String>* args,
        /* [out] */ Boolean * result);
};

}
}
}

#endif // __ELASTOS_DROID_OS_CDEBUG_H__
