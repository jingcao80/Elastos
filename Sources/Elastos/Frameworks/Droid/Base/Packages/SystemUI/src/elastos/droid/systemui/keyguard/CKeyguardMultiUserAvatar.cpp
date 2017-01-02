//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/keyguard/CKeyguardMultiUserAvatar.h"
#include "elastos/droid/systemui/keyguard/MultiUserAvatarCache.h"
#include "elastos/droid/systemui/keyguard/KeyguardCircleFramedDrawable.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardMultiUserAvatar::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode CKeyguardMultiUserAvatar::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Float r;
    animation->GetAnimatedFraction(&r);
    Float scale = (1 - r) * mInitScale + r * mFinalScale;
    Float alpha = (1 - r) * mInitAlpha + r * mFinalAlpha;
    Int32 textAlpha = (Int32) ((1 - r) * mInitTextAlpha + r * mFinalTextAlpha);
    mHost->mFramed->SetScale(scale);
    mHost->mUserImage->SetAlpha(alpha);
    AutoPtr<IColor> helper;
    CColor::AcquireSingleton((IColor**)&helper);
    Int32 argb;
    helper->Argb(textAlpha, 255, 255, 255, &argb);
    mHost->mUserName->SetTextColor(argb);
    return IView::Probe(mHost->mUserImage)->Invalidate();
}

ECode CKeyguardMultiUserAvatar::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mOnComplete != NULL) {
        mOnComplete->Run();
    }
    return NOERROR;
}

const String CKeyguardMultiUserAvatar::TAG("KeyguardMultiUserAvatar");
const Boolean CKeyguardMultiUserAvatar::DEBUG = IKeyguardConstants::DEBUG;

const Float CKeyguardMultiUserAvatar::ACTIVE_ALPHA = 1.0f;
const Float CKeyguardMultiUserAvatar::INACTIVE_ALPHA = 1.0f;
const Float CKeyguardMultiUserAvatar::ACTIVE_SCALE = 1.5f;
const Float CKeyguardMultiUserAvatar::ACTIVE_TEXT_ALPHA = 0.0f;
const Float CKeyguardMultiUserAvatar::INACTIVE_TEXT_ALPHA = 0.5f;
const Int32 CKeyguardMultiUserAvatar::SWITCH_ANIMATION_DURATION = 150;

CAR_OBJECT_IMPL(CKeyguardMultiUserAvatar)

CAR_INTERFACE_IMPL(CKeyguardMultiUserAvatar, FrameLayout, IKeyguardMultiUserAvatar)

CKeyguardMultiUserAvatar::CKeyguardMultiUserAvatar()
    : mActiveAlpha(0.0f)
    , mActiveScale(0.0f)
    , mActiveTextAlpha(0.0f)
    , mInactiveAlpha(0.0f)
    , mInactiveTextAlpha(0.0f)
    , mShadowRadius(0.0f)
    , mStroke(0.0f)
    , mIconSize(0.0f)
    , mFrameColor(0.0f)
    , mFrameShadowColor(0.0f)
    , mTextColor(0.0f)
    , mHighlightColor(0.0f)
    , mTouched(FALSE)
    , mActive(FALSE)
    , mInit(TRUE)
    , mPressLock(TRUE)
{
}

AutoPtr<IKeyguardMultiUserAvatar> CKeyguardMultiUserAvatar::FromXml(
    /* [in] */ Int32 resId,
    /* [in] */ IContext* context,
    /* [in] */ IKeyguardMultiUserSelectorView* userSelector,
    /* [in] */ IUserInfo* info)
{
    AutoPtr<ILayoutInflater> flater;
    LayoutInflater::From(context, (ILayoutInflater**)&flater);
    AutoPtr<IView> view;
    flater->Inflate(resId, IViewGroup::Probe(userSelector), FALSE, (IView**)&view);
    AutoPtr<IKeyguardMultiUserAvatar> icon = IKeyguardMultiUserAvatar::Probe(view);
    icon->Init(info, userSelector);
    return icon;
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetColor(R::color::keyguard_avatar_nick_color, &mTextColor);
    res->GetDimension(R::dimen::keyguard_avatar_size, &mIconSize);
    res->GetDimension(R::dimen::keyguard_avatar_frame_stroke_width, &mStroke);
    res->GetDimension(R::dimen::keyguard_avatar_frame_shadow_radius, &mShadowRadius);
    res->GetColor(R::color::keyguard_avatar_frame_color, &mFrameColor);
    res->GetColor(R::color::keyguard_avatar_frame_shadow_color, &mFrameShadowColor);
    res->GetColor(R::color::keyguard_avatar_frame_pressed_color, &mHighlightColor);
    mActiveTextAlpha = ACTIVE_TEXT_ALPHA;
    mInactiveTextAlpha = INACTIVE_TEXT_ALPHA;
    mActiveScale = ACTIVE_SCALE;
    mActiveAlpha = ACTIVE_ALPHA;
    mInactiveAlpha = INACTIVE_ALPHA;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    mTouched = FALSE;

    return SetLayerType(IView::LAYER_TYPE_SOFTWARE, NULL);
}

ECode CKeyguardMultiUserAvatar::RewriteIconPath(
    /* [in] */ const String& path,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = path;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::Init(
    /* [in] */ IUserInfo* user,
    /* [in] */ IKeyguardMultiUserSelectorView* userSelector)
{
    mUserInfo = user;
    mUserSelector = userSelector;

    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_user_avatar, (IView**)&view);
    mUserImage = IImageView::Probe(view);

    AutoPtr<IView> view2;
    FindViewById(R::id::keyguard_user_name, (IView**)&view2);
    mUserName = ITextView::Probe(view2);

    AutoPtr<MultiUserAvatarCache> cache = MultiUserAvatarCache::GetInstance();
    Int32 id;
    user->GetId(&id);
    AutoPtr<IDrawable> drawable;
    cache->Get(id, (IDrawable**)&drawable);
    mFramed = IKeyguardCircleFramedDrawable::Probe(drawable);

    // If we can't find it or the params don't match, create the drawable again
    Boolean res;
    if (mFramed == NULL
            || (mFramed->VerifyParams(mIconSize, mFrameColor, mStroke, mFrameShadowColor,
                    mShadowRadius, mHighlightColor, &res), !res)) {
        AutoPtr<IBitmap> icon;
        //try {
        mUserManager->GetUserIcon(id, (IBitmap**)&icon);
        //} catch (Exception e) {
        //    if (DEBUG) Log.d(TAG, "failed to get profile icon " + user, e);
        //}

        if (icon == NULL) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            AutoPtr<IBitmapFactory> helper;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
            helper->DecodeResource(resources, Elastos::Droid::R::drawable::ic_contact_picture, (IBitmap**)&icon);
        }

        mFramed = new KeyguardCircleFramedDrawable(icon, (Int32)mIconSize, mFrameColor, mStroke,
                mFrameShadowColor, mShadowRadius, mHighlightColor);
        MultiUserAvatarCache::GetInstance()->Put(id, IDrawable::Probe(mFramed));
    }

    mFramed->Reset();

    mUserImage->SetImageDrawable(IDrawable::Probe(mFramed));
    String name;
    mUserInfo->GetName(&name);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(name);
    mUserName->SetText(cchar);
    SetOnClickListener(IViewOnClickListener::Probe(mUserSelector));
    mInit = FALSE;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::SetActive(
    /* [in] */ Boolean active,
    /* [in] */ Boolean animate,
    /* [in] */ IRunnable* onComplete)
{
    if (mActive != active || mInit) {
        mActive = active;

        if (active) {
            AutoPtr<IViewParent> res;
            GetParent((IViewParent**)&res);
            AutoPtr<IKeyguardLinearLayout> parent = IKeyguardLinearLayout::Probe(res);
            parent->SetTopChild(this);
            // TODO: Create an appropriate asset when string changes are possible.
            AutoPtr<ICharSequence> cchar;
            mUserName->GetText((ICharSequence**)&cchar);
            String text;
            cchar->ToString(&text);

            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
            AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(String(""));
            array->Set(0, TO_IINTERFACE(cchar2));
            String str;
            mContext->GetString(R::string::user_switched, array, &str);

            StringBuilder sb;
            sb += text;
            sb += ". ";
            sb += str;
            String tmp;
            sb.ToString(&tmp);
            AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(tmp);
            SetContentDescription(cchar3);
        }
        else {
            AutoPtr<ICharSequence> text;
            mUserName->GetText((ICharSequence**)&text);
            SetContentDescription(text);
        }
    }
    return UpdateVisualsForActive(mActive, animate, SWITCH_ANIMATION_DURATION, onComplete);
}

ECode CKeyguardMultiUserAvatar::UpdateVisualsForActive(
    /* [in] */ Boolean active,
    /* [in] */ Boolean animate,
    /* [in] */ Int32 duration,
    /* [in] */ IRunnable* onComplete)
{
    Float finalAlpha = active ? mActiveAlpha : mInactiveAlpha;
    Float initAlpha = active ? mInactiveAlpha : mActiveAlpha;
    Float finalScale = active ? 1.0f : 1.0f / mActiveScale;
    Float initScale;
    mFramed->GetScale(&initScale);
    Int32 finalTextAlpha = active ? (Int32) (mActiveTextAlpha * 255) :
            (Int32) (mInactiveTextAlpha * 255);
    Int32 initTextAlpha = active ? (Int32) (mInactiveTextAlpha * 255) :
            (Int32) (mActiveTextAlpha * 255);
    Int32 textColor = mTextColor;
    mUserName->SetTextColor(textColor);

    if (animate && mTouched) {
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = 0.0f;
        (*array)[1] = 1.0f;
        AutoPtr<IValueAnimator> va;
        helper->OfFloat(array, (IValueAnimator**)&va);

        AutoPtr<IAnimatorUpdateListener> lis = new MyAnimatorUpdateListener(this,
                finalAlpha, finalScale, initAlpha, initScale, initTextAlpha,
                finalTextAlpha);
        va->AddUpdateListener(lis);
        AutoPtr<IAnimatorListener> lis2 = new MyAnimatorListenerAdapter(onComplete);
        IAnimator::Probe(va)->AddListener(lis2);
        IAnimator::Probe(va)->SetDuration(duration);
        IAnimator::Probe(va)->Start();
    }
    else {
        mFramed->SetScale(finalScale);
        mUserImage->SetAlpha(finalAlpha);
        AutoPtr<IColor> helper;
        CColor::AcquireSingleton((IColor**)&helper);
        Int32 argb;
        helper->Argb(finalTextAlpha, 255, 255, 255, &argb);
        mUserName->SetTextColor(argb);
        if (onComplete != NULL) {
            Boolean res;
            Post(onComplete, &res);
        }
    }

    mTouched = TRUE;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::SetPressed(
    /* [in] */ Boolean pressed)
{
    if (mPressLock && !pressed) {
        return NOERROR;
    }

    Boolean res;
    if (mPressLock || !pressed || (IsClickable(&res), res)) {
        FrameLayout::SetPressed(pressed);
        mFramed->SetPressed(pressed);
        IView::Probe(mUserImage)->Invalidate();
    }
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::LockPressed(
    /* [in] */ Boolean pressed)
{
    mPressLock = pressed;
    return SetPressed(pressed);
}

ECode CKeyguardMultiUserAvatar::GetUserInfo(
    /* [out] */ IUserInfo** info)
{
    VALIDATE_NOT_NULL(info)

    *info = mUserInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
