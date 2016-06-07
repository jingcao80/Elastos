
#include "elastos/droid/server/spam_filter/SpamBlocker.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode SpamBlocker::constructor(
    /* [in] */ IContext* context)
{
    return NOERROR;
}

ECode SpamBlocker::ShouldBlock(
    /* [in] */ const String& number,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(this);
        VALIDATE_NOT_NULL(result)
        *result = FALSE;
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
