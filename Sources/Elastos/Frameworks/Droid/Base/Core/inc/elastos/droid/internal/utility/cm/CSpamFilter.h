#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CSPAMFILTER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CSPAMFILTER_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CSpamFilter.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CSpamFilter)
    , public Singleton
    , public ISpamFilter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetNormalizedContent(
        /* [in] */ const String& msg,
        /* [out] */ String* result);

    CARAPI GetNotificationContent(
        /* [in] */ INotification* notification,
        /* [out] */ String* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CSPAMFILTER_H__
