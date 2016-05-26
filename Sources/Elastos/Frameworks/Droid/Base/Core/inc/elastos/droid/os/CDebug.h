
#ifndef __ELASTOS_DROID_OS_CDEBUG_H__
#define __ELASTOS_DROID_OS_CDEBUG_H__

#include "_Elastos_Droid_Os_CDebug.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CDebug)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI DumpHeap(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Retrieves information about this processes memory usages. This information is broken down by
     * how much is in use by dalivk, the native heap, and everything else.
     */
    CARAPI GetMemoryInfo(
        /* [in] */ IDebugMemoryInfo* memoryInfo);

    /**
     * Note: currently only works when the requested pid has the same UID
     * as the caller.
     * @hide
     */
    CARAPI GetMemoryInfo(
        /* [in] */ Int32 pid,
        /* [in] */ IDebugMemoryInfo* memoryInfo);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps.
     */
    CARAPI GetPss(
        /* [out] */ Int64* pss);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps. @hide
     */
    CARAPI GetPss(
        /* [in] */ Int32 pid,
        /* [out] */ Int64* pss);

    /**
     * Have the stack traces of the given native process dumped to the
     * specified file.  Will be appended to the file.
     * @hide
     */
    CARAPI DumpNativeBacktraceToFile(
        /* [in] */ Int32 pid,
        /* [in] */ const String& file);
private:
    CARAPI DoDumpHeap(
        /* [in] */ FILE* fp);
};

}
}
}

#endif // __ELASTOS_DROID_OS_CDEBUG_H__
