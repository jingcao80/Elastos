#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONRESPONSE_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONRESPONSE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PackageVerificationResponse : public Object
{
public:
    PackageVerificationResponse(
        /* [in] */ Int32 code,
        /* [in] */ Int32 callerUid)
        : mCode(code)
        , mCallerUid(callerUid)
    {}

public:
    Int32 mCode;

    Int32 mCallerUid;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONRESPONSE_H__
