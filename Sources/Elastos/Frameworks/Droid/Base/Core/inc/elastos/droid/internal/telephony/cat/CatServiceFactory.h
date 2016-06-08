#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICEFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.TelephonyManager;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
// import com.android.internal.telephony.uicc.IccFileHandler;
// import com.android.internal.telephony.uicc.UiccCardApplication;
// import com.android.internal.telephony.uicc.UiccController;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Cat::ICatService;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class that creates the CatServices for each card.
  *
  * {@hide}
  */
class CatServiceFactory
    : public Object
    , public ICatServiceFactory
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Used for instantiating the Service from the Card.
      *
      * @param ci CommandsInterface object
      * @param context phone app context
      * @param ic Icc card
      * @param slotId to know the index of card
      * @return The only Service object in the system
      */
    static CARAPI_(AutoPtr<ICatService>) MakeCatService(
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IContext* context,
        /* [in] */ IUiccCard* ic,
        /* [in] */ Int32 slotId);

    static CARAPI_(AutoPtr<ICatService>) GetCatService(
        /* [in] */ Int32 slotId);

    CARAPI DisposeCatService(
        /* [in] */ Int32 slotId);

private:
    static AutoPtr<ArrayOf<ICatService*> > sCatServices;
    static const Int32 sSimCount;// = TelephonyManager.getDefault().getSimCount();
    // Protects singleton instance lazy initialization.
    static Object sInstanceLock;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICEFACTORY_H__

