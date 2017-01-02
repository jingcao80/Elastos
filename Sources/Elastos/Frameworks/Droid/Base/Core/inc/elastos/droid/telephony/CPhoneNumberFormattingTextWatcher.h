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

#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Telephony_CPhoneNumberFormattingTextWatcher.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneNumberFormattingTextWatcher)
    , public Object
    , public IPhoneNumberFormattingTextWatcher
    , public ITextWatcher
    , public INoCopySpan
{
public:
    CPhoneNumberFormattingTextWatcher();

    virtual ~CPhoneNumberFormattingTextWatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& countryCode);

    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

private:

    CARAPI_(String) Reformat(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 cursor);

    CARAPI_(String) GetFormattedNumber(
        /* [in] */ Char32 lastNonSeparator,
        /* [in] */ Boolean hasCursor);

    CARAPI_(void) StopFormatting();

    CARAPI_(Boolean) HasSeparator(
        /* [in] */ ICharSequence* s,
        /* [in] */ const Int32 start,
        /* [in] */ const Int32 count);

    /**
     * Indicates the change was caused by ourselves.
     */
    Boolean mSelfChange;

    /**
     * Indicates the formatting has been stopped.
     */
    Boolean mStopFormatting;

// TODO: Need AsYouTypeFormatter
    //AutoPtr<AsYouTypeFormatter> mFormatter;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
