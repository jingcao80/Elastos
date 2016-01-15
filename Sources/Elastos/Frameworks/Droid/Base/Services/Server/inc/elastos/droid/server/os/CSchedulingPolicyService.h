
#ifndef __ELASTOS_DROID_SERVER_OS_CSCHEDULING_POLICY_SERVICE_H__
#define __ELASTOS_DROID_SERVER_OS_CSCHEDULING_POLICY_SERVICE_H__

#include "_Elastos_Droid_Server_Os_CSchedulingPolicyService.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IISchedulingPolicyService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Os {

CarClass(CSchedulingPolicyService)
    , public Object
    , public IISchedulingPolicyService
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSchedulingPolicyService();

    CARAPI constructor();

    CARAPI RequestPriority(
        /* [in] */ Int32 pid,
        /* [in] */  Int32 tid,
        /* [in] */  Int32 prio,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    // Minimum and maximum values allowed for requestPriority parameter prio
    static const Int32 PRIORITY_MIN;
    static const Int32 PRIORITY_MAX;
};

} // namespace Os
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_OS_CSCHEDULING_POLICY_SERVICE_H__
