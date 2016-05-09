
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/systemui/keyguard/CKeyguardViewMediator.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class CIKeyguardService;

class KeyguardService
    : public Elastos::Droid::App::Service
    , public IKeyguardService
{
public:
    CAR_INTERFACE_DECL();

    KeyguardService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    CARAPI CheckPermission();

public:
    const static String TAG;
    const static String PERMISSION;

private:
    AutoPtr<CKeyguardViewMediator> mKeyguardViewMediator;

    AutoPtr<IBinder> mBinder;
    friend class CIKeyguardService;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__