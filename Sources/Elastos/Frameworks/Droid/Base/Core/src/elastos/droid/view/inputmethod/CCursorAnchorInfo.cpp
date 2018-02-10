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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CCursorAnchorInfo.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/text/CSpannedString.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/inputmethod/CSparseRectFArray.h"

#include <elastos/core/Math.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::CSpannedString;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::InputMethod::CSparseRectFArray;

using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CCursorAnchorInfo::Builder::
//========================================================================================
CAR_INTERFACE_IMPL(CCursorAnchorInfo::Builder, Object, ICursorAnchorInfoBuilder)

CCursorAnchorInfo::Builder::Builder()
    : mSelectionStart(-1)
    , mSelectionEnd(-1)
    , mComposingTextStart(-1)
    , mInsertionMarkerHorizontal(Elastos::Core::Math::FLOAT_NAN)
    , mInsertionMarkerTop(Elastos::Core::Math::FLOAT_NAN)
    , mInsertionMarkerBaseline(Elastos::Core::Math::FLOAT_NAN)
    , mInsertionMarkerBottom(Elastos::Core::Math::FLOAT_NAN)
    , mInsertionMarkerFlags(0)
    , mMatrixInitialized(FALSE)
{
    CMatrix::New(CMatrix::IDENTITY_MATRIX, (IMatrix**)&mMatrix);
}

ECode CCursorAnchorInfo::Builder::SetSelectionRange(
    /* [in] */ Int32 newStart,
    /* [in] */ Int32 newEnd)
{
    mSelectionStart = newStart;
    mSelectionEnd = newEnd;
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::SetComposingText(
    /* [in] */ Int32 composingTextStart,
    /* [in] */ ICharSequence* composingText)
{
    mComposingTextStart = composingTextStart;
    if (composingText == NULL) {
        mComposingText = NULL;
    }
    else {
        // Make a snapshot of the given char sequence.
        AutoPtr<ISpannedString> ss;
        CSpannedString::New(composingText, (ISpannedString**)&ss);
        mComposingText = ICharSequence::Probe(ss);
    }
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::SetInsertionMarkerLocation(
    /* [in] */ Float horizontalPosition,
    /* [in] */ Float lineTop,
    /* [in] */ Float lineBaseline,
    /* [in] */ Float lineBottom,
    /* [in] */ Int32 flags)
{
    mInsertionMarkerHorizontal = horizontalPosition;
    mInsertionMarkerTop = lineTop;
    mInsertionMarkerBaseline = lineBaseline;
    mInsertionMarkerBottom = lineBottom;
    mInsertionMarkerFlags = flags;
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::AddCharacterBounds(
    /* [in] */ Int32 index,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 flags)
{
    if (index < 0) {
        Logger::E(TAG, "index must not be a negative integer.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mCharacterBoundsArrayBuilder == NULL) {
        mCharacterBoundsArrayBuilder = new CSparseRectFArray::SparseRectFArrayBuilder();
    }
    mCharacterBoundsArrayBuilder->Append(index, left, top, right, bottom, flags);
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    mMatrix->Set(matrix != NULL ? matrix : CMatrix::IDENTITY_MATRIX.Get());
    mMatrixInitialized = TRUE;
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::Build(
    /* [out] */ ICursorAnchorInfo** info)
{
    VALIDATE_NOT_NULL(info)

    if (!mMatrixInitialized) {
        // Coordinate transformation matrix is mandatory when at least one positional
        // parameter is specified.
        Boolean bIsEmp = FALSE;
        Boolean hasCharacterBounds = (mCharacterBoundsArrayBuilder != NULL
                && !(mCharacterBoundsArrayBuilder->IsEmpty(&bIsEmp), bIsEmp));
        if (hasCharacterBounds
                || !Elastos::Core::Math::IsNaN(mInsertionMarkerHorizontal)
                || !Elastos::Core::Math::IsNaN(mInsertionMarkerTop)
                || !Elastos::Core::Math::IsNaN(mInsertionMarkerBaseline)
                || !Elastos::Core::Math::IsNaN(mInsertionMarkerBottom)) {
            // throw new IllegalArgumentException("Coordinate transformation matrix is " +
            //         "required when positional parameters are specified.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    AutoPtr<ICursorAnchorInfo> res;
    CCursorAnchorInfo::New(this, (ICursorAnchorInfo**)&res);
    *info = res;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode CCursorAnchorInfo::Builder::Reset()
{
    mSelectionStart = -1;
    mSelectionEnd = -1;
    mComposingTextStart = -1;
    mComposingText = NULL;
    mInsertionMarkerFlags = 0;
    mInsertionMarkerHorizontal = Elastos::Core::Math::FLOAT_NAN;
    mInsertionMarkerTop = Elastos::Core::Math::FLOAT_NAN;
    mInsertionMarkerBaseline = Elastos::Core::Math::FLOAT_NAN;
    mInsertionMarkerBottom = Elastos::Core::Math::FLOAT_NAN;
    mMatrix->Set(CMatrix::IDENTITY_MATRIX);
    mMatrixInitialized = FALSE;
    if (mCharacterBoundsArrayBuilder != NULL) {
        mCharacterBoundsArrayBuilder->Reset();
    }
    return NOERROR;
}

//========================================================================================
//              CCursorAnchorInfo::
//========================================================================================
const String CCursorAnchorInfo::TAG("CCursorAnchorInfo");

CAR_INTERFACE_IMPL_2(CCursorAnchorInfo, Object, ICursorAnchorInfo, IParcelable)

CAR_OBJECT_IMPL(CCursorAnchorInfo)

ECode CCursorAnchorInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSelectionStart);
    source->ReadInt32(&mSelectionEnd);
    source->ReadInt32(&mComposingTextStart);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mComposingText);
    source->ReadInt32(&mInsertionMarkerFlags);
    source->ReadFloat(&mInsertionMarkerHorizontal);
    source->ReadFloat(&mInsertionMarkerTop);
    source->ReadFloat(&mInsertionMarkerBaseline);
    source->ReadFloat(&mInsertionMarkerBottom);
    assert(0 && "TODO");
//    mCharacterBoundsArray = source.readParcelable(SparseRectFArray.class.getClassLoader());
    CMatrix::New((IMatrix**)&mMatrix);
    AutoPtr<ArrayOf<Float> > arr;
    source->ReadArrayOf((HANDLE*)&arr);
    mMatrix->SetValues(arr);
    return NOERROR;
}

ECode CCursorAnchorInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSelectionStart);
    dest->WriteInt32(mSelectionEnd);
    dest->WriteInt32(mComposingTextStart);
    TextUtils::WriteToParcel(mComposingText, dest);
    dest->WriteInt32(mInsertionMarkerFlags);
    dest->WriteFloat(mInsertionMarkerHorizontal);
    dest->WriteFloat(mInsertionMarkerTop);
    dest->WriteFloat(mInsertionMarkerBaseline);
    dest->WriteFloat(mInsertionMarkerBottom);
    assert(0 && "TODO");
//    dest.writeParcelable(mCharacterBoundsArray, flags);
    AutoPtr<ArrayOf<Float> > matrixArray = ArrayOf<Float>::Alloc(9);
    mMatrix->GetValues(matrixArray);
    dest->WriteArrayOf((HANDLE)matrixArray.Get());
    return NOERROR;
}

#ifdef __clang__
__attribute__((no_sanitize("integer")))
#endif
ECode CCursorAnchorInfo::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc)

    Float floatHash = mInsertionMarkerHorizontal + mInsertionMarkerTop
            + mInsertionMarkerBaseline + mInsertionMarkerBottom;
    Int32 hash = floatHash > 0 ? (Int32) floatHash : (Int32)(-floatHash);
    hash *= 31;
    hash += mInsertionMarkerFlags;
    hash *= 31;
    hash += mSelectionStart + mSelectionEnd + mComposingTextStart;
    hash *= 31;
    hash += Objects::GetHashCode(mComposingText);
    hash *= 31;
    hash += Objects::GetHashCode(mCharacterBoundsArray);
    hash *= 31;
    hash += Objects::GetHashCode(mMatrix);
    *hc = hash;
    return NOERROR;
}

Boolean CCursorAnchorInfo::AreSameFloatImpl(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    if (Elastos::Core::Math::IsNaN(a) && Elastos::Core::Math::IsNaN(b)) {
        return TRUE;
    }
    return a == b;
}

ECode CCursorAnchorInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj->Probe(EIID_ICursorAnchorInfo) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICursorAnchorInfo> that = ICursorAnchorInfo::Probe(obj);
    Int32 thisHc = 0, thatHc = 0;
    GetHashCode(&thisHc);
    IObject::Probe(that)->GetHashCode(&thatHc);
    if (thisHc != thatHc) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CCursorAnchorInfo> cThat = (CCursorAnchorInfo*)that.Get();
    if (mSelectionStart != cThat->mSelectionStart || mSelectionEnd != cThat->mSelectionEnd) {
        *result = FALSE;
        return NOERROR;
    }
    if (mComposingTextStart != cThat->mComposingTextStart
            || !Objects::Equals(mComposingText, cThat->mComposingText)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mInsertionMarkerFlags != cThat->mInsertionMarkerFlags
            || !AreSameFloatImpl(mInsertionMarkerHorizontal, cThat->mInsertionMarkerHorizontal)
            || !AreSameFloatImpl(mInsertionMarkerTop, cThat->mInsertionMarkerTop)
            || !AreSameFloatImpl(mInsertionMarkerBaseline, cThat->mInsertionMarkerBaseline)
            || !AreSameFloatImpl(mInsertionMarkerBottom, cThat->mInsertionMarkerBottom)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!Objects::Equals(mCharacterBoundsArray, cThat->mCharacterBoundsArray)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!Objects::Equals(mMatrix, cThat->mMatrix)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CCursorAnchorInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // *result = "SelectionInfo{mSelection=" + mSelectionStart + "," + mSelectionEnd
    //         + " mComposingTextStart=" + mComposingTextStart
    //         + " mComposingText=" + Objects::ToString(mComposingText)
    //         + " mInsertionMarkerFlags=" + mInsertionMarkerFlags
    //         + " mInsertionMarkerHorizontal=" + mInsertionMarkerHorizontal
    //         + " mInsertionMarkerTop=" + mInsertionMarkerTop
    //         + " mInsertionMarkerBaseline=" + mInsertionMarkerBaseline
    //         + " mInsertionMarkerBottom=" + mInsertionMarkerBottom
    //         + " mCharacterBoundsArray=" + Objects::ToString(mCharacterBoundsArray)
    //         + " mMatrix=" + Objects::ToString(mMatrix)
    //         + "}";
    return NOERROR;
}

ECode CCursorAnchorInfo::constructor()
{
    return NOERROR;
}

ECode CCursorAnchorInfo::constructor(
    /* [in] */ ICursorAnchorInfoBuilder* builder)
{
    AutoPtr<Builder> cBuilder = (Builder*)builder;
    mSelectionStart = cBuilder->mSelectionStart;
    mSelectionEnd = cBuilder->mSelectionEnd;
    mComposingTextStart = cBuilder->mComposingTextStart;
    mComposingText = cBuilder->mComposingText;
    mInsertionMarkerFlags = cBuilder->mInsertionMarkerFlags;
    mInsertionMarkerHorizontal = cBuilder->mInsertionMarkerHorizontal;
    mInsertionMarkerTop = cBuilder->mInsertionMarkerTop;
    mInsertionMarkerBaseline = cBuilder->mInsertionMarkerBaseline;
    mInsertionMarkerBottom = cBuilder->mInsertionMarkerBottom;
    if (cBuilder->mCharacterBoundsArrayBuilder != NULL) {
        cBuilder->mCharacterBoundsArrayBuilder->Build((ISparseRectFArray**)&mCharacterBoundsArray);
    }
    CMatrix::New(cBuilder->mMatrix, (IMatrix**)&mMatrix);
    return NOERROR;
}

ECode CCursorAnchorInfo::GetSelectionStart(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSelectionStart;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetSelectionEnd(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSelectionEnd;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetComposingTextStart(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mComposingTextStart;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetComposingText(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mComposingText;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCursorAnchorInfo::GetInsertionMarkerFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInsertionMarkerFlags;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetInsertionMarkerHorizontal(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInsertionMarkerHorizontal;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetInsertionMarkerTop(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInsertionMarkerTop;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetInsertionMarkerBaseline(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInsertionMarkerBaseline;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetInsertionMarkerBottom(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInsertionMarkerBottom;
    return NOERROR;
}

ECode CCursorAnchorInfo::GetCharacterBounds(
    /* [in] */ Int32 index,
    /* [out] */ IRectF** rect)
{
    VALIDATE_NOT_NULL(rect)

    if (mCharacterBoundsArray == NULL) {
        *rect = NULL;
        return NOERROR;
    }
    return mCharacterBoundsArray->Get(index, rect);
}

ECode CCursorAnchorInfo::GetCharacterBoundsFlags(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mCharacterBoundsArray == NULL) {
        *result = 0;
        return NOERROR;
    }
    return mCharacterBoundsArray->GetFlags(index, 0, result);
}

ECode CCursorAnchorInfo::GetMatrix(
    /* [out] */ IMatrix** matrix)
{
    VALIDATE_NOT_NULL(matrix)

    AutoPtr<IMatrix> res;
    CMatrix::New(mMatrix, (IMatrix**)&res);
    *matrix = res;
    REFCOUNT_ADD(*matrix)
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
