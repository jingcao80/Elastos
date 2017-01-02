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

#include "elastos/droid/systemui/statusbar/phone/CUserAvatarView.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CUserAvatarView)
CAR_INTERFACE_IMPL(CUserAvatarView, View, IUserAvatarView)
CUserAvatarView::CUserAvatarView()
    : mActiveFrameColor(0)
    , mFrameColor(0)
    , mFrameWidth(0)
    , mFramePadding(0)
    , mScale(1)
{
    CPaint::New((IPaint**)&mFramePaint);
    CPaint::New((IPaint**)&mBitmapPaint);
    CMatrix::New((IMatrix**)&mDrawMatrix);
}

ECode CUserAvatarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    View::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::UserAvatarView);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 N = 0;
    a->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 attr = 0;
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::UserAvatarView_frameWidth: {
                Float v = 0;
                SetFrameWidth((a->GetDimension(attr, 0, &v), v));
                break;
            }
            case R::styleable::UserAvatarView_framePadding: {
                Float v = 0;
                SetFramePadding((a->GetDimension(attr, 0, &v), v));
                break;
            }
            case R::styleable::UserAvatarView_activeFrameColor: {
                Int32 v = 0;
                SetActiveFrameColor((a->GetColor(attr, 0, &v), v));
                break;
            }
            case R::styleable::UserAvatarView_frameColor: {
                Int32 v = 0;
                SetFrameColor((a->GetColor(attr, 0, &v), v));
                break;
            }
        }
    }
    a->Recycle();

    mFramePaint->SetAntiAlias(TRUE);
    mFramePaint->SetStyle(PaintStyle_STROKE);
    mBitmapPaint->SetAntiAlias(TRUE);
    return NOERROR;
}

ECode CUserAvatarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CUserAvatarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CUserAvatarView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CUserAvatarView::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    SetDrawable(NULL);
    mBitmap = bitmap;
    if (mBitmap != NULL) {
        AutoPtr<IShader> sh;
        CBitmapShader::New(bitmap, ShaderTileMode_CLAMP, ShaderTileMode_CLAMP, (IShader**)&sh);
        mBitmapPaint->SetShader(sh);
    }
    else {
        mBitmapPaint->SetShader(NULL);
    }
    ConfigureBounds();
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::SetFrameColor(
    /* [in] */ Int32 frameColor)
{
    mFrameColor = frameColor;
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::SetActiveFrameColor(
    /* [in] */ Int32 activeFrameColor)
{
    mActiveFrameColor = activeFrameColor;
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::SetFrameWidth(
    /* [in] */ Float frameWidth)
{
    mFrameWidth = frameWidth;
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::SetFramePadding(
    /* [in] */ Float framePadding)
{
    mFramePadding = framePadding;
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    View::OnLayout(changed, left, top, right, bottom);
    ConfigureBounds();
    return NOERROR;
}

ECode CUserAvatarView::ConfigureBounds()
{
    Int32 v = 0;
    Int32 vwidth = (GetWidth(&v), v) - mPaddingLeft - mPaddingRight;
    Int32 vheight = (GetHeight(&v), v) - mPaddingTop - mPaddingBottom;

    Int32 dwidth;
    Int32 dheight;
    if (mBitmap != NULL) {
        mBitmap->GetWidth(&dwidth);
        mBitmap->GetHeight(&dheight);
    }
    else if (mDrawable != NULL) {
        vwidth -= 2 * (mFrameWidth - 1);
        vheight -= 2 * (mFrameWidth - 1);
        dwidth = vwidth;
        dheight = vheight;
        mDrawable->SetBounds(0, 0, dwidth, dheight);
    }
    else {
        return NOERROR;
    }

    Float scale;
    Float dx;
    Float dy;

    scale = Elastos::Core::Math::Min((Float) vwidth / (Float) dwidth,
            (Float) vheight / (Float) dheight);

    dx = (Int32) ((vwidth - dwidth * scale) * 0.5f + 0.5f);
    dy = (Int32) ((vheight - dheight * scale) * 0.5f + 0.5f);

    mDrawMatrix->SetScale(scale, scale);
    Boolean tmp = FALSE;
    mDrawMatrix->PostTranslate(dx, dy, &tmp);
    mScale = scale;
    return NOERROR;
}

void CUserAvatarView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Boolean tmp = FALSE;
    Int32 frameColor = (IsActivated(&tmp), tmp) ? mActiveFrameColor : mFrameColor;
    Int32 v = 0;
    Float halfW = (GetWidth(&v), v) / 2.f;
    Float halfH = (GetHeight(&v), v) / 2.f;
    Float halfSW = Elastos::Core::Math::Min(halfH, halfW);
    if (mBitmap != NULL && mScale > 0) {
        Int32 saveCount = 0, tv = 0;
        canvas->GetSaveCount(&saveCount);
        canvas->Save(&tv);
        canvas->Translate(mPaddingLeft, mPaddingTop);
        canvas->Concat(mDrawMatrix);
        Float halfBW = (mBitmap->GetWidth(&v), v) / 2.f;
        Float halfBH = (mBitmap->GetHeight(&v), v) / 2.f;
        Float halfBSW = Elastos::Core::Math::Min(halfBH, halfBW);
        canvas->DrawCircle(halfBW, halfBH, halfBSW - mFrameWidth / mScale + 1, mBitmapPaint);
        canvas->RestoreToCount(saveCount);
    }
    else if (mDrawable != NULL && mScale > 0) {
        Int32 saveCount = 0, tv = 0;
        canvas->GetSaveCount(&saveCount);
        canvas->Save(&tv);
        canvas->Translate(mPaddingLeft, mPaddingTop);
        canvas->Translate(mFrameWidth - 1, mFrameWidth - 1);
        canvas->Concat(mDrawMatrix);
        mDrawable->Draw(canvas);
        canvas->RestoreToCount(saveCount);
    }
    if (frameColor != 0) {
        mFramePaint->SetColor(frameColor);
        mFramePaint->SetStrokeWidth(mFrameWidth);
        canvas->DrawCircle(halfW, halfH, halfSW + (mFramePadding - mFrameWidth) / 2.f,
                mFramePaint);
    }
}

ECode CUserAvatarView::SetDrawable(
    /* [in] */ IDrawable* d)
{
    if (mDrawable != NULL) {
        mDrawable->SetCallback(NULL);
        UnscheduleDrawable(mDrawable);
    }
    mDrawable = d;
    if (d != NULL) {
        d->SetCallback(this);
        Boolean tmp = FALSE;
        if (d->IsStateful(&tmp), tmp) {
            AutoPtr<ArrayOf<Int32> > ivs;
            GetDrawableState((ArrayOf<Int32>**)&ivs);
            d->SetState(ivs, &tmp);
        }
        Int32 dir = 0;
        GetLayoutDirection(&dir);
        d->SetLayoutDirection(dir);
        ConfigureBounds();
    }
    if (d != NULL) {
        mBitmap = NULL;
    }
    ConfigureBounds();
    Invalidate();
    return NOERROR;
}

ECode CUserAvatarView::InvalidateDrawable(
    /* [in] */ IDrawable* dr)
{
    if (dr == mDrawable) {
        Invalidate();
    }
    else {
        View::InvalidateDrawable(dr);
    }
    return NOERROR;
}

Boolean CUserAvatarView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mDrawable || View::VerifyDrawable(who);
}

ECode CUserAvatarView::DrawableStateChanged()
{
    View::DrawableStateChanged();
    Boolean tmp = FALSE;
    if (mDrawable != NULL && (mDrawable->IsStateful(&tmp), tmp)) {
        AutoPtr<ArrayOf<Int32> > ivs;
        GetDrawableState((ArrayOf<Int32>**)&ivs);
        mDrawable->SetState(ivs, &tmp);
    }
    return NOERROR;
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
