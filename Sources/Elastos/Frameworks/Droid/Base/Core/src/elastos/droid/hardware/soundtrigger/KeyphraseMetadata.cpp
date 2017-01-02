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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/soundtrigger/KeyphraseMetadata.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(KeyphraseMetadata, Object, IKeyphraseMetadata)

KeyphraseMetadata::KeyphraseMetadata()
    : mId(0)
    , mKeyphrase(NULL)
    , mSupportedLocales(NULL)
    , mRecognitionModeFlags(0)
{
}

KeyphraseMetadata::~KeyphraseMetadata()
{
}

ECode KeyphraseMetadata::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& keyphrase,
    /* [in] */ IArraySet* supportedLocales,
    /* [in] */ Int32 recognitionModeFlags)
{
    mId = id;
    mKeyphrase = keyphrase;
    mSupportedLocales = supportedLocales;
    mRecognitionModeFlags = recognitionModeFlags;
    return NOERROR;
}

ECode KeyphraseMetadata::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "id=";
    sb += mId;
    sb += ", keyphrase=";
    sb += mKeyphrase;
    sb += ", supported-locales=";
    String tmp;
    (IObject::Probe(mSupportedLocales))->ToString(&tmp);
    sb += tmp;
    sb += ", recognition-modes=";
    sb += mRecognitionModeFlags;
    return sb.ToString(str);
}

ECode KeyphraseMetadata::SupportsPhrase(
    /* [in] */ const String& phrase,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mKeyphrase.IsEmpty() || mKeyphrase.EqualsIgnoreCase(phrase);
    return NOERROR;
}

ECode KeyphraseMetadata::SupportsLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean result1, result2;
    FAIL_RETURN(ICollection::Probe(mSupportedLocales)->IsEmpty(&result1))
    FAIL_RETURN(ICollection::Probe(mSupportedLocales)->Contains(TO_IINTERFACE(locale), &result2))
    *result = (result1 || result2);
    return NOERROR;
}

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos