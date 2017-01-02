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

#include "elastos/droid/view/textservice/CSentenceSuggestionsInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CSentenceSuggestionsInfo::
//========================================================================================
CAR_INTERFACE_IMPL_2(CSentenceSuggestionsInfo, Object, ISentenceSuggestionsInfo, IParcelable)

CAR_OBJECT_IMPL(CSentenceSuggestionsInfo)

ECode CSentenceSuggestionsInfo::constructor()
{
    return NOERROR;
}

ECode CSentenceSuggestionsInfo::constructor(
    /* [in] */ ArrayOf<ISuggestionsInfo*>* suggestionsInfos,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* lengths)
{
    if (suggestionsInfos == NULL || offsets == NULL ||lengths == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (suggestionsInfos->GetLength() != offsets->GetLength() ||
        offsets->GetLength() != lengths->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 infoSize = suggestionsInfos->GetLength();
    mSuggestionsInfos->Copy(suggestionsInfos, 0, infoSize);
    mOffsets->Copy(offsets, 0, infoSize);
    mLengths->Copy(lengths, 0, infoSize);

    return NOERROR;
}

ECode CSentenceSuggestionsInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 infoSize = 0;
    source->ReadInt32(&infoSize);
    mSuggestionsInfos = ArrayOf<ISuggestionsInfo*>::Alloc(infoSize);
    source->ReadArrayOf((Handle32*)&mSuggestionsInfos);
    mOffsets = ArrayOf<Int32>::Alloc(mSuggestionsInfos->GetLength());
    source->ReadArrayOf((Handle32*)&mOffsets);
    mLengths = ArrayOf<Int32>::Alloc(mSuggestionsInfos->GetLength());
    source->ReadArrayOf((Handle32*)&mLengths);

    return NOERROR;
}

ECode CSentenceSuggestionsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 infoSize = mSuggestionsInfos->GetLength();
    dest->WriteInt32(infoSize);
    dest->WriteArrayOf((Handle32)mSuggestionsInfos.Get());
    dest->WriteArrayOf((Handle32)mOffsets.Get());
    dest->WriteArrayOf((Handle32)mLengths.Get());

    return NOERROR;
}

ECode CSentenceSuggestionsInfo::GetSuggestionsCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    mSuggestionsInfos->GetLength();
    return NOERROR;
}

ECode CSentenceSuggestionsInfo::GetSuggestionsInfoAt(
    /* [in] */ Int32 i,
    /* [out] */ ISuggestionsInfo** info)
{
    VALIDATE_NOT_NULL(info);

    if (i >= 0 && i < mSuggestionsInfos->GetLength()) {
        *info = (*mSuggestionsInfos)[i];
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    *info = NULL;
    return NOERROR;
}

ECode CSentenceSuggestionsInfo::GetOffsetAt(
    /* [in] */ Int32 i,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    if (i >= 0 && i < mOffsets->GetLength()) {
        *offset = (*mOffsets)[i];
        return NOERROR;
    }

    *offset = -1;
    return NOERROR;
}

ECode CSentenceSuggestionsInfo::GetLengthAt(
    /* [in] */ Int32 i,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    if (i >= 0 && i < mLengths->GetLength()) {
        *length = (*mLengths)[i];
        return NOERROR;
    }

    *length = -1;
    return NOERROR;
}

} // namespace TextService
} // namespace View
} // namespace Droid
} // namespace Elastos
