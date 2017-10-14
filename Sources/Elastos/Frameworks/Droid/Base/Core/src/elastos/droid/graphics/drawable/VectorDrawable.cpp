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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "elastos/droid/graphics/drawable/CVectorDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPathMeasure.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/drawable/CVGroup.h"
#include "elastos/droid/graphics/drawable/CVFullPath.h"
#include "elastos/droid/graphics/drawable/CVClipPath.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <math.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::ILayoutDirection;
using Elastos::Droid::Utility::PathParser;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CStack;
using Elastos::Utility::IStack;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

const String VectorDrawable::LOGTAG("VectorDrawable")/*.class.getSimpleName()*/;
const String VectorDrawable::SHAPE_CLIP_PATH("clip-path");
const String VectorDrawable::SHAPE_GROUP("group");
const String VectorDrawable::SHAPE_PATH("path");
const String VectorDrawable::SHAPE_VECTOR("vector");
const Int32 VectorDrawable::LINECAP_BUTT = 0;
const Int32 VectorDrawable::LINECAP_ROUND = 1;
const Int32 VectorDrawable::LINECAP_SQUARE = 2;
const Int32 VectorDrawable::LINEJOIN_MITER = 0;
const Int32 VectorDrawable::LINEJOIN_ROUND = 1;
const Int32 VectorDrawable::LINEJOIN_BEVEL = 2;
const Boolean VectorDrawable::DBG_VECTOR_DRAWABLE = TRUE;

//================================================================
// VectorDrawable::VectorDrawableState
//================================================================
VectorDrawable::VectorDrawableState::VectorDrawableState(
    /* [in] */ VectorDrawableState* copy)
    : mChangingConfigurations(0)
    , mTintMode(DEFAULT_TINT_MODE)
    , mAutoMirrored(FALSE)
    , mCachedTintMode(-1)
    , mCachedRootAlpha(0)
    , mCachedAutoMirrored(FALSE)
    , mCacheDirty(FALSE)
{
    if (copy != NULL) {
        mThemeAttrs = copy->mThemeAttrs;
        mChangingConfigurations = copy->mChangingConfigurations;
        mVPathRenderer = new VPathRenderer(copy->mVPathRenderer);
        if (copy->mVPathRenderer->mFillPaint != NULL) {
            CPaint::New(copy->mVPathRenderer->mFillPaint, (IPaint**)&mVPathRenderer->mFillPaint);
        }
        if (copy->mVPathRenderer->mStrokePaint != NULL) {
            CPaint::New(copy->mVPathRenderer->mStrokePaint, (IPaint**)&mVPathRenderer->mStrokePaint);
        }
        mTint = copy->mTint;
        mTintMode = copy->mTintMode;
        mAutoMirrored = copy->mAutoMirrored;
    }
}

ECode VectorDrawable::VectorDrawableState::DrawCachedBitmapWithRootAlpha(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IColorFilter* filter)
{
    // The bitmap's size is the same as the bounds.
    AutoPtr<IPaint> p = GetPaint(filter);
    return canvas->DrawBitmap(mCachedBitmap, 0.f, 0.f, p);
}

Boolean VectorDrawable::VectorDrawableState::HasTranslucentRoot()
{
    return mVPathRenderer->GetRootAlpha() < 255;
}

AutoPtr<IPaint> VectorDrawable::VectorDrawableState::GetPaint(
    /* [in] */ IColorFilter* filter)
{
    if (!HasTranslucentRoot() && filter == NULL) {
        return NULL;
    }

    if (mTempPaint == NULL) {
        CPaint::New((IPaint**)&mTempPaint);
        mTempPaint->SetFilterBitmap(TRUE);
    }
    mTempPaint->SetAlpha(mVPathRenderer->GetRootAlpha());
    mTempPaint->SetColorFilter(filter);
    return mTempPaint;
}

void VectorDrawable::VectorDrawableState::UpdateCachedBitmap(
    /* [in] */ IRect* bounds)
{
    mCachedBitmap->EraseColor(IColor::TRANSPARENT);
    AutoPtr<ICanvas> tmpCanvas;
    CCanvas::New(mCachedBitmap, (ICanvas**)&tmpCanvas);
    Int32 width = 0, height = 0;
    bounds->GetWidth(&width);
    bounds->GetHeight(&height);
    mVPathRenderer->Draw(tmpCanvas, width, height, NULL);
}

void VectorDrawable::VectorDrawableState::CreateCachedBitmapIfNeeded(
    /* [in] */ IRect* bounds)
{
    Int32 width = 0, height = 0;
    bounds->GetWidth(&width);
    bounds->GetHeight(&height);
    if (mCachedBitmap == NULL || !CanReuseBitmap(width, height)) {
        mCachedBitmap = NULL;
        CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&mCachedBitmap);
        mCacheDirty = TRUE;
    }
}

Boolean VectorDrawable::VectorDrawableState::CanReuseBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 w = 0, h = 0;
    mCachedBitmap->GetWidth(&w);
    mCachedBitmap->GetHeight(&h);

    if (width == w && height == h) {
        return TRUE;
    }
    return FALSE;
}

Boolean VectorDrawable::VectorDrawableState::CanReuseCache()
{
    if (!mCacheDirty
            && mCachedThemeAttrs == mThemeAttrs
            && mCachedTint == mTint
            && mCachedTintMode == mTintMode
            && mCachedAutoMirrored == mAutoMirrored
            && mCachedRootAlpha == mVPathRenderer->GetRootAlpha()) {
        return TRUE;
    }
    return FALSE;
}

void VectorDrawable::VectorDrawableState::UpdateCacheStates()
{
    // Use shallow copy here and shallow comparison in canReuseCache(),
    // likely hit cache miss more, but practically not much difference.
    mCachedThemeAttrs = mThemeAttrs;
    mCachedTint = mTint;
    mCachedTintMode = mTintMode;
    mCachedRootAlpha = mVPathRenderer->GetRootAlpha();
    mCachedAutoMirrored = mAutoMirrored;
    mCacheDirty = FALSE;
}

ECode VectorDrawable::VectorDrawableState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = (ConstantState::CanApplyTheme(can), *can) || mThemeAttrs != NULL
            || (mVPathRenderer != NULL && mVPathRenderer->CanApplyTheme());
    return NOERROR;
}

VectorDrawable::VectorDrawableState::VectorDrawableState()
    : mChangingConfigurations(0)
    , mTintMode(DEFAULT_TINT_MODE)
    , mAutoMirrored(FALSE)
    , mCachedTintMode(-1)
    , mCachedRootAlpha(0)
    , mCachedAutoMirrored(FALSE)
    , mCacheDirty(FALSE)
{
    mVPathRenderer = new VPathRenderer();
}

ECode VectorDrawable::VectorDrawableState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CVectorDrawable::New(this, NULL, NULL, drawable);
}

ECode VectorDrawable::VectorDrawableState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CVectorDrawable::New(this, res, NULL, drawable);
}

ECode VectorDrawable::VectorDrawableState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CVectorDrawable::New(this, res, theme, drawable);
}

ECode VectorDrawable::VectorDrawableState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config)
    *config = mChangingConfigurations;
    return NOERROR;
}

//================================================================
// VectorDrawable::VPathRenderer
//================================================================

static AutoPtr<IMatrix> InitDENTITY_MATRIX()
{
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    return matrix;
}

AutoPtr<IMatrix> VectorDrawable::VPathRenderer::IDENTITY_MATRIX = InitDENTITY_MATRIX();

VectorDrawable::VPathRenderer::VPathRenderer()
    : mChangingConfigurations(0)
    , mBaseWidth(0)
    , mBaseHeight(0)
    , mViewportWidth(0)
    , mViewportHeight(0)
    , mRootAlpha(0xFF)
{
    CMatrix::New((IMatrix**)&mFinalPathMatrix);
    CArrayMap::New((IArrayMap**)&mVGTargetsMap);
    AutoPtr<IVGroup> ivgroup;
    CVGroup::New((IVGroup**)&ivgroup);
    mRootGroup = (VGroup*)ivgroup.Get();
    CPath::New((IPath**)&mPath);
    CPath::New((IPath**)&mRenderPath);
}

VectorDrawable::VPathRenderer::VPathRenderer(
    /* [in] */ VPathRenderer* copy)
    : mChangingConfigurations(0)
    , mBaseWidth(0)
    , mBaseHeight(0)
    , mViewportWidth(0)
    , mViewportHeight(0)
    , mRootAlpha(0xFF)
{
    CMatrix::New((IMatrix**)&mFinalPathMatrix);
    CArrayMap::New((IArrayMap**)&mVGTargetsMap);

    AutoPtr<IVGroup> ivgroup;
    CVGroup::New(copy->mRootGroup, mVGTargetsMap, (IVGroup**)&ivgroup);
    mRootGroup = (VGroup*)ivgroup.Get();
    CPath::New(copy->mPath, (IPath**)&mPath);
    CPath::New(copy->mRenderPath, (IPath**)&mRenderPath);
    mBaseWidth = copy->mBaseWidth;
    mBaseHeight = copy->mBaseHeight;
    mViewportWidth = copy->mViewportWidth;
    mViewportHeight = copy->mViewportHeight;
    mChangingConfigurations = copy->mChangingConfigurations;
    mRootAlpha = copy->mRootAlpha;
    mRootName = copy->mRootName;
    if (copy->mRootName != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(copy->mRootName, (ICharSequence**)&cs);
        mVGTargetsMap->Put(cs, (IObject*)this);
    }
}

void VectorDrawable::VPathRenderer::SetRootAlpha(
    /* [in] */ Int32 alpha)
{
    mRootAlpha = alpha;
}

Int32 VectorDrawable::VPathRenderer::GetRootAlpha()
{
    return mRootAlpha;
}

void VectorDrawable::VPathRenderer::SetAlpha(
    /* [in] */ Float alpha)
{
    SetRootAlpha((Int32) (alpha * 255));
}

Float VectorDrawable::VPathRenderer::GetAlpha()
{
    return GetRootAlpha() / 255.0f;
}

Boolean VectorDrawable::VPathRenderer::CanApplyTheme()
{
    // If one of the paths can apply theme, then return TRUE;
    return RecursiveCanApplyTheme(mRootGroup);
}

Boolean VectorDrawable::VPathRenderer::RecursiveCanApplyTheme(
    /* [in] */ VGroup* currentGroup)
{
    // We can do a tree traverse here, if there is one path return TRUE,
    // then we return TRUE for the whole tree.
    AutoPtr<IArrayList> children = currentGroup->mChildren;
    Int32 size = 0;
    IList::Probe(children)->GetSize(&size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> child;
        children->Get(i, (IInterface**)&child);
        if (IVGroup::Probe(child)) {
            AutoPtr<VGroup> childGroup = (VGroup*)IVGroup::Probe(child);
            if (childGroup->CanApplyTheme() || RecursiveCanApplyTheme(childGroup)) {
                return TRUE;
            }
        }
        else if (IVPath::Probe(child)) {
            AutoPtr<VPath> childPath = (VPath*)IVPath::Probe(child);
            if (childPath->CanApplyTheme()) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void VectorDrawable::VPathRenderer::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    // Apply theme to every path of the tree.
    RecursiveApplyTheme(mRootGroup, t);
}

void VectorDrawable::VPathRenderer::RecursiveApplyTheme(
    /* [in] */ VGroup* currentGroup,
    /* [in] */ IResourcesTheme* t)
{
    // We can do a tree traverse here, apply theme to all paths which
    // can apply theme.
    AutoPtr<IArrayList> children = currentGroup->mChildren;
    Int32 size = 0;
    IList::Probe(children)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> child;
        children->Get(i, (IInterface**)&child);
        if (IVGroup::Probe(child)) {
            AutoPtr<VGroup> childGroup = (VGroup*)IVGroup::Probe(child);
            if (childGroup->CanApplyTheme()) {
                childGroup->ApplyTheme(t);
            }
            RecursiveApplyTheme(childGroup, t);
        }
        else if (IVPath::Probe(child)) {
            AutoPtr<VPath> childPath = (VPath*)IVPath::Probe(child);
            if (childPath->CanApplyTheme()) {
                childPath->ApplyTheme(t);
            }
        }
    }
}

void VectorDrawable::VPathRenderer::DrawGroupTree(
    /* [in] */ VGroup* currentGroup,
    /* [in] */ IMatrix* currentMatrix,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IColorFilter* filter)
{
    // Calculate current group's matrix by preConcat the parent's and
    // and the current one on the top of the stack.
    // Basically the Mfinal = Mviewport * M0 * M1 * M2;
    // Mi the local matrix at level i of the group tree.
    currentGroup->mStackedMatrix->Set(currentMatrix);

    Boolean res = FALSE;
    currentGroup->mStackedMatrix->PreConcat(currentGroup->mLocalMatrix, &res);

    // Draw the group tree in the same order as the XML file.
    Int32 size = 0;
    currentGroup->mChildren->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> child;
        currentGroup->mChildren->Get(i, (IInterface**)&child);
        if (IVGroup::Probe(child)) {
            AutoPtr<VGroup> childGroup = (VGroup*)IVGroup::Probe(child);
            DrawGroupTree(childGroup, currentGroup->mStackedMatrix, canvas, w, h, filter);
        }
        else if (IVPath::Probe(child)) {
            AutoPtr<VPath> childPath = (VPath*)IVPath::Probe(child);
            DrawPath(currentGroup, childPath, canvas, w, h, filter);
        }
    }
}

void VectorDrawable::VPathRenderer::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IColorFilter* filter)
{
    // Travese the tree in pre-order to draw.
    DrawGroupTree(mRootGroup, IDENTITY_MATRIX, canvas, w, h, filter);
}

void VectorDrawable::VPathRenderer::DrawPath(
    /* [in] */ VGroup* vGroup,
    /* [in] */ VPath* vPath,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IColorFilter* filter)
{
    Float scaleX = w / mViewportWidth;
    Float scaleY = h / mViewportHeight;
    Float minScale = Elastos::Core::Math::Min(scaleX, scaleY);

    mFinalPathMatrix->Set(vGroup->mStackedMatrix);
    Boolean res = FALSE;
    mFinalPathMatrix->PostScale(scaleX, scaleY, &res);

    vPath->ToPath(mPath);
    AutoPtr<IPath> path = mPath;

    mRenderPath->Reset();

    if (vPath->IsClipPath()) {
        mRenderPath->AddPath(path, mFinalPathMatrix);
        Boolean isNonEmpty = FALSE;
        canvas->ClipPath(mRenderPath, RegionOp_REPLACE, &isNonEmpty);
    }
    else {
        AutoPtr<VFullPath> fullPath = (VFullPath*) vPath;
        if (fullPath->mTrimPathStart != 0.0f || fullPath->mTrimPathEnd != 1.0f) {
            // Float start = (fullPath->mTrimPathStart + fullPath->mTrimPathOffset) % 1.0f;
            // Float end = (fullPath->mTrimPathEnd + fullPath->mTrimPathOffset) % 1.0f;
            Float start = fmod((fullPath->mTrimPathStart + fullPath->mTrimPathOffset), 1.0f);
            Float end = fmod((fullPath->mTrimPathEnd + fullPath->mTrimPathOffset), 1.0f);
            if (mPathMeasure == NULL) {
                CPathMeasure::New((IPathMeasure**)&mPathMeasure);
            }
            mPathMeasure->SetPath(mPath, FALSE);

            Float len = 0;
            mPathMeasure->GetLength(&len);
            start = start * len;
            end = end * len;
            path->Reset();
            Boolean bv = FALSE;
            if (start > end) {
                mPathMeasure->GetSegment(start, len, path, TRUE, &bv);
                mPathMeasure->GetSegment(0.f, end, path, TRUE, &bv);
            }
            else {
                mPathMeasure->GetSegment(start, end, path, TRUE, &bv);
            }
            path->RLineTo(0, 0); // fix bug in measure
        }
        mRenderPath->AddPath(path, mFinalPathMatrix);

        if (fullPath->mFillColor != IColor::TRANSPARENT) {
            if (mFillPaint == NULL) {
                CPaint::New((IPaint**)&mFillPaint);
                mFillPaint->SetStyle(PaintStyle_FILL);
                mFillPaint->SetAntiAlias(TRUE);
            }

            AutoPtr<IPaint> fillPaint = mFillPaint;
            fillPaint->SetColor(ApplyAlpha(fullPath->mFillColor, fullPath->mFillAlpha));
            fillPaint->SetColorFilter(filter);
            canvas->DrawPath(mRenderPath, fillPaint);
        }

        if (fullPath->mStrokeColor != IColor::TRANSPARENT) {
            if (mStrokePaint == NULL) {
                CPaint::New((IPaint**)&mStrokePaint);
                mStrokePaint->SetStyle(PaintStyle_STROKE);
                mStrokePaint->SetAntiAlias(TRUE);
            }

            AutoPtr<IPaint> strokePaint = mStrokePaint;
            if (fullPath->mStrokeLineJoin != -1) {
                strokePaint->SetStrokeJoin(fullPath->mStrokeLineJoin);
            }

            if (fullPath->mStrokeLineCap != -1) {
                strokePaint->SetStrokeCap(fullPath->mStrokeLineCap);
            }

            strokePaint->SetStrokeMiter(fullPath->mStrokeMiterlimit);
            strokePaint->SetColor(ApplyAlpha(fullPath->mStrokeColor, fullPath->mStrokeAlpha));
            strokePaint->SetColorFilter(filter);
            strokePaint->SetStrokeWidth(fullPath->mStrokeWidth * minScale);
            canvas->DrawPath(mRenderPath, strokePaint);
        }
    }
}

//================================================================
// VectorDrawable::VGroup
//================================================================
CAR_INTERFACE_IMPL(VectorDrawable::VGroup, Object, IVGroup);

ECode VectorDrawable::VGroup::constructor(
    /* [in] */ IVGroup* _copy,
    /* [in] */ IArrayMap/*<String, Object>*/* targetsMap)
{
    CMatrix::New((IMatrix**)&mStackedMatrix);
    CArrayList::New((IArrayList**)&mChildren);
    CMatrix::New((IMatrix**)&mLocalMatrix);
    VGroup* copy = (VGroup*)_copy;

    mRotate = copy->mRotate;
    mPivotX = copy->mPivotX;
    mPivotY = copy->mPivotY;
    mScaleX = copy->mScaleX;
    mScaleY = copy->mScaleY;
    mTranslateX = copy->mTranslateX;
    mTranslateY = copy->mTranslateY;
    mThemeAttrs = copy->mThemeAttrs;
    mGroupName = copy->mGroupName;
    mChangingConfigurations = copy->mChangingConfigurations;
    if (mGroupName != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(mGroupName, (ICharSequence**)&cs);
        targetsMap->Put(cs, (IVGroup*)this);
    }

    mLocalMatrix->Set(copy->mLocalMatrix);

    AutoPtr<IArrayList> children = copy->mChildren;
    Int32 size = 0;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> copyChild;
        children->Get(i, (IInterface**)&copyChild);
        if (IVGroup::Probe(copyChild)) {
            AutoPtr<VGroup> copyGroup = (VGroup*)IVGroup::Probe(copyChild);
            AutoPtr<IVGroup> ivgroup;
            CVGroup::New(copyGroup, targetsMap, (IVGroup**)&ivgroup);
            mChildren->Add(ivgroup);
        }
        else {
            AutoPtr<VPath> newPath;
            if (IVFullPath::Probe(copyChild)) {
                AutoPtr<IVFullPath> ivfp;
                CVFullPath::New(IVFullPath::Probe(copyChild), (IVFullPath**)&ivfp);
                newPath = (VFullPath*)ivfp.Get();
            }
            else if (IVClipPath::Probe(copyChild)) {
                AutoPtr<IVClipPath> ivcp;
                CVClipPath::New(IVClipPath::Probe(copyChild), (IVClipPath**)&ivcp);
                newPath = (VClipPath*)ivcp.Get();
            }
            else {
                // throw new IllegalStateException("Unknown object in the tree!");
                // return E_ILLEGAL_STATE_EXCEPTION;
                assert(0);
            }
            mChildren->Add((IVPath*)newPath);
            if (newPath->mPathName != NULL) {
                AutoPtr<ICharSequence> cs;
                CString::New(newPath->mPathName, (ICharSequence**)&cs);
                targetsMap->Put(cs, (IVPath*)newPath);
            }
        }
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::constructor()
{
    mRotate = 0;
    mPivotX = 0;
    mPivotY = 0;
    mScaleX = 1;
    mScaleY = 1;
    mTranslateX = 0;
    mTranslateY = 0;
    mChangingConfigurations = 0;
    CMatrix::New((IMatrix**)&mStackedMatrix);
    CArrayList::New((IArrayList**)&mChildren);
    CMatrix::New((IMatrix**)&mLocalMatrix);
    return NOERROR;
}

String VectorDrawable::VGroup::GetGroupName()
{
    return mGroupName;
}

AutoPtr<IMatrix> VectorDrawable::VGroup::GetLocalMatrix()
{
    return mLocalMatrix;
}

void VectorDrawable::VGroup::Inflate(
    /* [in] */ IResources* res,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawableGroup);
    ObtainAttributes(res, theme, attrs, layout, (ITypedArray**)&a);
    UpdateStateFromTypedArray(a);
    a->Recycle();
}

void VectorDrawable::VGroup::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&mThemeAttrs);
    a->GetFloat(R::styleable::VectorDrawableGroup_rotation, mRotate, &mRotate);
    a->GetFloat(R::styleable::VectorDrawableGroup_pivotX, mPivotX, &mPivotX);
    a->GetFloat(R::styleable::VectorDrawableGroup_pivotY, mPivotY, &mPivotY);
    a->GetFloat(R::styleable::VectorDrawableGroup_scaleX, mScaleX, &mScaleX);
    a->GetFloat(R::styleable::VectorDrawableGroup_scaleY, mScaleY, &mScaleY);
    a->GetFloat(R::styleable::VectorDrawableGroup_translateX, mTranslateX, &mTranslateX);
    a->GetFloat(R::styleable::VectorDrawableGroup_translateY, mTranslateY, &mTranslateY);

    String groupName;
    a->GetString(R::styleable::VectorDrawableGroup_name, &groupName);
    if (groupName != NULL) {
        mGroupName = groupName;
    }

    UpdateLocalMatrix();
}

Boolean VectorDrawable::VGroup::CanApplyTheme()
{
    return mThemeAttrs != NULL;
}

void VectorDrawable::VGroup::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    if (mThemeAttrs == NULL) {
        return;
    }

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawableGroup);
    ((CResources::Theme*)t)->ResolveAttribute(mThemeAttrs, layout, (ITypedArray**)&a);
    UpdateStateFromTypedArray(a);
    a->Recycle();
}

void VectorDrawable::VGroup::UpdateLocalMatrix()
{
    // The order we apply is the same as the
    // RenderNode.cpp::applyViewPropertyTransforms().
    mLocalMatrix->Reset();
    Boolean res = FALSE;
    mLocalMatrix->PostTranslate(-mPivotX, -mPivotY, &res);
    mLocalMatrix->PostScale(mScaleX, mScaleY, &res);
    mLocalMatrix->PostRotate(mRotate, 0, 0, &res);
    mLocalMatrix->PostTranslate(mTranslateX + mPivotX, mTranslateY + mPivotY, &res);
}

ECode VectorDrawable::VGroup::GetRotation(
    /* [out] */ Float* rotation)
{
    VALIDATE_NOT_NULL(rotation);
    *rotation = mRotate;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetRotation(
    /* [in] */ Float rotation)
{
    if (rotation != mRotate) {
        mRotate = rotation;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetPivotX(
    /* [out] */ Float* pivotX)
{
    VALIDATE_NOT_NULL(pivotX);
    *pivotX = mPivotX;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetPivotX(
    /* [in] */ Float pivotX)
{
    if (pivotX != mPivotX) {
        mPivotX = pivotX;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetPivotY(
    /* [out] */ Float* pivotY)
{
    VALIDATE_NOT_NULL(pivotY);
    *pivotY = mPivotY;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetPivotY(
    /* [in] */ Float pivotY)
{
    if (pivotY != mPivotY) {
        mPivotY = pivotY;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetScaleX(
    /* [out] */ Float* scaleX)
{
    VALIDATE_NOT_NULL(scaleX);
    *scaleX = mScaleX;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetScaleX(
    /* [in] */ Float scaleX)
{
    if (scaleX != mScaleX) {
        mScaleX = scaleX;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetScaleY(
    /* [out] */ Float* scaleY)
{
    VALIDATE_NOT_NULL(scaleY);
    *scaleY = mScaleY;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetScaleY(
    /* [in] */ Float scaleY)
{
    if (scaleY != mScaleY) {
        mScaleY = scaleY;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetTranslateX(
    /* [out] */ Float* translateX)
{
    VALIDATE_NOT_NULL(translateX);
    *translateX = mTranslateX;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetTranslateX(
    /* [in] */ Float translateX)
{
    if (translateX != mTranslateX) {
        mTranslateX = translateX;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

ECode VectorDrawable::VGroup::GetTranslateY(
    /* [out] */ Float* translateY)
{
    VALIDATE_NOT_NULL(translateY);
    *translateY = mTranslateY;
    return NOERROR;
}

ECode VectorDrawable::VGroup::SetTranslateY(
    /* [in] */ Float translateY)
{
    if (translateY != mTranslateY) {
        mTranslateY = translateY;
        UpdateLocalMatrix();
    }
    return NOERROR;
}

//================================================================
// VectorDrawable::VPath
//================================================================
CAR_INTERFACE_IMPL(VectorDrawable::VPath, Object, IVPath)

VectorDrawable::VPath::VPath()
    : mChangingConfigurations(0)
{
    // Empty constructor.
}

ECode VectorDrawable::VPath::constructor(
    /* [in] */ VPath* copy)
{
    mChangingConfigurations = 0;
    mPathName = copy->mPathName;
    mChangingConfigurations = copy->mChangingConfigurations;
    mNodes = PathParser::DeepCopyNodes(copy->mNodes);
    return NOERROR;
}

void VectorDrawable::VPath::ToPath(
    /* [in] */ IPath* path)
{
    path->Reset();
    if (mNodes != NULL) {
        PathDataNode::NodesToPath(mNodes, path);
    }
}

String VectorDrawable::VPath::GetPathName()
{
    return mPathName;
}

Boolean VectorDrawable::VPath::CanApplyTheme()
{
    return FALSE;
}

void VectorDrawable::VPath::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
}

Boolean VectorDrawable::VPath::IsClipPath()
{
    return FALSE;
}

AutoPtr<ArrayOf<PathDataNode*> > VectorDrawable::VPath::GetPathData()
{
    return mNodes;
}

void VectorDrawable::VPath::SetPathData(
    /* [in] */ ArrayOf<PathDataNode*>* nodes)
{
    if (!PathParser::CanMorph(mNodes, nodes)) {
        // This should not happen in the middle of animation.
        mNodes = PathParser::DeepCopyNodes(nodes);
    }
    else {
        PathParser::UpdateNodes(mNodes, nodes);
    }
}

//================================================================
// VectorDrawable::VClipPath
//================================================================
CAR_INTERFACE_IMPL(VectorDrawable::VClipPath, VPath, IVClipPath)

ECode VectorDrawable::VClipPath::constructor()
{
    // Empty constructor.
    return NOERROR;
}

ECode VectorDrawable::VClipPath::constructor(
    /* [in] */ IVClipPath* copy)
{
    VPath::constructor((VClipPath*)copy);
    return NOERROR;
}

void VectorDrawable::VClipPath::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawableClipPath);
    ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
    UpdateStateFromTypedArray(a);
    a->Recycle();
}

void VectorDrawable::VClipPath::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    mChangingConfigurations |= config;

    String pathName;
    a->GetString(R::styleable::VectorDrawableClipPath_name, &pathName);
    if (pathName != NULL) {
        mPathName = pathName;
    }

    String pathData;
    a->GetString(R::styleable::VectorDrawableClipPath_pathData, &pathData);
    if (pathData != NULL) {
        mNodes = PathParser::CreateNodesFromPathData(pathData);
    }
}

Boolean VectorDrawable::VClipPath::IsClipPath()
{
    return TRUE;
}

//================================================================
// VectorDrawable::VFullPath
//================================================================
CAR_INTERFACE_IMPL(VectorDrawable::VFullPath, VPath, IVFullPath)

VectorDrawable::VFullPath::VFullPath()
    : mStrokeColor(IColor::TRANSPARENT)
    , mStrokeWidth(0)
    , mFillColor(IColor::TRANSPARENT)
    , mStrokeAlpha(1.0f)
    , mFillRule(0)
    , mFillAlpha(1.0f)
    , mTrimPathStart(0)
    , mTrimPathEnd(1)
    , mTrimPathOffset(0)
    , mStrokeLineCap(PaintCap_BUTT)
    , mStrokeLineJoin(PaintJoin_MITER)
    , mStrokeMiterlimit(4)
{}

ECode VectorDrawable::VFullPath::constructor()
{
    // Empty constructor.
    return NOERROR;
}

ECode VectorDrawable::VFullPath::constructor(
    /* [in] */ IVFullPath* _copy)
{
    VFullPath* copy = (VFullPath*)_copy;
    VPath::constructor(copy);
    mThemeAttrs = copy->mThemeAttrs;

    mStrokeColor = copy->mStrokeColor;
    mStrokeWidth = copy->mStrokeWidth;
    mStrokeAlpha = copy->mStrokeAlpha;
    mFillColor = copy->mFillColor;
    mFillRule = copy->mFillRule;
    mFillAlpha = copy->mFillAlpha;
    mTrimPathStart = copy->mTrimPathStart;
    mTrimPathEnd = copy->mTrimPathEnd;
    mTrimPathOffset = copy->mTrimPathOffset;

    mStrokeLineCap = copy->mStrokeLineCap;
    mStrokeLineJoin = copy->mStrokeLineJoin;
    mStrokeMiterlimit = copy->mStrokeMiterlimit;
    return NOERROR;
}

PaintCap VectorDrawable::VFullPath::GetStrokeLineCap(
    /* [in] */ Int32 id,
    /* [in] */ PaintCap defValue)
{
    switch (id) {
        case VectorDrawable::LINECAP_BUTT:
            return PaintCap_BUTT;
        case VectorDrawable::LINECAP_ROUND:
            return PaintCap_ROUND;
        case VectorDrawable::LINECAP_SQUARE:
            return PaintCap_SQUARE;
        default:
            return defValue;
    }
}

PaintJoin VectorDrawable::VFullPath::GetStrokeLineJoin(
    /* [in] */ Int32 id,
    /* [in] */ PaintJoin defValue)
{
    switch (id) {
        case LINEJOIN_MITER:
            return PaintJoin_MITER;
        case LINEJOIN_ROUND:
            return PaintJoin_ROUND;
        case LINEJOIN_BEVEL:
            return PaintJoin_BEVEL;
        default:
            return defValue;
    }
}

Boolean VectorDrawable::VFullPath::CanApplyTheme()
{
    return mThemeAttrs != NULL;
}

void VectorDrawable::VFullPath::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawablePath);
    ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);

    UpdateStateFromTypedArray(a);
    a->Recycle();
}

void VectorDrawable::VFullPath::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&mThemeAttrs);

    String pathName;
    a->GetString(R::styleable::VectorDrawablePath_name, &pathName);
    if (pathName != NULL) {
        mPathName = pathName;
    }

    String pathData;
    a->GetString(R::styleable::VectorDrawablePath_pathData, &pathData);
    if (pathData != NULL) {
        mNodes = PathParser::CreateNodesFromPathData(pathData);
    }

    a->GetColor(R::styleable::VectorDrawablePath_fillColor, mFillColor, &mFillColor);
    a->GetFloat(R::styleable::VectorDrawablePath_fillAlpha, mFillAlpha, &mFillAlpha);
    Int32 value = 0;
    a->GetInt32(R::styleable::VectorDrawablePath_strokeLineCap, -1, &value);
    mStrokeLineCap = GetStrokeLineCap(value, mStrokeLineCap);

    a->GetInt32(R::styleable::VectorDrawablePath_strokeLineJoin, -1, &value);
    mStrokeLineJoin = GetStrokeLineJoin(value, mStrokeLineJoin);
    a->GetFloat(R::styleable::VectorDrawablePath_strokeMiterLimit, mStrokeMiterlimit, &mStrokeMiterlimit);
    a->GetColor(R::styleable::VectorDrawablePath_strokeColor, mStrokeColor, &mStrokeColor);
    a->GetFloat(R::styleable::VectorDrawablePath_strokeAlpha, mStrokeAlpha, &mStrokeAlpha);
    a->GetFloat(R::styleable::VectorDrawablePath_strokeWidth, mStrokeWidth, &mStrokeWidth);
    a->GetFloat(R::styleable::VectorDrawablePath_trimPathEnd, mTrimPathEnd, &mTrimPathEnd);
    a->GetFloat(R::styleable::VectorDrawablePath_trimPathOffset, mTrimPathOffset, &mTrimPathOffset);
    a->GetFloat(R::styleable::VectorDrawablePath_trimPathStart, mTrimPathStart, &mTrimPathStart);
}

void VectorDrawable::VFullPath::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    if (mThemeAttrs == NULL) {
        return;
    }

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawablePath);
    ((CResources::Theme*)t)->ResolveAttribute(mThemeAttrs, layout, (ITypedArray**)&a);
    UpdateStateFromTypedArray(a);
    a->Recycle();
}

ECode VectorDrawable::VFullPath::GetStrokeColor(
    /* [out] */ Int32* strokeColor)
{
    VALIDATE_NOT_NULL(strokeColor);
    *strokeColor = mStrokeColor;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetStrokeColor(
    /* [in] */ Int32 strokeColor)
{
    mStrokeColor = strokeColor;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetStrokeWidth(
    /* [out] */ Float* strokeWidth)
{
    VALIDATE_NOT_NULL(strokeWidth);
    *strokeWidth = mStrokeWidth;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetStrokeWidth(
    /* [in] */ Float strokeWidth)
{
    mStrokeWidth = strokeWidth;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetStrokeAlpha(
    /* [out] */ Float* strokeAlpha)
{
    VALIDATE_NOT_NULL(strokeAlpha);
    *strokeAlpha = mStrokeAlpha;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetStrokeAlpha(
    /* [in] */ Float strokeAlpha)
{
    mStrokeAlpha = strokeAlpha;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetFillColor(
    /* [out] */ Int32* fillColor)
{
    VALIDATE_NOT_NULL(fillColor);
    *fillColor = mFillColor;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetFillColor(
    /* [in] */ Int32 fillColor)
{
    mFillColor = fillColor;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetFillAlpha(
    /* [out] */ Float* fillAlpha)
{
    VALIDATE_NOT_NULL(fillAlpha);
    *fillAlpha = mFillAlpha;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetFillAlpha(
    /* [in] */ Float fillAlpha)
{
    mFillAlpha = fillAlpha;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetTrimPathStart(
    /* [out] */ Float* trimPathStart)
{
    VALIDATE_NOT_NULL(trimPathStart);
    *trimPathStart = mTrimPathStart;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetTrimPathStart(
    /* [in] */ Float trimPathStart)
{
    mTrimPathStart = trimPathStart;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetTrimPathEnd(
    /* [out] */ Float* trimPathEnd)
{
    VALIDATE_NOT_NULL(trimPathEnd);
    *trimPathEnd = mTrimPathEnd;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetTrimPathEnd(
    /* [in] */ Float trimPathEnd)
{
    mTrimPathEnd = trimPathEnd;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::GetTrimPathOffset(
    /* [out] */ Float* trimPathOffset)
{
    VALIDATE_NOT_NULL(trimPathOffset);
    *trimPathOffset = mTrimPathOffset;
    return NOERROR;
}

ECode VectorDrawable::VFullPath::SetTrimPathOffset(
    /* [in] */ Float trimPathOffset)
{
    mTrimPathOffset = trimPathOffset;
    return NOERROR;
}

//================================================================
// VectorDrawable
//================================================================
CAR_INTERFACE_IMPL(VectorDrawable, Drawable, IVectorDrawable)

VectorDrawable::VectorDrawable()
    : mMutated(FALSE)
    , mAllowCaching(TRUE)
{
}

ECode VectorDrawable::constructor()
{
    mVectorState = new VectorDrawableState();
    return NOERROR;
}

ECode VectorDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    VectorDrawableState* vds = (VectorDrawableState*)state;
    Boolean can = FALSE;
    if (theme != NULL && (vds->CanApplyTheme(&can), can)) {
        // If we need to apply a theme, implicitly mutate.
        mVectorState = new VectorDrawableState(vds);
        ApplyTheme(theme);
    }
    else {
        mVectorState = vds;
    }

    AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
    mTintFilter = UpdateTintFilter(temp, vds->mTint, vds->mTintMode);
    return NOERROR;
}

ECode VectorDrawable::Mutate()
{
    if (!mMutated) {
        mVectorState = new VectorDrawableState(mVectorState);
        mMutated = TRUE;
    }

    return NOERROR;
}

AutoPtr<IInterface> VectorDrawable::GetTargetByName(
    /* [in] */ const String& name)
{
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    mVectorState->mVPathRenderer->mVGTargetsMap->Get(cs, (IInterface**)&obj);
    return obj;
}

ECode VectorDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    GetChangingConfigurations(&mVectorState->mChangingConfigurations);
    *state = mVectorState;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode VectorDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 w = 0, h = 0;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    if (w == 0 || h == 0) {
        // too small to draw
        return NOERROR;
    }

    Int32 saveCount = 0;
    canvas->Save(&saveCount);
    Boolean needMirroring = NeedMirroring();

    Int32 left = 0, top = 0;
    bounds->GetLeft(&left);
    bounds->GetTop(&top);
    canvas->Translate(left, top);
    if (needMirroring) {
        canvas->Translate(w, 0);
        canvas->Scale(-1.0f, 1.0f);
    }

    // Color filters always override tint filters.
    AutoPtr<IColorFilter> colorFilter = mColorFilter == NULL ? IColorFilter::Probe(mTintFilter) : mColorFilter.Get();
    if (!mAllowCaching) {
        // AnimatedVectorDrawable
        if (!mVectorState->HasTranslucentRoot()) {
            mVectorState->mVPathRenderer->Draw(canvas, w, h, colorFilter);
        } else {
            mVectorState->CreateCachedBitmapIfNeeded(bounds);
            mVectorState->UpdateCachedBitmap(bounds);
            mVectorState->DrawCachedBitmapWithRootAlpha(canvas, colorFilter);
        }
    }
    else {
        // Static Vector Drawable case.
        mVectorState->CreateCachedBitmapIfNeeded(bounds);
        if (!mVectorState->CanReuseCache()) {
            mVectorState->UpdateCachedBitmap(bounds);
            mVectorState->UpdateCacheStates();
        }
        mVectorState->DrawCachedBitmapWithRootAlpha(canvas, colorFilter);
    }
    return canvas->RestoreToCount(saveCount);
}

ECode VectorDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mVectorState->mVPathRenderer->GetRootAlpha();
    return NOERROR;
}

ECode VectorDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    if (mVectorState->mVPathRenderer->GetRootAlpha() != alpha) {
        mVectorState->mVPathRenderer->SetRootAlpha(alpha);
        InvalidateSelf();
    }
    return NOERROR;
}

ECode VectorDrawable::SetColorFilter(
    /* [in] */ IColorFilter* colorFilter)
{
    mColorFilter = colorFilter;
    return InvalidateSelf();
}

ECode VectorDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    if (state->mTint.Get() != tint) {
        state->mTint = tint;
        AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
        mTintFilter = UpdateTintFilter(temp, tint, state->mTintMode);
        InvalidateSelf();
    }
    return NOERROR;
}

ECode VectorDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    if (state->mTintMode != tintMode) {
        state->mTintMode = tintMode;
        AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
        mTintFilter = UpdateTintFilter(temp, state->mTint, tintMode);
        InvalidateSelf();
    }
    return NOERROR;
}

ECode VectorDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    Boolean b1, b2;
    *isStateful = (Drawable::IsStateful(&b1), b1)
        || (mVectorState != NULL && mVectorState->mTint != NULL && (mVectorState->mTint->IsStateful(&b2), b2));
    return NOERROR;
}

Boolean VectorDrawable::OnStateChange(
    /* [in] */ ArrayOf<Int32>* stateSet)
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    if (state->mTint.Get() != NULL && state->mTintMode != -1) {
        AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
        mTintFilter = UpdateTintFilter(temp, state->mTint, state->mTintMode);
        InvalidateSelf();
        return TRUE;
    }
    return FALSE;
}

ECode VectorDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    return IPixelFormat::TRANSLUCENT;
}

ECode VectorDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = (Int32) mVectorState->mVPathRenderer->mBaseWidth;
    return NOERROR;
}

ECode VectorDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = (Int32) mVectorState->mVPathRenderer->mBaseHeight;
    return NOERROR;
}

ECode VectorDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    Boolean tmp = FALSE;
    *can = (Drawable::CanApplyTheme(&tmp), tmp) || (mVectorState != NULL && (mVectorState->CanApplyTheme(&tmp), tmp));
    return NOERROR;
}

ECode VectorDrawable::ApplyTheme(
    /* [out] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<VectorDrawableState> state = mVectorState;
    if (state.Get() != NULL && state->mThemeAttrs.Get() != NULL) {
        AutoPtr<ITypedArray> a;
        AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawable);
        ((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a);

        // try {
        state->mCacheDirty = TRUE;
        if (FAILED(UpdateStateFromTypedArray(a))) {
            a->Recycle();
            return E_RUNTIME_EXCEPTION;
        }

        // } catch (XmlPullParserException e) {
        //     throw new RuntimeException(e);
        // } finally {
        a->Recycle();
        // }

        AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
        mTintFilter = UpdateTintFilter(temp, state->mTint, state->mTintMode);
    }

    AutoPtr<VPathRenderer> path = state->mVPathRenderer;
    if (path != NULL && path->CanApplyTheme()) {
        path->ApplyTheme(t);
    }
    return NOERROR;
}

ECode VectorDrawable::GetPixelSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);
    if ((mVectorState == NULL && mVectorState->mVPathRenderer == NULL) ||
            mVectorState->mVPathRenderer->mBaseWidth == 0 ||
            mVectorState->mVPathRenderer->mBaseHeight == 0 ||
            mVectorState->mVPathRenderer->mViewportHeight == 0 ||
            mVectorState->mVPathRenderer->mViewportWidth == 0)
    {
        *size = 1; // fall back to 1:1 pixel mapping.
        return NOERROR;
    }
    Float intrinsicWidth = mVectorState->mVPathRenderer->mBaseWidth;
    Float intrinsicHeight = mVectorState->mVPathRenderer->mBaseHeight;
    Float viewportWidth = mVectorState->mVPathRenderer->mViewportWidth;
    Float viewportHeight = mVectorState->mVPathRenderer->mViewportHeight;
    Float scaleX = viewportWidth / intrinsicWidth;
    Float scaleY = viewportHeight / intrinsicHeight;
    *size = Elastos::Core::Math::Min(scaleX, scaleY);
    return NOERROR;
}

AutoPtr<IVectorDrawable> VectorDrawable::Create(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 rid)
{
    // try {
    AutoPtr<IXmlResourceParser> obj;
    if (FAILED(resources->GetXml(rid, (IXmlResourceParser**)&obj))) {
        return NULL;
    }
    IXmlPullParser* parser = IXmlPullParser::Probe(obj);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
    Int32 type = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
        && type != IXmlPullParser::END_DOCUMENT) {
        // Empty loop
    }
    if (type != IXmlPullParser::START_TAG) {
        Logger::E(LOGTAG, "No start tag found");
        // throw new XmlPullParserException("No start tag found");
        assert(0);
    }

    AutoPtr<IVectorDrawable> drawable;
    CVectorDrawable::New((IVectorDrawable**)&drawable);
    if (FAILED(IDrawable::Probe(drawable)->Inflate(resources, parser, attrs))) {
        return NULL;
    }

    return drawable;
}

Int32 VectorDrawable::ApplyAlpha(
    /* [in] */ Int32 color,
    /* [in] */ Float alpha)
{
    Int32 alphaBytes = Color::Alpha(color);
    color &= 0x00FFFFFF;
    color |= ((Int32)(alphaBytes * alpha) << 24);
    return color;
}

ECode VectorDrawable::Inflate(
    /* [in] */ IResources* res,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    AutoPtr<VPathRenderer> pathRenderer = new VPathRenderer();
    state->mVPathRenderer = pathRenderer;

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::VectorDrawable);
    ECode ec = ObtainAttributes(res, theme, attrs, layout, (ITypedArray**)&a);
    if (FAILED(ec)) {
        return ec;
    }

    ec = UpdateStateFromTypedArray(a);
    a->Recycle();
    if (FAILED(ec)) {
        return ec;
    }

    state->mCacheDirty = TRUE;
    FAIL_RETURN(InflateInternal(res, parser, attrs, theme));

    AutoPtr<IPorterDuffColorFilter> temp = mTintFilter;
    mTintFilter = UpdateTintFilter(temp, state->mTint, state->mTintMode);
    return NOERROR;
}

ECode VectorDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    AutoPtr<VPathRenderer> pathRenderer = state->mVPathRenderer;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    state->mThemeAttrs = NULL;
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs);

    Int32 tintMode = -1;
    a->GetInt32(R::styleable::VectorDrawable_tintMode, -1, &tintMode);
    if (tintMode != -1) {
        FAIL_RETURN(Drawable::ParseTintMode(tintMode, PorterDuffMode_SRC_IN, &state->mTintMode));
    }

    AutoPtr<IColorStateList> tint;
    a->GetColorStateList(R::styleable::VectorDrawable_tint, (IColorStateList**)&tint);
    if (tint != NULL) {
        state->mTint = tint;
    }

    a->GetBoolean(R::styleable::VectorDrawable_autoMirrored, state->mAutoMirrored, &state->mAutoMirrored);
    a->GetFloat(R::styleable::VectorDrawable_viewportWidth, pathRenderer->mViewportWidth, &pathRenderer->mViewportWidth);
    a->GetFloat(R::styleable::VectorDrawable_viewportHeight, pathRenderer->mViewportHeight, &pathRenderer->mViewportHeight);

    if (pathRenderer->mViewportWidth <= 0) {
        String des;
        a->GetPositionDescription(&des);
        Logger::E(LOGTAG, "%s <vector> tag requires viewportWidth > 0", des.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    else if (pathRenderer->mViewportHeight <= 0) {
        String des;
        a->GetPositionDescription(&des);
        Logger::E(LOGTAG, "%s <vector> tag requires viewportHeight > 0", des.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    a->GetDimension(R::styleable::VectorDrawable_width, pathRenderer->mBaseWidth, &pathRenderer->mBaseWidth);
    a->GetDimension(R::styleable::VectorDrawable_height, pathRenderer->mBaseHeight, &pathRenderer->mBaseHeight);

    if (pathRenderer->mBaseWidth <= 0) {
        String des;
        a->GetPositionDescription(&des);
        Logger::E(LOGTAG, "%s <vector> tag requires width > 0", des.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    else if (pathRenderer->mBaseHeight <= 0) {
        String des;
        a->GetPositionDescription(&des);
        Logger::E(LOGTAG, "%s <vector> tag requires height > 0", des.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    Float alphaInFloat = 0;
    a->GetFloat(R::styleable::VectorDrawable_alpha, pathRenderer->GetAlpha(), &alphaInFloat);
    pathRenderer->SetAlpha(alphaInFloat);

    String name;
    a->GetString(R::styleable::VectorDrawable_name, &name);
    if (name != NULL) {
        pathRenderer->mRootName = name;
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        pathRenderer->mVGTargetsMap->Put(cs, (IObject*)pathRenderer);
    }

    return NOERROR;
}

ECode VectorDrawable::InflateInternal(
    /* [in] */ IResources* res,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    AutoPtr<VectorDrawableState> state = mVectorState;
    AutoPtr<VPathRenderer> pathRenderer = state->mVPathRenderer;
    Boolean noPathTag = TRUE;

    // Use a stack to help to build the group tree.
    // The top of the stack is always the current group.
    AutoPtr<IStack> groupStack;
    CStack::New((IStack**)&groupStack);
    groupStack->Push((IVGroup*)pathRenderer->mRootGroup);

    Int32 eventType = 0;
    parser->GetEventType(&eventType);
    while (eventType != IXmlPullParser::END_DOCUMENT) {
        if (eventType == IXmlPullParser::START_TAG) {
            String tagName;
            parser->GetName(&tagName);
            AutoPtr<IInterface> currentGroupObj;
            groupStack->Peek((IInterface**)&currentGroupObj);
            AutoPtr<VGroup> currentGroup = (VGroup*)IVGroup::Probe(currentGroupObj);

            if (SHAPE_PATH.Equals(tagName)) {
                AutoPtr<IVFullPath> ivfp;
                CVFullPath::New((IVFullPath**)&ivfp);
                AutoPtr<VFullPath> path = (VFullPath*)ivfp.Get();
                path->Inflate(res, attrs, theme);
                currentGroup->mChildren->Add((IVFullPath*)path);
                if (path->GetPathName() != NULL) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(path->GetPathName(), (ICharSequence**)&cs);
                    pathRenderer->mVGTargetsMap->Put(cs, (IVFullPath*)path);
                }
                noPathTag = FALSE;
                state->mChangingConfigurations |= path->mChangingConfigurations;
            }
            else if (SHAPE_CLIP_PATH.Equals(tagName)) {
                AutoPtr<IVClipPath> ivcp;
                CVClipPath::New((IVClipPath**)&ivcp);
                AutoPtr<VClipPath> path = (VClipPath*)ivcp.Get();
                path->Inflate(res, attrs, theme);
                currentGroup->mChildren->Add((IVClipPath*)path);
                if (path->GetPathName() != NULL) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(path->GetPathName(), (ICharSequence**)&cs);
                    pathRenderer->mVGTargetsMap->Put(cs, (IVClipPath*)path);
                }
                state->mChangingConfigurations |= path->mChangingConfigurations;
            }
            else if (SHAPE_GROUP.Equals(tagName)) {
                AutoPtr<IVGroup> ivgroup;
                CVGroup::New((IVGroup**)&ivgroup);
                AutoPtr<VGroup> newChildGroup = (VGroup*)ivgroup.Get();
                newChildGroup->Inflate(res, attrs, theme);
                currentGroup->mChildren->Add((IVGroup*)newChildGroup);
                groupStack->Push((IVGroup*)newChildGroup);
                if (newChildGroup->GetGroupName() != NULL) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(newChildGroup->GetGroupName(), (ICharSequence**)&cs);
                    pathRenderer->mVGTargetsMap->Put(cs, (IVGroup*)newChildGroup);
                }
                state->mChangingConfigurations |= newChildGroup->mChangingConfigurations;
            }
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String tagName;
            parser->GetName(&tagName);
            if (SHAPE_GROUP.Equals(tagName)) {
                AutoPtr<IInterface> obj;
                groupStack->Pop((IInterface**)&obj);
            }
        }
        parser->Next(&eventType);
    }

    // Print the tree out for debug.
    if (DBG_VECTOR_DRAWABLE) {
        PrintGroupTree(pathRenderer->mRootGroup, 0);
    }

    if (noPathTag) {
        StringBuilder sb;
        if (sb.GetLength() > 0) {
            sb.Append(" or ");
        }
        sb.Append(SHAPE_PATH);
        Logger::E(LOGTAG, "no %s defined.", sb.ToString().string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

void VectorDrawable::PrintGroupTree(
    /* [in] */ VGroup* currentGroup,
    /* [in] */ Int32 level)
{
    StringBuilder sb("");
    String indent = String("");
    for (Int32 i = 0; i < level; i++) {
        sb += "    ";
    }
    // Print the current node
    // Logger::V(LOGTAG, "%s current group is :%s, rotation is %d",
    //     sb.ToString().string(), currentGroup->GetGroupName().string(), currentGroup->mRotate);

    AutoPtr<IMatrix> matrix = currentGroup->GetLocalMatrix();
    // Logger::V(LOGTAG, "%s matrix is :%s", sb.ToString().string(), TO_CSTR(matrix));
    // Then print all the children groups
    Int32 size = 0;
    currentGroup->mChildren->GetSize(&size);
    IVGroup* vg;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> child;
        currentGroup->mChildren->Get(i, (IInterface**)&child);
        vg = IVGroup::Probe(child);
        if (vg) {
            PrintGroupTree((VGroup*)vg, level + 1);
        }
    }
}

ECode VectorDrawable::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = (Drawable::GetChangingConfigurations(config), *config) | mVectorState->mChangingConfigurations;
    return NOERROR;
}

void VectorDrawable::SetAllowCaching(
    /* [in] */ Boolean allowCaching)
{
    mAllowCaching = allowCaching;
}

Boolean VectorDrawable::NeedMirroring()
{
    Boolean bv = FALSE;
    Int32 dir = 0;
    return (IsAutoMirrored(&bv), bv) && (GetLayoutDirection(&dir), dir) == ILayoutDirection::RTL;
}

ECode VectorDrawable::SetAutoMirrored(
    /* [in] */ Boolean mirrored)
{
    if (mVectorState->mAutoMirrored != mirrored) {
        mVectorState->mAutoMirrored = mirrored;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode VectorDrawable::IsAutoMirrored(
    /* [out] */ Boolean* mirrored)
{
    VALIDATE_NOT_NULL(mirrored);
    *mirrored = mVectorState->mAutoMirrored;
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
