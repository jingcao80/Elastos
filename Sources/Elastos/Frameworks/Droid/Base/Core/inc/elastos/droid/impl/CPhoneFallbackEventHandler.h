
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEFALLBACKEVENTHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEFALLBACKEVENTHANDLER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneFallbackEventHandler.h"

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneFallbackEventHandler)
{
public:
    CARAPI SetView(
        /* [in] */ IView *v);

    CARAPI PreDispatchKeyEvent(
        /* [in] */ IKeyEvent *event);

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent *event,
        /* [out] */ Boolean *res);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent *event,
        /* [out] */ Boolean *res);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent *event,
        /* [out] */ Boolean *res);

    CARAPI constructor(
        /* [in] */ IContext *context);

    CARAPI StartCallActivity();

    CARAPI GetSearchManager(
        /* [out] */ ISearchManager **sm);

    CARAPI GetTelephonyManager(
        /* [out] */ ITelephonyManager ** tm);

    CARAPI GetKeyguardManager(
        /* [out] */ IKeyguardManager **km);

    CARAPI GetAudioManager(
        /* [out] */ IAudioManager **am);

    CARAPI SendCloseSystemWindows();

private:
    CARAPI HandleMediaKeyEvent(
        /* [in] */ IKeyEvent *keyEvent);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IView> mView;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    AutoPtr<ISearchManager> mSearchManager;
    AutoPtr<ITelephonyManager> mTelephonyManager;

private:
    static String TAG;
    static const Boolean DEBUG = FALSE;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEFALLBACKEVENTHANDLER_H__
