
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatServiceFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                          CatServiceFactory
//=====================================================================
//CAR_INTERFACE_IMPL(CatServiceFactory, Object, ICatServiceFactory);

static Int32 InitSimCount()
{
    //TODO return TelephonyManager.getDefault().getSimCount();
    return 1;
}
const Int32 CatServiceFactory::sSimCount = InitSimCount();
Object CatServiceFactory::sInstanceLock;

AutoPtr<ICatService> CatServiceFactory::MakeCatService(
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IContext* context,
    /* [in] */ IUiccCard* ic,
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // UiccCardApplication ca = null;
    // IccFileHandler fh = null;
    //
    // if (sCatServices == null) {
    //     sCatServices = new CatService[sSimCount];
    // }
    //
    // if (ci == null || context == null || ic == null) return null;
    //
    // //get first valid filehandler in the card.
    // for (int i = 0; i < ic.getNumApplications(); i++) {
    //     ca = ic.getApplicationIndex(i);
    //     if (ca != null && (ca.getType() != AppType.APPTYPE_UNKNOWN)) {
    //         fh = ca.getIccFileHandler();
    //         break;
    //     }
    // }
    //
    // synchronized (sInstanceLock) {
    //     if (fh == null) return null;
    //
    //     if (sCatServices[slotId] == null) {
    //         sCatServices[slotId] = new CatService(ci, context, fh, slotId);
    //     }
    // }
    // return sCatServices[slotId];
    assert(0);
    AutoPtr<ICatService> empty;
    return empty;
}

AutoPtr<ICatService> CatServiceFactory::GetCatService(
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // return ((sCatServices == null) ? null : sCatServices[slotId]);
    assert(0);
    AutoPtr<ICatService> empty;
    return empty;
}

void CatServiceFactory::DisposeCatService(
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // if (sCatServices != null) {
    //     sCatServices[slotId].dispose();
    //     sCatServices[slotId] = null;
    // }
    assert(0);
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
