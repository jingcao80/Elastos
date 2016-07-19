
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardUpdateMonitorUserSwitchObserver.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardUpdateMonitorUserSwitchObserver)
    , public Object
    , public IIUserSwitchObserver
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IKeyguardUpdateMonitorUser* monitor);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    KeyguardUpdateMonitorUser* mHost;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__
