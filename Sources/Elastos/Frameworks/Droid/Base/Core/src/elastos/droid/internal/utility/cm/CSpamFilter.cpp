#include "elastos/droid/internal/utility/cm/SpamFilter.h"
#include "elastos/droid/internal/utility/cm/CSpamFilter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CSpamFilter, Singleton, ISpamFilter)

CAR_SINGLETON_IMPL(CSpamFilter)

ECode CSpamFilter::GetUri(
    /* [out] */ IUri** result)
{
    return SpamFilter::GetUri(result);
}

ECode CSpamFilter::GetNormalizedContent(
    /* [in] */ const String& msg,
    /* [out] */ String* result)
{
    return SpamFilter::GetNormalizedContent(msg, result);
}

ECode CSpamFilter::GetNotificationContent(
    /* [in] */ INotification* notification,
    /* [out] */ String* result)
{
    return SpamFilter::GetNotificationContent(notification, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
