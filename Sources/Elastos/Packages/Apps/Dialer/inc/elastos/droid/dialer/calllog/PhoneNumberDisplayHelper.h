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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__

#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class PhoneNumberDisplayHelper : public Object
{
public:
    PhoneNumberDisplayHelper(
        /* [in] */ IResources* resources);

    PhoneNumberDisplayHelper(
        /* [in] */ PhoneNumberUtilsWrapper* phoneNumberUtils,
        /* [in] */ IResources* resources);

    CARAPI_(AutoPtr<ICharSequence>) GetDisplayName(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);

    /**
     * Returns the string to display for the given phone number.
     *
     * @param number the number to display
     * @param formattedNumber the formatted number if available, may be null
     */
    CARAPI_(AutoPtr<ICharSequence>) GetDisplayNumber(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation,
        /* [in] */ ICharSequence* formattedNumber);

private:
    AutoPtr<PhoneNumberUtilsWrapper> mPhoneNumberUtils;
    AutoPtr<IResources> mResources;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__
