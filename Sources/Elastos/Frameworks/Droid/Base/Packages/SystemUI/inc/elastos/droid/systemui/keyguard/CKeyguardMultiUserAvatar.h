
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMULTIUSERAVATAR_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMULTIUSERAVATAR_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardMultiUserAvatar.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Widget::FrameLayout;

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
            /* [in] */ Float initScale)
            : mHost(host)
            , mFinalAlpha(finalAlpha)
            , mFinalScale(finalScale)
            , mInitAlpha(initAlpha)
            , mInitScale(initScale)
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

    const Float mActiveAlpha;
    const Float mActiveScale;
    const Float mActiveTextAlpha;
    const Float mInactiveAlpha;
    const Float mInactiveTextAlpha;
    const Float mShadowRadius;
    const Float mStroke;
    const Float mIconSize;
    const Int32 mFrameColor;
    const Int32 mFrameShadowColor;
    const Int32 mTextColor;
    const Int32 mHighlightColor;

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
