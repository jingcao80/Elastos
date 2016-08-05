
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMULTIUSERAVATAR_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMULTIUSERAVATAR_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardMultiUserAvatar.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/droid/widget/FrameLayout.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardMultiUserAvatar)
    , public FrameLayout
    , public IKeyguardMultiUserAvatar
{
private:
    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CKeyguardMultiUserAvatar::MyAnimatorUpdateListener")

        MyAnimatorUpdateListener(
            /* [in] */ CKeyguardMultiUserAvatar* host,
            /* [in] */ Float finalAlpha,
            /* [in] */ Float finalScale,
            /* [in] */ Float initAlpha,
            /* [in] */ Float initScale,
            /* [in] */ Int32 initTextAlpha,
            /* [in] */ Int32 finalTextAlpha)
            : mHost(host)
            , mFinalAlpha(finalAlpha)
            , mFinalScale(finalScale)
            , mInitAlpha(initAlpha)
            , mInitScale(initScale)
            , mInitTextAlpha(initTextAlpha)
            , mFinalTextAlpha(finalTextAlpha)
        {}

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardMultiUserAvatar* mHost;
        Float mFinalAlpha;
        Float mFinalScale;
        Float mInitAlpha;
        Float mInitScale;
        Int32 mInitTextAlpha;
        Int32 mFinalTextAlpha;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* onComplete)
            : mOnComplete(onComplete)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mOnComplete;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardMultiUserAvatar();

    static CARAPI_(AutoPtr<IKeyguardMultiUserAvatar>) FromXml(
        /* [in] */ Int32 resId,
        /* [in] */ IContext* context,
        /* [in] */ IKeyguardMultiUserSelectorView* userSelector,
        /* [in] */ IUserInfo* info);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Init(
        /* [in] */ IUserInfo* user,
        /* [in] */ IKeyguardMultiUserSelectorView* userSelector);

    CARAPI SetActive(
        /* [in] */ Boolean active,
        /* [in] */ Boolean animate,
        /* [in] */ IRunnable* onComplete);

    CARAPI UpdateVisualsForActive(
        /* [in] */ Boolean active,
        /* [in] */ Boolean animate,
        /* [in] */ Int32 duration,
        /* [in] */ IRunnable* onComplete);

    //@Override
    CARAPI SetPressed(
        /* [in] */ Boolean pressed);

    CARAPI LockPressed(
        /* [in] */ Boolean pressed);

    CARAPI GetUserInfo(
        /* [out] */ IUserInfo** info);

protected:
    CARAPI RewriteIconPath(
        /* [in] */ const String& path,
        /* [out] */ String* str);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IImageView> mUserImage;
    AutoPtr<ITextView> mUserName;
    AutoPtr<IUserInfo> mUserInfo;
    static const Float ACTIVE_ALPHA;
    static const Float INACTIVE_ALPHA;
    static const Float ACTIVE_SCALE;
    static const Float ACTIVE_TEXT_ALPHA;
    static const Float INACTIVE_TEXT_ALPHA;
    static const Int32 SWITCH_ANIMATION_DURATION;

    Float mActiveAlpha;
    Float mActiveScale;
    Float mActiveTextAlpha;
    Float mInactiveAlpha;
    Float mInactiveTextAlpha;
    Float mShadowRadius;
    Float mStroke;
    Float mIconSize;
    Int32 mFrameColor;
    Int32 mFrameShadowColor;
    Int32 mTextColor;
    Int32 mHighlightColor;

    Boolean mTouched;

    Boolean mActive;
    Boolean mInit;
    AutoPtr<IKeyguardMultiUserSelectorView> mUserSelector;
    AutoPtr<IKeyguardCircleFramedDrawable> mFramed;
    Boolean mPressLock;
    AutoPtr<IUserManager> mUserManager;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMULTIUSERAVATAR_H__
