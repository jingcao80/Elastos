
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscriptionControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscriptionControllerHelper)
    , public Singleton
    , public ISubscriptionControllerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Init(
        /* [in] */ IPhone* phone,
        /* [out] */ ISubscriptionController** result);

    CARAPI Init(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ ISubscriptionController** result);

    CARAPI GetInstance(
        /* [out] */ ISubscriptionController** result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLERHELPER_H__
