
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/view/RotationPolicy.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::View::IRotationPolicyListener;
using Elastos::Droid::Internal::View::RotationPolicy;
using Elastos::Core::Object;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/** Platform implementation of the rotation lock controller. **/
class RotationLockControllerImpl
    : public Object
    , public IRotationLockController
    , public IListenable
{
private:
    class LocalRotationPolicyListener: public RotationPolicy::RotationPolicyListener
    {
    public:
        LocalRotationPolicyListener(
            /* [in] */ RotationLockControllerImpl* host);

        // @Override
        CARAPI OnChange();

    private:
        RotationLockControllerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RotationLockControllerImpl(
        /* [in] */ IContext* context);

    CARAPI AddRotationLockControllerCallback(
        /* [in] */ IRotationLockControllerCallback* callback);

    CARAPI RemoveRotationLockControllerCallback(
        /* [in] */ IRotationLockControllerCallback* callback);

    CARAPI GetRotationLockOrientation(
        /* [out] */ Int32* result);

    CARAPI IsRotationLocked(
        /* [out] */ Boolean* result);

    CARAPI SetRotationLocked(
        /* [in] */ Boolean locked);

    CARAPI IsRotationLockAffordanceVisible(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

private:
    CARAPI_(void) NotifyChanged();

    CARAPI_(void) NotifyChanged(
        /* [in] */ IRotationLockControllerCallback* callback);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ICopyOnWriteArrayList> mCallbacks;  /*<RotationLockControllerCallback*/

    AutoPtr<IRotationPolicyListener> mRotationPolicyListener;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
