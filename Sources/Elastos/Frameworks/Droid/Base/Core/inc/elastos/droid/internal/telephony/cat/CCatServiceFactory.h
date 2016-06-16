#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatServiceFactory.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatServiceFactory)
    , public Singleton
    , public ICatServiceFactory
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI MakeCatService(
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IContext* context,
        /* [in] */ IUiccCard* ic,
        /* [in] */ Int32 slotId,
        /* [out] */ ICatService** result);

    CARAPI GetCatService(
        /* [in] */ Int32 slotId,
        /* [out] */ ICatService** result);

    CARAPI DisposeCatService(
        /* [in] */ Int32 slotId);

};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__
