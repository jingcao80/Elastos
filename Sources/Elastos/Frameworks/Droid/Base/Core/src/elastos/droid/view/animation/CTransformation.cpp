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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CTransformation.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CTransformation);

CAR_INTERFACE_IMPL(CTransformation, Object, ITransformation);

CTransformation::CTransformation()
    : mAlpha(0.0f)
    , mTransformationType(0)
    , mHasClipRect(FALSE)
{
}

CTransformation::~CTransformation()
{
}

ECode CTransformation::constructor()
{
    CRect::New((IRect**)&mClipRect);
    return Clear();
}

ECode CTransformation::Clear()
{
    if (mMatrix == NULL) {
        CMatrix::New((IMatrix**)&mMatrix);
    }
    else {
        mMatrix->Reset();
    }
    mClipRect->SetEmpty();
    mHasClipRect = FALSE;
    mAlpha = 1.0f;
    mTransformationType = ITransformation::TYPE_BOTH;

    return NOERROR;
}

ECode CTransformation::GetTransformationType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mTransformationType;
    return NOERROR;
}

ECode CTransformation::SetTransformationType(
    /* [in] */ Int32 transformationType)
{
    mTransformationType = transformationType;
    return NOERROR;
}

ECode CTransformation::Set(
    /* [in] */ ITransformation* t)
{
    t->GetAlpha(&mAlpha);
    AutoPtr<IMatrix> matrix;
    t->GetMatrix((IMatrix**)&matrix);
    mMatrix->Set(matrix);
    if (((CTransformation*)t)->mHasClipRect) {
        AutoPtr<IRect> cr;
        t->GetClipRect((IRect**)&cr);
        SetClipRect(cr);
    }
    else {
        mHasClipRect = FALSE;
        mClipRect->SetEmpty();
    }
    t->GetTransformationType(&mTransformationType);

    return NOERROR;
}

ECode CTransformation::Compose(
    /* [in] */ ITransformation* t)
{
    Float alpha;
    t->GetAlpha(&alpha);
    mAlpha *= alpha;

    AutoPtr<IMatrix> matrix;
    t->GetMatrix((IMatrix**)&matrix);
    Boolean res;
    mMatrix->PreConcat(matrix, &res);
    if (((CTransformation*)t)->mHasClipRect) {
        AutoPtr<IRect> cr;
        t->GetClipRect((IRect**)&cr);
        SetClipRect(cr);
    }
    return NOERROR;
}

ECode CTransformation::PostCompose(
    /* [in] */ ITransformation* t)
{
    assert(t != NULL);
    Float a = 0.f;
    t->GetAlpha(&a);

    mAlpha *= a;
    AutoPtr<IMatrix> mx;
    t->GetMatrix((IMatrix**)&mx);
    Boolean result;
    mMatrix->PostConcat(mx, &result);

    if (((CTransformation*)t)->mHasClipRect) {
        AutoPtr<IRect> cr;
        t->GetClipRect((IRect**)&cr);
        SetClipRect(cr);
    }
    return NOERROR;
}

ECode CTransformation::GetMatrix(
    /* [out] */ IMatrix** matrix)
{
    VALIDATE_NOT_NULL(matrix)
    *matrix = mMatrix;
    REFCOUNT_ADD(*matrix);
    return NOERROR;
}

ECode CTransformation::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode CTransformation::SetClipRect(
    /* [in] */ IRect* r)
{
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    r->Get(&left, &top, &right, &bottom);
    return SetClipRect(left, top, right, bottom);
}

ECode CTransformation::SetClipRect(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    mClipRect->Set(l, t, r, b);
    mHasClipRect = TRUE;
    return NOERROR;
}

ECode CTransformation::GetClipRect(
    /* [out] */ IRect** cr)
{
    VALIDATE_NOT_NULL(cr);
    *cr = mClipRect;
    REFCOUNT_ADD(*cr);
    return NOERROR;
}

ECode CTransformation::HasClipRect(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mHasClipRect;
    return NOERROR;
}

ECode CTransformation::GetAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode CTransformation::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<StringBuilder> sb = new StringBuilder(64);
    sb->Append("Transformation");
    ToShortString((IStringBuilder*)sb);
    return sb->ToString(str);
}

ECode CTransformation::ToShortString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<StringBuilder> sb = new StringBuilder(64);
    ToShortString((IStringBuilder*)sb);
    return sb->ToString(str);
}

ECode CTransformation::ToShortString(
    /* [in] */ IStringBuilder* sb)
{
    sb->Append(String("{alpha="));
    sb->Append(mAlpha);
    sb->Append(String(" matrix="));
    mMatrix->ToShortString(sb);
    sb->AppendChar('}');
    return NOERROR;
}

ECode CTransformation::PrintShortString(
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(String("{alpha="));
    pw->Print(mAlpha);
    pw->Print(String(" matrix="));
    mMatrix->PrintShortString(pw);
    pw->PrintChar('}');
    return NOERROR;
}

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos
