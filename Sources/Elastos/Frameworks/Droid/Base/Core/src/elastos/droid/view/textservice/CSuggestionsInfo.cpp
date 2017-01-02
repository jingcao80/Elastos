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

#include "elastos/droid/view/textservice/CSuggestionsInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CSuggestionsInfo::
//========================================================================================
AutoPtr<ArrayOf<String> > CSuggestionsInfo::EMPTY;// = ArrayUtils.emptyArray(String.class);

CAR_INTERFACE_IMPL_2(CSuggestionsInfo, Object, ISuggestionsInfo, IParcelable)

CAR_OBJECT_IMPL(CSuggestionsInfo)

ECode CSuggestionsInfo::constructor()
{
    return NOERROR;
}

ECode CSuggestionsInfo::constructor(
    /* [in] */ Int32 suggestionsAttributes,
    /* [in] */ ArrayOf<String>* suggestions)
{
    return constructor(suggestionsAttributes, suggestions, 0, 0);
}

ECode CSuggestionsInfo::constructor(
    /* [in] */ Int32 suggestionsAttributes,
    /* [in] */ ArrayOf<String>* suggestions,
    /* [in] */ Int32 cookie,
    /* [in] */ Int32 sequence)
{
    if (suggestions == NULL) {
        mSuggestions = EMPTY;
        mSuggestionsAvailable = FALSE;
    }
    else {
        mSuggestions = suggestions;
        mSuggestionsAvailable = TRUE;
    }
    mSuggestionsAttributes = suggestionsAttributes;
    mCookie = cookie;
    mSequence = sequence;

    return NOERROR;
}

ECode CSuggestionsInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSuggestionsAttributes);
    source->ReadArrayOfString((ArrayOf<String>**)&mSuggestions);
    source->ReadInt32(&mCookie);
    source->ReadInt32(&mSequence);
    Int32 temp = 0;
    source->ReadInt32(&temp);
    mSuggestionsAvailable = temp == 1;

    return NOERROR;
}

ECode CSuggestionsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSuggestionsAttributes);
    dest->WriteArrayOfString(mSuggestions);
    dest->WriteInt32(mCookie);
    dest->WriteInt32(mSequence);
    dest->WriteInt32(mSuggestionsAvailable ? 1 : 0);

    return NOERROR;
}

ECode CSuggestionsInfo::SetCookieAndSequence(
    /* [in] */ Int32 cookie,
    /* [in] */ Int32 sequence)
{
    mCookie = cookie;
    mSequence = sequence;
    return NOERROR;
}

ECode CSuggestionsInfo::GetCookie(
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = mCookie;
    return NOERROR;
}

ECode CSuggestionsInfo::GetSequence(
    /* [out] */ Int32* sequence)
{
    VALIDATE_NOT_NULL(sequence);
    *sequence = mSequence;
    return NOERROR;
}

ECode CSuggestionsInfo::GetSuggestionsAttributes(
    /* [out] */ Int32* attr)
{
    VALIDATE_NOT_NULL(attr);
    *attr = mSuggestionsAttributes;
    return NOERROR;
}

ECode CSuggestionsInfo::GetSuggestionsCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    if (!mSuggestionsAvailable) {
        *count = -1;
        return NOERROR;
    }

    *count = mSuggestions->GetLength();

    return NOERROR;
}

ECode CSuggestionsInfo::GetSuggestionAt(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = (*mSuggestions)[i];
    return NOERROR;
}

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
