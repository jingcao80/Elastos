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

#include "elastos/droid/app/CRemoteInput.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/content/CClipData.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::CClipData;
using Elastos::Droid::Content::CIntent;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//=========================================================================
// CRemoteInput::Builder
//=========================================================================
CAR_INTERFACE_IMPL(CRemoteInput::Builder, Object, IRemoteInputBuilder)

CRemoteInput::Builder::Builder()
    : mFlags(CRemoteInput::DEFAULT_FLAGS)
{
    CBundle::New((IBundle**)&mExtras);
}

CRemoteInput::Builder::~Builder()
{}

ECode CRemoteInput::Builder::constructor(
    /* [in] */ const String& resultKey)
{
    if (resultKey == NULL) {
        Logger::E("", "IllegalArgumentException: Result key can't be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mResultKey = resultKey;
    return NOERROR;
}

ECode CRemoteInput::Builder::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = CNotification::SafeCharSequence(label);
    return NOERROR;
}

ECode CRemoteInput::Builder::SetChoices(
    /* [in] */ ArrayOf<ICharSequence*>* choices)
{
    if (choices == NULL) {
        mChoices = NULL;
    }
    else {
        mChoices = ArrayOf<ICharSequence*>::Alloc(choices->GetLength());
        for (Int32 i = 0; i < choices->GetLength(); i++) {
            mChoices->Set(i, CNotification::SafeCharSequence((*choices)[i]));
        }
    }
    return NOERROR;
}

ECode CRemoteInput::Builder::SetAllowFreeFormInput(
    /* [in] */ Boolean allowFreeFormInput)
{
    return SetFlag(mFlags, allowFreeFormInput);
}

ECode CRemoteInput::Builder::AddExtras(
    /* [in] */ IBundle* extras)
{
    if (extras != NULL) {
        mExtras->PutAll(extras);
    }
    return NOERROR;
}

ECode CRemoteInput::Builder::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRemoteInput::Builder::SetFlag(
    /* [in] */ Int32 mask,
    /* [in] */ Boolean value)
{
    if (value) {
        mFlags |= mask;
    } else {
        mFlags &= ~mask;
    }
    return NOERROR;
}

ECode CRemoteInput::Builder::Build(
    /* [out] */ IRemoteInput** ri)
{
    return CRemoteInput::New(mResultKey, mLabel, mChoices, mFlags, mExtras, ri);
}

//=========================================================================
// CRemoteInput
//=========================================================================

// Flags bitwise-ored to mFlags
const Int32 CRemoteInput::FLAG_ALLOW_FREE_FORM_INPUT = 0x1;

// Default value for flags integer
const Int32 CRemoteInput::DEFAULT_FLAGS = 0x1;

CAR_INTERFACE_IMPL_2(CRemoteInput, Object, IRemoteInput, IParcelable)

CAR_OBJECT_IMPL(CRemoteInput)

CRemoteInput::CRemoteInput()
    : mFlags(0)
{}

ECode CRemoteInput::constructor()
{
    return NOERROR;
}

ECode CRemoteInput::constructor(
    /* [in] */ const String& resultKey,
    /* [in] */ ICharSequence* label,
    /* [in] */ ArrayOf<ICharSequence*>* choices,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* extras)
{
    mResultKey = resultKey;
    mLabel = label;
    mChoices = choices;
    mFlags = flags;
    mExtras = extras;
    return NOERROR;
}

ECode CRemoteInput::GetResultKey(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mResultKey;
    return NOERROR;
}

ECode CRemoteInput::GetLabel(
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)
    *seq = mLabel;
    REFCOUNT_ADD(*seq)
    return NOERROR;
}

ECode CRemoteInput::GetChoices(
    /* [out, callee] */ ArrayOf<ICharSequence*>** choices)
{
    VALIDATE_NOT_NULL(choices)
    *choices = mChoices;
    REFCOUNT_ADD(*choices)
    return NOERROR;
}

ECode CRemoteInput::GetAllowFreeFormInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & FLAG_ALLOW_FREE_FORM_INPUT) != 0;
    return NOERROR;
}

ECode CRemoteInput::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRemoteInput::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mResultKey);
    in->ReadInterfacePtr((HANDLE*)&mLabel);
    mChoices = Elastos::Droid::Os::CParcel::ReadCharSequenceArray(in);
    in->ReadInt32(&mFlags);
    mExtras = Elastos::Droid::Os::CParcel::ReadBundle(in);
    return NOERROR;
}

ECode CRemoteInput::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mResultKey);
    out->WriteInterfacePtr(mLabel.Get());
    Elastos::Droid::Os::CParcel::WriteCharSequenceArray(out, mChoices);
    out->WriteInt32(mFlags);
    Elastos::Droid::Os::CParcel::WriteBundle(out, mExtras);
    return NOERROR;
}

AutoPtr<IBundle> CRemoteInput::GetResultsFromIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IClipData> clipData;
    intent->GetClipData((IClipData**)&clipData);
    if (clipData == NULL) {
        return NULL;
    }
    AutoPtr<IClipDescription> clipDescription;
    clipData->GetDescription((IClipDescription**)&clipDescription);
    Boolean bval;
    clipDescription->HasMimeType(IClipDescription::MIMETYPE_TEXT_INTENT, &bval);
    if (!bval) {
        return NULL;
    }
    AutoPtr<ICharSequence> label;
    clipDescription->GetLabel((ICharSequence**)&label);
    if (Object::ToString(label).Equals(IRemoteInput::RESULTS_CLIP_LABEL)) {
        AutoPtr<IClipDataItem> item;
        clipData->GetItemAt(0, (IClipDataItem**)&item);
        AutoPtr<IIntent> intent;
        item->GetIntent((IIntent**)&intent);
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        AutoPtr<IParcelable> p;
        extras->GetParcelable(IRemoteInput::EXTRA_RESULTS_DATA, (IParcelable**)&p);
        return IBundle::Probe(p);
    }
    return NULL;
}

ECode CRemoteInput::AddResultsToIntent(
    /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* results)
{
    AutoPtr<IBundle> resultsBundle;
    CBundle::New((IBundle**)&resultsBundle);
    String key;
    for (Int32 i = 0; i < remoteInputs->GetLength(); ++i) {
        IRemoteInput* remoteInput = (*remoteInputs)[i];
        remoteInput->GetResultKey(&key);
        AutoPtr<IInterface> result;
        results->Get(key, (IInterface**)&result);
        if (ICharSequence::Probe(result)) {
            resultsBundle->PutCharSequence(key, ICharSequence::Probe(result));
        }
    }
    AutoPtr<IIntent> clipIntent;
    CIntent::New((IIntent**)&clipIntent);
    clipIntent->PutExtra(IRemoteInput::EXTRA_RESULTS_DATA, resultsBundle);
    AutoPtr<IClipData> cd;
    CClipData::NewIntent(
        CoreUtils::Convert(IRemoteInput::RESULTS_CLIP_LABEL), clipIntent, (IClipData**)&cd);
    intent->SetClipData(cd);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos