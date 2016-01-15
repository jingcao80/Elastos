
#include "pm/CResourcesChangedReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CResourcesChangedReceiver, Object, IIntentReceiver, IBinder)

CAR_OBJECT_IMPL(CResourcesChangedReceiver)

ECode CResourcesChangedReceiver::constructor(
    /* [in] */ IIPackageManager* owner,
    /* [in] */ ISet* keys,
    /* [in] */ Boolean reportStatus)
{
    mOwner = reinterpret_cast<CPackageManagerService*>(owner->Probe(EIID_CPackageManagerService));
    mKeys = keys;
    mReportStatus = reportStatus;
    return NOERROR;
}

ECode CResourcesChangedReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(CPackageManagerService::UPDATED_MEDIA_STATUS, (IMessage**)&msg);
    msg->SetObj(mKeys);
    msg->SetArg1(mReportStatus ? 1 : 0);
    msg->SetArg2(1);
    Boolean result;
    return mOwner->mHandler->SendMessage(msg, &result);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
