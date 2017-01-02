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
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/LayerDrawable.h"
#include "elastos/droid/graphics/drawable/CLayerDrawable.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::View::IView;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

//=====================================================================
// ChildDrawable
//=====================================================================
ChildDrawable::ChildDrawable()
    : mInsetL(0)
    , mInsetT(0)
    , mInsetR(0)
    , mInsetB(0)
    , mId(IView::NO_ID)
{}

ChildDrawable::ChildDrawable(
    /* [in] */ ChildDrawable* orig,
    /* [in] */ LayerDrawable* owner,
    /* [in] */ IResources* res)
    : mThemeAttrs(orig->mThemeAttrs)
    , mInsetL(orig->mInsetL)
    , mInsetT(orig->mInsetT)
    , mInsetR(orig->mInsetR)
    , mInsetB(orig->mInsetB)
    , mId(orig->mId)
{
    if (res != NULL) {
        AutoPtr<IDrawableConstantState> state;
        orig->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        state->NewDrawable(res, (IDrawable**)&mDrawable);
    }
    else {
        AutoPtr<IDrawableConstantState> state;
        orig->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        state->NewDrawable((IDrawable**)&mDrawable);
    }
    mDrawable->SetCallback(owner);
    Int32 dir = 0;
    orig->mDrawable->GetLayoutDirection(&dir);
    mDrawable->SetLayoutDirection(dir);
    AutoPtr<IRect> bounds;
    orig->mDrawable->GetBounds((IRect**)&bounds);
    mDrawable->SetBounds(bounds);
    Int32 level = 0;
    orig->mDrawable->GetLevel(&level);
    Boolean changed = FALSE;
    mDrawable->SetLevel(level, &changed);
}


//=====================================================================
// LayerDrawable::LayerState
//=====================================================================
LayerDrawable::LayerState::LayerState(
    /* [in] */ LayerState* orig,
    /* [in] */ LayerDrawable* owner,
    /* [in] */ IResources* res)
    : mNum(0)
    , mChangingConfigurations(0)
    , mChildrenChangingConfigurations(0)
    , mHaveOpacity(FALSE)
    , mOpacity(0)
    , mHaveIsStateful(FALSE)
    , mIsStateful(FALSE)
    , mAutoMirrored(FALSE)
    , mPaddingMode(PADDING_MODE_NEST)
{
    if (orig != NULL) {
        AutoPtr<ArrayOf<ChildDrawable*> > origChildDrawable = orig->mChildren;
        Int32 N = orig->mNum;

        mNum = N;
        mChildren = ArrayOf<ChildDrawable*>::Alloc(N);

        mChangingConfigurations = orig->mChangingConfigurations;
        mChildrenChangingConfigurations = orig->mChildrenChangingConfigurations;

        for (Int32 i = 0; i < N; i++) {
            ChildDrawable* origcd = (*origChildDrawable)[i];
            AutoPtr<ChildDrawable> cd = new ChildDrawable(origcd, owner, res);
            mChildren->Set(i, cd);
        }

        mHaveOpacity = orig->mHaveOpacity;
        mOpacity = orig->mOpacity;
        mHaveIsStateful = orig->mHaveIsStateful;
        mIsStateful = orig->mIsStateful;
        mAutoMirrored = orig->mAutoMirrored;
        mPaddingMode = orig->mPaddingMode;
        mThemeAttrs = orig->mThemeAttrs;
    }
    else {
        mNum = 0;
        mChildren = NULL;
    }
}

LayerDrawable::LayerState::~LayerState()
{}

ECode LayerDrawable::LayerState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = mThemeAttrs != NULL;
    return NOERROR;
}

ECode LayerDrawable::LayerState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CLayerDrawable::New(this, NULL, NULL, drawable);
}

ECode LayerDrawable::LayerState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CLayerDrawable::New(this, res, NULL, drawable);
}

ECode LayerDrawable::LayerState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CLayerDrawable::New(this, res, theme, drawable);
}

ECode LayerDrawable::LayerState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

Int32 LayerDrawable::LayerState::GetOpacity()
{
    if (mHaveOpacity) {
        return mOpacity;
    }

    Int32 N = mNum;
    Int32 op = IPixelFormat::TRANSPARENT;
    if (N > 0) {
        (*mChildren)[0]->mDrawable->GetOpacity(&op);
    }
    for (Int32 i = 1; i < N; i++) {
        Int32 op2;
        (*mChildren)[i]->mDrawable->GetOpacity(&op2);
        Drawable::ResolveOpacity(op, op2, &op);
    }
    mOpacity = op;
    mHaveOpacity = TRUE;
    return op;
}

Boolean LayerDrawable::LayerState::IsStateful()
{
    if (mHaveIsStateful) {
        return mIsStateful;
    }

    Boolean stateful = FALSE;
    Int32 N = mNum;
    for (Int32 i = 0; i < N; i++) {
        Boolean isStateful;
        (*mChildren)[i]->mDrawable->IsStateful(&isStateful);
        if (isStateful) {
            stateful = TRUE;
            break;
        }
    }

    mIsStateful = stateful;
    mHaveIsStateful = TRUE;
    return stateful;
}

Boolean LayerDrawable::LayerState::CanConstantState()
{
    AutoPtr<ArrayOf<ChildDrawable*> > array = mChildren;
    const Int32 N = mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IDrawableConstantState> state;
        (*array)[i]->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        if (state == NULL) {
            return FALSE;
        }
    }

    // Don't cache the result, this method is not called very often.
    return TRUE;
}

void LayerDrawable::LayerState::InvalidateCache()
{
    mHaveOpacity = FALSE;
    mHaveIsStateful = FALSE;
}


//=====================================================================
// LayerDrawable
//=====================================================================
CAR_INTERFACE_IMPL_2(LayerDrawable, Drawable, ILayerDrawable, IDrawableCallback);
LayerDrawable::LayerDrawable()
    : mOpacityOverride(IPixelFormat::UNKNOWN)
    , mMutated(FALSE)
{
    CRect::New((IRect**)&mTmpRect);
}

LayerDrawable::~LayerDrawable()
{}

ECode LayerDrawable::constructor()
{
    return constructor(NULL, NULL, NULL);
}

ECode LayerDrawable::constructor(
    /* [in] */ ArrayOf<IDrawable*>* layers)
{
    return constructor(layers, NULL);
}

ECode LayerDrawable::constructor(
    /* [in] */ ArrayOf<IDrawable*>* layers,
    /* [in] */ IDrawableConstantState* state)
{
    constructor(state, NULL, NULL);
    Int32 length = layers->GetLength();
    AutoPtr< ArrayOf<ChildDrawable*> > r = ArrayOf<ChildDrawable*>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        AutoPtr<ChildDrawable> cdTemp = new ChildDrawable();
        r->Set(i, cdTemp);
        (*r)[i]->mDrawable = (*layers)[i];
        (*layers)[i]->SetCallback(this);
        Int32 config;
        (*layers)[i]->GetChangingConfigurations(&config);
        mLayerState->mChildrenChangingConfigurations |= config;
    }
    mLayerState->mNum = length;
    mLayerState->mChildren = r;

    EnsurePadding();
    return NOERROR;
}

ECode LayerDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<LayerState> as = CreateConstantState((LayerState*)state, res);
    mLayerState = as;
    if (as->mNum > 0) {
        EnsurePadding();
    }
    Boolean can = FALSE;
    if (theme != NULL && (CanApplyTheme(&can), can)) {
        ApplyTheme(theme);
    }

    return NOERROR;
}

AutoPtr<LayerDrawable::LayerState> LayerDrawable::CreateConstantState(
    /* [in] */ LayerState* state,
    /* [in] */ IResources* res)
{
    return new LayerState(state, this, res);
}

ECode LayerDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(Drawable::Inflate(r, parser, attrs, theme));

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::LayerDrawable);
    AutoPtr<ITypedArray> a;
    ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a);
    UpdateStateFromTypedArray(a);
    a->Recycle();

    FAIL_RETURN(InflateLayers(r, parser, attrs, theme));

    EnsurePadding();
    AutoPtr<ArrayOf<Int32> > state;
    GetState((ArrayOf<Int32>**)&state);
    OnStateChange(state);
    return NOERROR;
}

void LayerDrawable::UpdateStateFromTypedArray(
    /* [in] */ ITypedArray* a)
{
    AutoPtr<LayerState> state = mLayerState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    state->mThemeAttrs = NULL;
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&state->mThemeAttrs);

    a->GetInt32(R::styleable::LayerDrawable_opacity, mOpacityOverride, &mOpacityOverride);

    a->GetBoolean(R::styleable::LayerDrawable_autoMirrored,
            state->mAutoMirrored, &state->mAutoMirrored);
    a->GetInteger(R::styleable::LayerDrawable_paddingMode,
            state->mPaddingMode, &state->mPaddingMode);
}

ECode LayerDrawable::InflateLayers(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<LayerState> state = mLayerState;

    Int32 innerDepth = 0;
    parser->GetDepth(&innerDepth);
    innerDepth += 1;
    Int32 type = 0;
    Int32 depth = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && ((parser->GetDepth(&depth), depth) >= innerDepth || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        if (depth > innerDepth || (parser->GetName(&name), !name.Equals("item"))) {
            continue;
        }

        AutoPtr<ChildDrawable> layer = new ChildDrawable();
        AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::LayerDrawableItem);
        AutoPtr<ITypedArray> a;
        FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));
        UpdateLayerFromTypedArray(layer, a);
        a->Recycle();

        if (layer->mDrawable == NULL) {
            while ((parser->Next(&type), type) == IXmlPullParser::TEXT) {
            }
            if (type != IXmlPullParser::START_TAG) {
                // throw new XmlPullParserException(parser.getPositionDescription()
                //         + ": <item> tag requires a 'drawable' attribute or "
                //         + "child tag defining a drawable");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            FAIL_RETURN(Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&layer->mDrawable));
        }

        if (layer->mDrawable != NULL) {
            Int32 config = 0;
            layer->mDrawable->GetChangingConfigurations(&config);
            state->mChildrenChangingConfigurations |= config;
            layer->mDrawable->SetCallback(this);
        }

        AddLayer(layer);
    }
    return NOERROR;
}

void LayerDrawable::UpdateLayerFromTypedArray(
    /* [in] */ ChildDrawable* layer,
    /* [in] */ ITypedArray* a)
{
    AutoPtr<LayerState> state = mLayerState;

    // Account for any configuration changes.
    Int32 config = 0;
    a->GetChangingConfigurations(&config);
    state->mChildrenChangingConfigurations |= config;

    // Extract the theme attributes, if any.
    layer->mThemeAttrs = NULL;
    ((CTypedArray*)a)->ExtractThemeAttrs((ArrayOf<Int32>**)&layer->mThemeAttrs);

    a->GetDimensionPixelOffset(R::styleable::LayerDrawableItem_left,
            layer->mInsetL, &layer->mInsetL);
    a->GetDimensionPixelOffset(R::styleable::LayerDrawableItem_top,
            layer->mInsetT, &layer->mInsetT);
    a->GetDimensionPixelOffset(R::styleable::LayerDrawableItem_right,
            layer->mInsetR, &layer->mInsetR);
    a->GetDimensionPixelOffset(R::styleable::LayerDrawableItem_bottom,
            layer->mInsetB, &layer->mInsetB);
    a->GetResourceId(R::styleable::LayerDrawableItem_id, layer->mId, &layer->mId);

    AutoPtr<IDrawable> dr;
    a->GetDrawable(R::styleable::LayerDrawableItem_drawable, (IDrawable**)&dr);
    if (dr != NULL) {
        layer->mDrawable = dr;
    }
}

ECode LayerDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    FAIL_RETURN(Drawable::ApplyTheme(t));

    AutoPtr<LayerState> state = mLayerState;
    if (state == NULL) {
        return NOERROR;
    }

    if (state->mThemeAttrs != NULL) {
        AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::LayerDrawable);
        AutoPtr<ITypedArray> a;
        ((CResources::Theme*)t)->ResolveAttribute(state->mThemeAttrs, layout, (ITypedArray**)&a);
        UpdateStateFromTypedArray(a);
        a->Recycle();
    }

    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> layer = (*array)[i];
        if (layer->mThemeAttrs != NULL) {
            AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::LayerDrawableItem);
            AutoPtr<ITypedArray> a;
            ((CResources::Theme*)t)->ResolveAttribute(layer->mThemeAttrs, layout, (ITypedArray**)&a);
            UpdateLayerFromTypedArray(layer, a);
            a->Recycle();
        }

        AutoPtr<IDrawable> d = layer->mDrawable;
        Boolean can = FALSE;
        if (d->CanApplyTheme(&can), can) {
            d->ApplyTheme(t);
        }
    }

    EnsurePadding();
    AutoPtr<ArrayOf<Int32> > states;
    GetState((ArrayOf<Int32>**)&states);
    return OnStateChange(states);
}

ECode LayerDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    AutoPtr<LayerState> state = mLayerState;
    if (state == NULL) {
        *can = FALSE;
        return NOERROR;
    }

    if (state->mThemeAttrs != NULL) {
        *can = TRUE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<ChildDrawable*> >  array = state->mChildren;
    const Int32 N = state->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> layer = (*array)[i];
        Boolean tmp = FALSE;
        if (layer->mThemeAttrs != NULL || (layer->mDrawable->CanApplyTheme(&tmp), tmp)) {
            *can = TRUE;
            return NOERROR;
        }
    }

    *can = FALSE;
    return NOERROR;
}

ECode LayerDrawable::IsProjected(
    /* [out] */ Boolean* projected)
{
    VALIDATE_NOT_NULL(projected);
    if (Drawable::IsProjected(projected), *projected) {
        *projected = TRUE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<ChildDrawable*> > layers = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    Boolean tmp = FALSE;
    for (Int32 i = 0; i < N; i++) {
        if ((*layers)[i]->mDrawable->IsProjected(&tmp), tmp) {
            *projected = TRUE;
            return NOERROR;
        }
    }

    *projected = FALSE;
    return NOERROR;
}

void LayerDrawable::AddLayer(
    /* [in] */ ChildDrawable* layer)
{
    AutoPtr<LayerState> st = mLayerState;
    const Int32 N = st->mChildren != NULL ? st->mChildren->GetLength() : 0;
    const Int32 i = st->mNum;
    if (i >= N) {
        AutoPtr<ArrayOf<ChildDrawable*> > nu = ArrayOf<ChildDrawable*>::Alloc(N + 10);
        if (i > 0) {
            // System.arraycopy(st.mChildren, 0, nu, 0, i);
            nu->Copy(0, st->mChildren, 0, i);
        }

        st->mChildren = nu;
    }

    st->mChildren->Set(i, layer);
    st->mNum++;
    st->InvalidateCache();
}

AutoPtr<ChildDrawable> LayerDrawable::AddLayer(
    /* [in] */ IDrawable* layer,
    /* [in] */ ArrayOf<Int32>* themeAttrs,
    /* [in] */ Int32 id,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<ChildDrawable> childDrawable = new ChildDrawable();
    childDrawable->mId = id;
    childDrawable->mThemeAttrs = themeAttrs;
    childDrawable->mDrawable = layer;
    Boolean mirrored = FALSE;
    IsAutoMirrored(&mirrored);
    childDrawable->mDrawable->SetAutoMirrored(mirrored);
    childDrawable->mInsetL = left;
    childDrawable->mInsetT = top;
    childDrawable->mInsetR = right;
    childDrawable->mInsetB = bottom;

    AddLayer(childDrawable);

    Int32 config = 0;
    layer->GetChangingConfigurations(&config);
    mLayerState->mChildrenChangingConfigurations |= config;
    layer->SetCallback(this);

    return childDrawable;
}

ECode LayerDrawable::FindDrawableByLayerId(
    /* [in] */ Int32 id,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr< ArrayOf<ChildDrawable*> > layers = mLayerState->mChildren;
    for (Int32 i = mLayerState->mNum - 1; i >= 0; i--) {
        if ((*layers)[i]->mId == id) {
            *drawable = (*layers)[i]->mDrawable;
            REFCOUNT_ADD(*drawable);
            return NOERROR;
        }
    }

    *drawable = NULL;
    return NOERROR;
}

ECode LayerDrawable::SetId(
    /* [in] */ Int32 index,
    /* [in] */ Int32 id)
{
    (*mLayerState->mChildren)[index]->mId = id;
    return NOERROR;
}

ECode LayerDrawable::GetNumberOfLayers(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mLayerState->mNum;
    return NOERROR;
}

ECode LayerDrawable::GetDrawable(
    /* [in] */ Int32 index,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = (*mLayerState->mChildren)[index]->mDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode LayerDrawable::GetId(
    /* [in] */ Int32 index,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = (*mLayerState->mChildren)[index]->mId;
    return NOERROR;
}

ECode LayerDrawable::SetDrawableByLayerId(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* drawable,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr< ArrayOf<ChildDrawable*> > layers = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> childDrawable = (*layers)[i];
        if (childDrawable->mId == id) {
            if (childDrawable->mDrawable != NULL) {
                if (drawable != NULL) {
                    AutoPtr<IRect> bounds;
                    childDrawable->mDrawable->GetBounds((IRect**)&bounds);
                    drawable->SetBounds(bounds);
                }

                childDrawable->mDrawable->SetCallback(NULL);
            }

            if (drawable != NULL) {
                drawable->SetCallback(this);
            }

            childDrawable->mDrawable = drawable;
            mLayerState->InvalidateCache();
            *res = TRUE;
            return NOERROR;
        }
    }

    *res = FALSE;
    return NOERROR;
}

ECode LayerDrawable::SetLayerInset(
    /* [in] */ Int32 index,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AutoPtr<ChildDrawable> childDrawable = (*mLayerState->mChildren)[index];
    childDrawable->mInsetL = l;
    childDrawable->mInsetT = t;
    childDrawable->mInsetR = r;
    childDrawable->mInsetB = b;
    return NOERROR;
}

ECode LayerDrawable::SetPaddingMode(
    /* [in] */ Int32 mode)
{
    if (mLayerState->mPaddingMode != mode) {
        mLayerState->mPaddingMode = mode;
    }
    return NOERROR;
}

ECode LayerDrawable::GetPaddingMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mLayerState->mPaddingMode;
    return NOERROR;
}

ECode LayerDrawable::InvalidateDrawable(
    /* [in] */ IDrawable* who)
{
    return InvalidateSelf();
}

ECode LayerDrawable::ScheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what,
    /* [in] */ Int64 when)
{
    return ScheduleSelf(what, when);
}

ECode LayerDrawable::UnscheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what)
{
    return UnscheduleSelf(what);
}

ECode LayerDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->Draw(canvas);
    }
    return NOERROR;
}

ECode LayerDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configs)
{
    VALIDATE_NOT_NULL(configs);
    Drawable::GetChangingConfigurations(configs);
    *configs = (*configs)
            | mLayerState->mChangingConfigurations
            | mLayerState->mChildrenChangingConfigurations;
    return NOERROR;
}

ECode LayerDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    VALIDATE_NOT_NULL(isPadding);
    if (mLayerState->mPaddingMode == PADDING_MODE_NEST) {
        ComputeNestedPadding(padding);
    }
    else {
        ComputeStackedPadding(padding);
    }

    CRect* rect = (CRect*)padding;
    *isPadding = rect->mLeft != 0 || rect->mTop != 0 || rect->mRight != 0 || rect->mBottom != 0;
    return NOERROR;
}

void LayerDrawable::ComputeNestedPadding(
    /* [in] */ IRect* rect)
{
    CRect* padding = (CRect*)rect;
    padding->mLeft = 0;
    padding->mTop = 0;
    padding->mRight = 0;
    padding->mBottom = 0;

    // Add all the padding.
    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        RefreshChildPadding(i, (*array)[i]);

        padding->mLeft += (*mPaddingL)[i];
        padding->mTop += (*mPaddingT)[i];
        padding->mRight += (*mPaddingR)[i];
        padding->mBottom += (*mPaddingB)[i];
    }
}

void LayerDrawable::ComputeStackedPadding(
    /* [in] */ IRect* _padding)
{
    CRect* padding = (CRect*)_padding;
    padding->mLeft = 0;
    padding->mTop = 0;
    padding->mRight = 0;
    padding->mBottom = 0;

    // Take the max padding.
    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        RefreshChildPadding(i, (*array)[i]);

        using Elastos::Core::Math;
        padding->mLeft = Math::Max(padding->mLeft, (*mPaddingL)[i]);
        padding->mTop = Math::Max(padding->mTop, (*mPaddingT)[i]);
        padding->mRight = Math::Max(padding->mRight, (*mPaddingR)[i]);
        padding->mBottom = Math::Max(padding->mBottom, (*mPaddingB)[i]);
    }
}

ECode LayerDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    AutoPtr<LayerState> state = mLayerState;
    AutoPtr<ArrayOf<ChildDrawable*> > children = state->mChildren;
    const Int32 N = state->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*children)[i]->mDrawable->GetOutline(outline);
        Boolean empty = FALSE;
        if (outline->IsEmpty(&empty), !empty) {
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode LayerDrawable::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetHotspot(x, y);
    }
    return NOERROR;
}

ECode LayerDrawable::SetHotspotBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetHotspotBounds(left, top, right, bottom);
    }

    if (mHotspotBounds == NULL) {
        CRect::New(left, top, right, bottom, (IRect**)&mHotspotBounds);
    }
    else {
        mHotspotBounds->Set(left, top, right, bottom);
    }
    return NOERROR;
}

ECode LayerDrawable::GetHotspotBounds(
    /* [in] */ IRect* outRect)
{
    if (mHotspotBounds != NULL) {
        outRect->Set(mHotspotBounds);
    }
    else {
        Drawable::GetHotspotBounds(outRect);
    }
    return NOERROR;
}

ECode LayerDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    Boolean changed = FALSE;
    Drawable::SetVisible(visible, restart, &changed);
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        Boolean res;
        (*array)[i]->mDrawable->SetVisible(visible, restart, &res);
    }

    *isDifferent = changed;
    return NOERROR;
}

ECode LayerDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetDither(dither);
    }
    return NOERROR;
}

ECode LayerDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetAlpha(alpha);
    }
    return NOERROR;
}

ECode LayerDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    if (mLayerState->mNum > 0) {
        // All layers should have the same alpha set on them - just return
        // the first one
        return (*array)[0]->mDrawable->GetAlpha(alpha);
    }
    return Drawable::GetAlpha(alpha);
}

ECode LayerDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetColorFilter(cf);
    }
    return NOERROR;
}

ECode LayerDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetTintList(tint);
    }
    return NOERROR;
}

ECode LayerDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetTintMode(tintMode);
    }
    return NOERROR;
}

ECode LayerDrawable::SetOpacity(
    /* [in] */ Int32 opacity)
{
    mOpacityOverride = opacity;
    return NOERROR;
}

ECode LayerDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    if (mOpacityOverride != IPixelFormat::UNKNOWN) {
        *opacity = mOpacityOverride;
        return NOERROR;
    }
    *opacity = mLayerState->GetOpacity();
    return NOERROR;
}

ECode LayerDrawable::SetAutoMirrored(
    /* [in] */ Boolean mirrored)
{
    mLayerState->mAutoMirrored = mirrored;

    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetAutoMirrored(mirrored);
    }
    return NOERROR;
}

ECode LayerDrawable::IsAutoMirrored(
    /* [out] */ Boolean* mirrored)
{
    VALIDATE_NOT_NULL(mirrored);
    *mirrored = mLayerState->mAutoMirrored;
    return NOERROR;
}

ECode LayerDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    *isStateful = mLayerState->IsStateful();
    return NOERROR;
}

Boolean LayerDrawable::OnStateChange(
    /* [in] */ ArrayOf<Int32>* state)
{
    Boolean paddingChanged = FALSE;
    Boolean changed = FALSE;

    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> r = (*array)[i];
        Boolean res = FALSE, res2 = FALSE;
        r->mDrawable->IsStateful(&res);
        r->mDrawable->SetState(state, &res2);
        if (res && res2) {
            changed = TRUE;
        }

        if (RefreshChildPadding(i, r)) {
            paddingChanged = TRUE;
        }
    }

    if (paddingChanged) {
        AutoPtr<IRect> rect;
        GetBounds((IRect**)&rect);
        OnBoundsChange(rect);
    }

    return changed;
}

Boolean LayerDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean paddingChanged = FALSE;
    Boolean changed = FALSE;

    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        Boolean res;
        (*array)[i]->mDrawable->SetLevel(level, &res);
        if (res) {
            changed = TRUE;
        }

        if (RefreshChildPadding(i, (*array)[i])) {
            paddingChanged = TRUE;
        }
    }

    if (paddingChanged) {
        AutoPtr<IRect> rect;
        GetBounds((IRect**)&rect);
        OnBoundsChange(rect);
    }

    return changed;
}

void LayerDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    Int32 padL = 0, padT = 0, padR = 0, padB = 0;

    Boolean nest = mLayerState->mPaddingMode == PADDING_MODE_NEST;
    CRect* rect = (CRect*)bounds;
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> r = (*array)[i];
        r->mDrawable->SetBounds(
            rect->mLeft + r->mInsetL + padL,
            rect->mTop + r->mInsetT + padT,
            rect->mRight - r->mInsetR - padR,
            rect->mBottom - r->mInsetB - padB);

        if (nest) {
            padL += (*mPaddingL)[i];
            padR += (*mPaddingR)[i];
            padT += (*mPaddingT)[i];
            padB += (*mPaddingB)[i];
        }
    }
}

Int32 LayerDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 width = -1;
    Int32 padL = 0, padR = 0;

    Boolean nest = mLayerState->mPaddingMode == PADDING_MODE_NEST;
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> r = (*array)[i];
        Int32 w;
        r->mDrawable->GetIntrinsicWidth(&w);
        w += r->mInsetL + r->mInsetR + padL + padR;
        if (w > width) {
            width = w;
        }

        if (nest) {
            padL += (*mPaddingL)[i];
            padR += (*mPaddingR)[i];
        }
    }

    *res = width;
    return NOERROR;
}

Int32 LayerDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 height = -1;
    Int32 padT = 0, padB = 0;

    Boolean nest = mLayerState->mPaddingMode == PADDING_MODE_NEST;
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ChildDrawable> r = (*array)[i];
        Int32 h;
        r->mDrawable->GetIntrinsicHeight(&h);
        h += r->mInsetT + r->mInsetB + + padT + padB;
        if (h > height) {
            height = h;
        }

        if (nest) {
            padT += (*mPaddingT)[i];
            padB += (*mPaddingB)[i];
        }
    }

    *res = height;
    return NOERROR;
}

Boolean LayerDrawable::RefreshChildPadding(
    /* [in] */ Int32 i,
    /* [in] */ ChildDrawable* r)
{
    CRect* rect = (CRect*)mTmpRect.Get();
    Boolean res;
    r->mDrawable->GetPadding(rect, &res);
    if (rect->mLeft != (*mPaddingL)[i] || rect->mTop != (*mPaddingT)[i] ||
            rect->mRight != (*mPaddingR)[i] || rect->mBottom != (*mPaddingB)[i]) {
        (*mPaddingL)[i] = rect->mLeft;
        (*mPaddingT)[i] = rect->mTop;
        (*mPaddingR)[i] = rect->mRight;
        (*mPaddingB)[i] = rect->mBottom;
        return TRUE;
    }
    return FALSE;
}

void LayerDrawable::EnsurePadding()
{
    Int32 N = mLayerState->mNum;
    if (mPaddingL != NULL && mPaddingL->GetLength() >= N) {
        return;
    }

    mPaddingL = ArrayOf<Int32>::Alloc(N);
    mPaddingT = ArrayOf<Int32>::Alloc(N);
    mPaddingR = ArrayOf<Int32>::Alloc(N);
    mPaddingB = ArrayOf<Int32>::Alloc(N);
}

ECode LayerDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    if (mLayerState->CanConstantState()) {
        GetChangingConfigurations(&mLayerState->mChangingConfigurations);
        *state = mLayerState;
        REFCOUNT_ADD(*state);
        return NOERROR;
    }
    *state = NULL;
    return NOERROR;
}

ECode LayerDrawable::Mutate()
{
    if (!mMutated) {
        mLayerState = CreateConstantState(mLayerState, NULL);
        AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
        Int32 N = mLayerState->mNum;
        for (Int32 i = 0; i < N; i++) {
            (*array)[i]->mDrawable->Mutate();
        }
        mMutated = TRUE;
    }
    return NOERROR;
}

ECode LayerDrawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    AutoPtr< ArrayOf<ChildDrawable*> > array = mLayerState->mChildren;
    const Int32 N = mLayerState->mNum;
    for (Int32 i = 0; i < N; i++) {
        (*array)[i]->mDrawable->SetLayoutDirection(layoutDirection);
    }
    return Drawable::SetLayoutDirection(layoutDirection);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos