
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDTOUCHDELEGATE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDTOUCHDELEGATE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Internal::Policy::IIKeyguardService;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Facilitates event communication between navigation bar and keyguard.  Currently used to
 * control WidgetPager in keyguard to expose the camera widget.
 *
 */
class KeyguardTouchDelegate
    : public Object
    , public IKeyguardTouchDelegate
{
private:
    class KeyguardConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        KeyguardConnection(
            /* [in] */ KeyguardTouchDelegate* host);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        KeyguardTouchDelegate* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<KeyguardTouchDelegate>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI IsSecure(
        /* [out] */ Boolean* result);

    CARAPI Dispatch(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI IsInputRestricted(
        /* [out] */ Boolean* result);

    CARAPI IsShowingAndNotOccluded(
        /* [out] */ Boolean* result);

    CARAPI ShowAssistant();

    CARAPI LaunchCamera();

    CARAPI Dismiss();

    static CARAPI_(void) AddListener(
        /* [in] */ IOnKeyguardConnectionListener* listener);

private:
    KeyguardTouchDelegate(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IList>) InitList();

protected:
    static const Boolean DEBUG;
    static const String TAG;

private:
    // TODO: propagate changes to these to {@link KeyguardServiceDelegate}
    static const String KEYGUARD_PACKAGE;
    static const String KEYGUARD_CLASS;

    static AutoPtr<KeyguardTouchDelegate> sInstance;
    static AutoPtr<IList> sConnectionListeners;  /*<OnKeyguardConnectionListener*/

    /*volatile*/ AutoPtr<IIKeyguardService> mService;

    AutoPtr<IServiceConnection> mKeyguardConnection;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDTOUCHDELEGATE_H__
