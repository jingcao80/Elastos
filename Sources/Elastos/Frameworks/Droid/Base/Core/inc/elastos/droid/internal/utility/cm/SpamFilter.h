#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class SpamFilter
{
public:
    static CARAPI GetUri(
        /* [out] */ IUri** uri);

    static CARAPI GetNormalizedContent(
        /* [in] */ const String& msg,
        /* [out] */ String* result);

    static CARAPI GetNotificationContent(
        /* [in] */ INotification* notification,
        /* [out] */ String* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__
