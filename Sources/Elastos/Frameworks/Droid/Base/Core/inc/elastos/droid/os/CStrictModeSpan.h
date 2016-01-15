#ifndef __ELASTOS_DROID_OS_CSTRICTMODESPAN_H__
#define __ELASTOS_DROID_OS_CSTRICTMODESPAN_H__

#include "_Elastos_Droid_Os_CStrictModeSpan.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include "elastos/droid/os/CStrictMode.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeSpan)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Handle32 threadState);

    CARAPI Finish();

private:
    friend class CStrictMode;
    friend class CStrictModeViolationInfo;

    String mName;
    Int64 mCreateMillis;
    AutoPtr<CStrictModeSpan> mNext;
    AutoPtr<CStrictModeSpan> mPrev;  // not used when in freeList, only active
    AutoPtr<CStrictMode::ThreadSpanState> mContainerState;

    Object mContainerStateLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODESPAN_H__
