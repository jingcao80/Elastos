
#ifndef __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
#define __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__

#include "_Org_Alljoyn_Bus_Alljoyn_CDaemonInit.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

CarClass(CDaemonInit)
    , public Singleton
    , public IDaemonInit
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI PrepareDaemon(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
