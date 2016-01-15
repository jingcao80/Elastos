
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEFALLBACKEVENTHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEFALLBACKEVENTHANDLER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.View.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IFallbackEventHandler;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneFallbackEventHandler
    : public Object
    , public IPhoneFallbackEventHandler
    , public IFallbackEventHandler
{
public:
    CAR_INTERFACE_DECL()

    PhoneFallbackEventHandler();

    CARAPI constructor(
        /* [in] */ IContext* context);

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
    static const Boolean DEBUG;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEFALLBACKEVENTHANDLER_H__
