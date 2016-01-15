
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/utility/LocalLog.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(LocalLog, Object, ILocalLog)

LocalLog::LocalLog()
    : mMaxLines(20)
{
}

ECode LocalLog::constructor(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode LocalLog::W(
    /* [in] */ const String& msg)
{
    AutoLock lock(mLinesLock);
    Slogger::W(mTag, msg);
    if (mLines.GetSize() >= mMaxLines) {
        mLines.PopFront();
    }
    mLines.PushBack(msg);
    return NOERROR;
}

ECode LocalLog::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& header,
    /* [in] */ const String& prefix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mLinesLock);
    if (mLines.GetSize() <= 0) {
        *result = FALSE;
        return NOERROR;
    }
    if (header != NULL) {
        pw->Println(header);
    }
    List<String>::Iterator iter = mLines.Begin();
    for (; iter != mLines.End(); ++iter) {
        if (prefix != NULL) {
            pw->Print(prefix);
        }
        pw->Println(*iter);
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
