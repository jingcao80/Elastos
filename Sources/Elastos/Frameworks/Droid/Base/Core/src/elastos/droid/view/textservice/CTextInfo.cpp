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
#include "elastos/droid/view/textservice/CTextInfo.h"
#include "elastos/droid/text/CTextUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"

using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::Style::ISpellCheckSpan;
using Elastos::Droid::Text::Style::EIID_ISpellCheckSpan;

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CTextInfo::
//========================================================================================
Int32 CTextInfo::DEFAULT_COOKIE = 0;
Int32 CTextInfo::DEFAULT_SEQUENCE_NUMBER = 0;

CAR_INTERFACE_IMPL_2(CTextInfo, Object, ITextInfo, IParcelable)

CAR_OBJECT_IMPL(CTextInfo)

ECode CTextInfo::constructor()
{
    return NOERROR;
}

ECode CTextInfo::constructor(
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    return constructor(cs, 0, GetStringLengthOrZero(text), DEFAULT_COOKIE, DEFAULT_SEQUENCE_NUMBER);
}

ECode CTextInfo::constructor(
    /* [in] */ const String& text,
    /* [in] */ Int32 cookie,
    /* [in] */ Int32 sequence)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    return constructor(cs, 0, GetStringLengthOrZero(text), cookie, sequence);
}

Int32 CTextInfo::GetStringLengthOrZero(
    /* [in] */ const String& text)
{
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    return (tu->IsEmpty(text, &bEmp), bEmp) ? 0 : text.GetLength();
}

ECode CTextInfo::constructor(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 cookie,
    /* [in] */ Int32 sequenceNumber)
{
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if (tu->IsEmpty(charSequence, &bEmp), bEmp) {
        // throw new IllegalArgumentException("charSequence is empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Create a snapshot of the text including spans in case they are updated outside later.
    AutoPtr<ISpannableStringBuilder> spannableString;
    CSpannableStringBuilder::New(charSequence, start, end, (ISpannableStringBuilder**)&spannableString);
    // SpellCheckSpan is for internal use. We do not want to marshal this for TextService.
    AutoPtr<ArrayOf<IInterface*> > spans;
    Int32 len = 0;
    ICharSequence::Probe(spannableString)->GetLength(&len);
    ISpanned::Probe(spannableString)->GetSpans(0, len,
            EIID_ISpellCheckSpan, (ArrayOf<IInterface*>**)&spans);
    for (Int32 i = 0; i < spans->GetLength(); ++i) {
        ISpannable::Probe(spannableString)->RemoveSpan((*spans)[i]);
    }

    mCharSequence = ICharSequence::Probe(spannableString);
    mCookie = cookie;
    mSequenceNumber = sequenceNumber;
    return NOERROR;
}

ECode CTextInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mCharSequence);
    source->ReadInt32(&mCookie);
    source->ReadInt32(&mSequenceNumber);

    return NOERROR;
}

ECode CTextInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    tu->WriteToParcel(mCharSequence, dest);
    dest->WriteInt32(mCookie);
    dest->WriteInt32(mSequenceNumber);

    return NOERROR;
}

ECode CTextInfo::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    if (mCharSequence == NULL) {
        *text = NULL;
        return NOERROR;
    }
    return mCharSequence->ToString(text);
}

ECode CTextInfo::GetCharSequence(
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)
    *seq = mCharSequence;
    REFCOUNT_ADD(*seq)
    return NOERROR;
}

ECode CTextInfo::GetCookie(
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = mCookie;
    return NOERROR;
}

ECode CTextInfo::GetSequence(
    /* [out] */ Int32* sequence)
{
    VALIDATE_NOT_NULL(sequence);
    *sequence = mSequenceNumber;
    return NOERROR;
}

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
