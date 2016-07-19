
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorUserSwitchObserver.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CKeyguardUpdateMonitorUserSwitchObserver)

CAR_INTERFACE_IMPL_2(CKeyguardUpdateMonitorUserSwitchObserver, Object, IIUserSwitchObserver, IBinder)

ECode CKeyguardUpdateMonitorUserSwitchObserver::constructor(
    /* [in] */ IKeyguardUpdateMonitorUser* monitor)
{
    mHost = (KeyguardUpdateMonitorUser*)monitor;
    return NOERROR;
}

ECode CKeyguardUpdateMonitorUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(MSG_USER_SWITCHING, newUserId, 0, reply, (IMessage**)&m);
    mHost->mHandler->SendMessage(m);
    mHost->mSwitchingUser = TRUE;
    return NOERROR;
}

ECode CKeyguardUpdateMonitorUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(MSG_USER_SWITCH_COMPLETE, newUserId, 0, (IMessage**)&m);
    mHost->mHandler->SendMessage(m);
    mHost->mSwitchingUser = FALSE;
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
