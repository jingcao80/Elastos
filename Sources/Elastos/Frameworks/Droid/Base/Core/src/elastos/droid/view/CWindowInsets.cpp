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

#include "elastos/droid/view/CWindowInsets.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {


static AutoPtr<IRect> InitEMPTY_RECT()
{
    AutoPtr<IRect> rect;
    CRect::New(0, 0, 0, 0, (IRect**)&rect);
    return rect;
}

AutoPtr<IWindowInsets> CWindowInsets::CONSUMED;
AutoPtr<IRect> CWindowInsets::EMPTY_RECT = InitEMPTY_RECT();

CAR_OBJECT_IMPL(CWindowInsets);
CAR_INTERFACE_IMPL(CWindowInsets, Object, IWindowInsets);

AutoPtr<IWindowInsets> CWindowInsets::GetCONSUMED()
{
    if (CONSUMED == NULL) {
        AutoPtr<IWindowInsets> wi;
        CWindowInsets::New(NULL, NULL, NULL, FALSE, (IWindowInsets**)&CONSUMED);
    }
    return CONSUMED;
}

CWindowInsets::CWindowInsets()
    : mIsRound(FALSE)
    , mSystemWindowInsetsConsumed(FALSE)
    , mWindowDecorInsetsConsumed(FALSE)
    , mStableInsetsConsumed(FALSE)
{}

ECode CWindowInsets::constructor(
    /* [in] */ IRect* systemWindowInsets,
    /* [in] */ IRect* windowDecorInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean isRound)
{
    mSystemWindowInsetsConsumed = systemWindowInsets == NULL;
    mSystemWindowInsets = mSystemWindowInsetsConsumed ? EMPTY_RECT.Get() : systemWindowInsets;

    mWindowDecorInsetsConsumed = windowDecorInsets == NULL;
    mWindowDecorInsets = mWindowDecorInsetsConsumed ? EMPTY_RECT.Get() : windowDecorInsets;

    mStableInsetsConsumed = stableInsets == NULL;
    mStableInsets = mStableInsetsConsumed ? EMPTY_RECT.Get() : stableInsets;

    mIsRound = isRound;
    return NOERROR;
}

ECode CWindowInsets::constructor(
    /* [in] */ IWindowInsets* _src)
{
    CWindowInsets* src = (CWindowInsets*)_src;
    mSystemWindowInsets = src->mSystemWindowInsets;
    mWindowDecorInsets = src->mWindowDecorInsets;
    mStableInsets = src->mStableInsets;
    mSystemWindowInsetsConsumed = src->mSystemWindowInsetsConsumed;
    mWindowDecorInsetsConsumed = src->mWindowDecorInsetsConsumed;
    mStableInsetsConsumed = src->mStableInsetsConsumed;
    mIsRound = src->mIsRound;
    return NOERROR;
}

ECode CWindowInsets::constructor(
    /* [in] */ IRect* systemWindowInsets)
{
    return constructor(systemWindowInsets, NULL, NULL, FALSE);
}

ECode CWindowInsets::GetSystemWindowInsets(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    if (mTempRect == NULL) {
        CRect::New((IRect**)&mTempRect);
    }
    if (mSystemWindowInsets != NULL) {
        mTempRect->Set(mSystemWindowInsets);
    }
    else {
        // If there were no system window insets, this is just empty.
        mTempRect->SetEmpty();
    }
    *rect = mTempRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CWindowInsets::GetSystemWindowInsetLeft(
    /* [out] */ Int32* result)
{
    return mSystemWindowInsets->GetLeft(result);
}

ECode CWindowInsets::GetSystemWindowInsetTop(
    /* [out] */ Int32* result)
{
    return mSystemWindowInsets->GetTop(result);
}

ECode CWindowInsets::GetSystemWindowInsetRight(
    /* [out] */ Int32* result)
{
    return mSystemWindowInsets->GetRight(result);
}

ECode CWindowInsets::GetSystemWindowInsetBottom(
    /* [out] */ Int32* result)
{
    return mSystemWindowInsets->GetBottom(result);
}

ECode CWindowInsets::GetWindowDecorInsetLeft(
    /* [out] */ Int32* result)
{
    return mWindowDecorInsets->GetLeft(result);
}

ECode CWindowInsets::GetWindowDecorInsetTop(
    /* [out] */ Int32* result)
{
    return mWindowDecorInsets->GetTop(result);
}

ECode CWindowInsets::GetWindowDecorInsetRight(
    /* [out] */ Int32* result)
{
    return mWindowDecorInsets->GetRight(result);
}

ECode CWindowInsets::GetWindowDecorInsetBottom(
    /* [out] */ Int32* result)
{
    return mWindowDecorInsets->GetBottom(result);
}

ECode CWindowInsets::HasSystemWindowInsets(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    mSystemWindowInsets->Get(&left, &top, &right, &bottom);
    *has = left != 0 || top != 0 || right != 0 || bottom != 0;
    return NOERROR;
}

ECode CWindowInsets::HasWindowDecorInsets(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    mWindowDecorInsets->Get(&left, &top, &right, &bottom);
    *has = left != 0 || top != 0 || right != 0 || bottom != 0;
    return NOERROR;
}

ECode CWindowInsets::HasInsets(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Boolean value = FALSE;
    HasSystemWindowInsets(&value);
    if (!value) {
        HasWindowDecorInsets(&value);
    }
    *has = value;
    return NOERROR;
}

ECode CWindowInsets::IsConsumed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSystemWindowInsetsConsumed && mWindowDecorInsetsConsumed && mStableInsetsConsumed;
    return NOERROR;
}

ECode CWindowInsets::IsRound(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsRound;
    return NOERROR;
}

ECode CWindowInsets::ConsumeSystemWindowInsets(
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
    result->mSystemWindowInsets = EMPTY_RECT;
    result->mSystemWindowInsetsConsumed = TRUE;
    *insets = (IWindowInsets*)result.Get();
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ConsumeSystemWindowInsets(
    /* [in] */ Boolean left,
    /* [in] */ Boolean top,
    /* [in] */ Boolean right,
    /* [in] */ Boolean bottom,
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    if (left || top || right || bottom) {
        AutoPtr<CWindowInsets> result;
        CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
        Int32 l = 0, t = 0, r = 0, b = 0;
        mSystemWindowInsets->Get(&l, &t, &r, &b);
        result->mSystemWindowInsets->Set(left ? 0 : l, top ? 0 : t, right ? 0 : r, bottom ? 0 : b);
        *insets = (IWindowInsets*)result.Get();
        REFCOUNT_ADD(*insets);
        return NOERROR;
    }

    *insets = this;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ReplaceSystemWindowInsets(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
    result->mSystemWindowInsets->Set(left, top, right, bottom);
    *insets = result;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ReplaceSystemWindowInsets(
    /* [in] */ IRect* systemWindowInsets,
    /* [out] */ IWindowInsets** insets)
{
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
    result->mSystemWindowInsets->Set(systemWindowInsets);
    *insets = result;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ConsumeWindowDecorInsets(
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&insets);
    result->mWindowDecorInsets->Set(0, 0, 0, 0);
    result->mWindowDecorInsetsConsumed = TRUE;
    *insets = result;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ConsumeWindowDecorInsets(
    /* [in] */ Boolean left,
    /* [in] */ Boolean top,
    /* [in] */ Boolean right,
    /* [in] */ Boolean bottom,
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    if (left || top || right || bottom) {
        AutoPtr<CWindowInsets> result;
        CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
        Int32 l = 0, t = 0, r = 0, b = 0;
        mWindowDecorInsets->Get(&l, &t, &r, &b);
        result->mWindowDecorInsets->Set(left ? 0 : l, top ? 0 : t, right ? 0 : r, bottom ? 0 : b);
        *insets = result;
        REFCOUNT_ADD(*insets);
        return NOERROR;
    }
    *insets = this;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ReplaceWindowDecorInsets(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
    result->mWindowDecorInsets->Set(left, top, right, bottom);
    *insets = result;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::GetStableInsetTop(
    /* [out] */ Int32* result)
{
    return mStableInsets->GetTop(result);
}

ECode CWindowInsets::GetStableInsetLeft(
    /* [out] */ Int32* result)
{
    return mStableInsets->GetLeft(result);
}

ECode CWindowInsets::GetStableInsetRight(
    /* [out] */ Int32* result)
{
    return mStableInsets->GetRight(result);
}

ECode CWindowInsets::GetStableInsetBottom(
    /* [out] */ Int32* result)
{
    return mStableInsets->GetBottom(result);
}

ECode CWindowInsets::HasStableInsets(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    mStableInsets->Get(&left, &top, &right, &bottom);
    *has = top != 0 || left != 0 || right != 0 || bottom != 0;
    return NOERROR;
}

ECode CWindowInsets::ConsumeStableInsets(
    /* [out] */ IWindowInsets** insets)
{
    VALIDATE_NOT_NULL(insets);
    AutoPtr<CWindowInsets> result;
    CWindowInsets::NewByFriend(this, (CWindowInsets**)&result);
    result->mStableInsets = EMPTY_RECT;
    result->mStableInsetsConsumed = TRUE;
    *insets = result;
    REFCOUNT_ADD(*insets);
    return NOERROR;
}

ECode CWindowInsets::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb("WindowInsets{systemWindowInsets=");
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    mSystemWindowInsets->Get(&left, &top, &right, &bottom);
    sb.Append("left:");
    sb.Append(left);
    sb.Append(", top:");
    sb.Append(top);
    sb.Append(", right:");
    sb.Append(right);
    sb.Append(", bottom:");
    sb.Append(bottom);

    mWindowDecorInsets->Get(&left, &top, &right, &bottom);
    sb.Append("; windowDecorInsets=left:");
    sb.Append(left);
    sb.Append(", top:");
    sb.Append(top);
    sb.Append(", right:");
    sb.Append(right);
    sb.Append(", bottom:");
    sb.Append(bottom);

    mStableInsets->Get(&left, &top, &right, &bottom);
    sb.Append("; stableInsets=left:");
    sb.Append(left);
    sb.Append(", top:");
    sb.Append(top);
    sb.Append(", right:");
    sb.Append(right);
    sb.Append(", bottom:");
    sb.Append(bottom);

    Boolean round = FALSE;
    if (round) {
        sb.Append(" round}");
    }
    else {
        sb.Append("}");
    }

    return sb.ToString(str);
}

}// namespace View
}// namespace Droid
}// namespace Elastos
