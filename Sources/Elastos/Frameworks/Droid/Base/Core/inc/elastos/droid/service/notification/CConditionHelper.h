#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONHELPER_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONHELPER_H__

#include "_Elastos_Droid_Service_Notification_CConditionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Service::Notification::IConditionHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CConditionHelper)
    , public Singleton
    , public IConditionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI StateToString(
        /* [in] */ Int32 state,
        /* [out] */ String* str);

    CARAPI RelevanceToString(
        /* [in] */ Int32 flags,
        /* [out] */ String* str);

    CARAPI NewId(
        /* [in] */ IContext* context,
        /* [out] */ IUriBuilder** id);

    CARAPI IsValidId(
        /* [in] */ IUri* id,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* isValidId);
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONHELPER_H__