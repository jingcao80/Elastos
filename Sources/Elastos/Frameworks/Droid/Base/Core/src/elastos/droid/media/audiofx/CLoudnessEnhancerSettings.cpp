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
#include "elastos/droid/media/audiofx/CLoudnessEnhancerSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::CStringTokenizer;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CLoudnessEnhancerSettings, Object, ILoudnessEnhancerSettings)

CAR_OBJECT_IMPL(CLoudnessEnhancerSettings)

CLoudnessEnhancerSettings::CLoudnessEnhancerSettings()
    : mTargetGainmB(0)
{
}

ECode CLoudnessEnhancerSettings::constructor()
{
    return NOERROR;
}

/**
 * Settings class constructor from a key=value; pairs formatted string. The string is
 * typically returned by Settings.toString() method.
 * @throws IllegalArgumentException if the string is not correctly formatted.
 */
ECode CLoudnessEnhancerSettings::constructor(
    /* [in] */ const String& settings)
{
    AutoPtr<IStringTokenizer> st;
    CStringTokenizer::New(settings, String("=;"), (IStringTokenizer**)&st);
    //int tokens = st.countTokens();
    Int32 count;
    st->CountTokens(&count);
    if (count != 3) {
        // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String key;
    st->GetNextToken(&key);
    if (!key.Equals("LoudnessEnhancer")) {
        // throw new IllegalArgumentException(
        //         "invalid settings for LoudnessEnhancer: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
        st->GetNextToken(&key);
        if (!key.Equals("targetGainmB")) {
            // throw new IllegalArgumentException("invalid key name: " + key);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        st->GetNextToken(&key);
        StringUtils::Parse(key, &mTargetGainmB);
    //  } catch (NumberFormatException nfe) {
    //     throw new IllegalArgumentException("invalid value for key: " + key);
    // }

    return NOERROR;
}

ECode CLoudnessEnhancerSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("LoudnessEnhancer");
    sb += ";targetGainmB=";
    sb += mTargetGainmB;

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
