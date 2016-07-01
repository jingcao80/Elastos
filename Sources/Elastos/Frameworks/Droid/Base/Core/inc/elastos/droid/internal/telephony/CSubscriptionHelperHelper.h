
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscriptionHelperHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscriptionHelperHelper)
    , public Singleton
    , public ISubscriptionHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Init(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ ISubscriptionHelper** result);

    CARAPI GetInstance(
        /* [out] */ ISubscriptionHelper** result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPERHELPER_H__
