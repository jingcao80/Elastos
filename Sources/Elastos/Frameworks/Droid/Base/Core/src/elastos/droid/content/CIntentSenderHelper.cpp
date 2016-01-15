
#include "elastos/droid/content/CIntentSenderHelper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentSenderHelper, Singleton, IIntentSenderHelper)

CAR_SINGLETON_IMPL(CIntentSenderHelper)

ECode CIntentSenderHelper::WriteIntentSenderOrNullToParcel(
    /* [in] */ IIntentSender* sender,
    /* [in] */ IParcel* parcel)
{
    assert(0 && "TODO");
//    out.writeStrongBinder(sender != null ? sender.mTarget.asBinder() : null);
    return E_NOT_IMPLEMENTED;
}

ECode CIntentSenderHelper::ReadIntentSenderOrNullFromParcel(
    /* [in] */ IParcel* parcel,
    /* [out] */ IIntentSender** intentSender)
{
    assert(0 && "TODO");
//    IBinder b = in.readStrongBinder();
//    return b != null ? new IntentSender(b) : null;
    return E_NOT_IMPLEMENTED;
}

} // Content
} // Droid
} // Elastos
