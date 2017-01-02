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

#ifndef __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMRESULTS_H__
#define __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMRESULTS_H__

#include "_Elastos_Droid_Telephony_Cdma_CCdmaSmsCbProgramResults.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaSmsCbProgramResults)
    , public Object
    , public ICdmaSmsCbProgramResults
    , public IParcelable
{
public:
    CCdmaSmsCbProgramResults();

    virtual ~CCdmaSmsCbProgramResults();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 category,
        /* [in] */ Int32 language,
        /* [in] */ Int32 categoryResult);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetCategory(
        /* [out] */ Int32* res);

    CARAPI GetLanguage(
        /* [out] */ Int32* res);

    CARAPI GetCategoryResult(
        /* [out] */ Int32* res);

    CARAPI SetCategory(
        /* [in] */ Int32 category);

    CARAPI SetLanguage(
        /* [in] */ Int32 language);

    CARAPI SetCategoryResult(
        /* [in] */ Int32 categoryresult);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    /** Service category to modify. */
    Int32 mCategory;

    /** Language used for service category name (defined in BearerData.LANGUAGE_*). */
    Int32 mLanguage;

    /** Result of service category programming for this category. */
    Int32 mCategoryResult;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMRESULTS_H__
