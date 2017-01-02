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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILSHELPER_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternUtilsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternUtilsHelper)
    , public Singleton
    , public ILockPatternUtilsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /*
     * Returns the maximum length of a sequential characters.  A sequence is defined as
     * monotonically increasing characters with a constant interval or the same character repeated.
     *
     * For example:
     * maxLengthSequence("1234") == 4
     * maxLengthSequence("1234abc") == 4
     * maxLengthSequence("aabc") == 3
     * maxLengthSequence("qwertyuio") == 1
     * maxLengthSequence("@ABC") == 3
     * maxLengthSequence(";;;;") == 4 (anything that repeats)
     * maxLengthSequence(":;<=>") == 1  (ordered, but not composed of alphas or digits)
     *
     * @param string the pass
     * @return the number of sequential letters or digits
     */
    CARAPI MaxLengthSequence(
            /* [in] */ const String& string,
        /* [out] */ Int32* len);

    /**
     * Gets whether the device is encrypted.
     *
     * @return Whether the device is encrypted.
     */
    CARAPI IsDeviceEncrypted(
        /* [out] */ Boolean* isDeviceEncrypted);

    /**
     * Determine if the device supports encryption, even if it's set to default. This
     * differs from isDeviceEncrypted() in that it returns true even if the device is
     * encrypted with the default password.
     * @return true if device encryption is enabled
     */
    CARAPI IsDeviceEncryptionEnabled(
        /* [out] */ Boolean* isDeviceEncryptionEnabled);

    /**
     * Serialize a pattern.
     * @param pattern The pattern.
     * @return The pattern in string form.
     */
    CARAPI PatternToString(
        /* [in] */ IList* pattern,
        /* [in] */ Int32 patternGridSize,
        /* [out] */ String* str);

    CARAPI IsSafeModeEnabled(
        /* [out] */ Boolean* result);
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILSHELPER_H__
