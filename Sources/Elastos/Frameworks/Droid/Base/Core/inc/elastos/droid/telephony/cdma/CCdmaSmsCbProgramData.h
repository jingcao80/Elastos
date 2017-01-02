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

#ifndef __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMDATA_H__
#define __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMDATA_H__

#include "_Elastos_Droid_Telephony_Cdma_CCdmaSmsCbProgramData.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaSmsCbProgramData)
    , public Object
    , public ICdmaSmsCbProgramData
    , public IParcelable
{
public:
    CCdmaSmsCbProgramData();

    virtual ~CCdmaSmsCbProgramData();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 operation,
        /* [in] */ Int32 category,
        /* [in] */ Int32 language,
        /* [in] */ Int32 maxMessages,
        /* [in] */ Int32 alertOption,
        /* [in] */ const String& categoryName);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetOperation(
        /* [out] */ Int32* operation);

    CARAPI GetCategory(
        /* [out] */ Int32* category);

    CARAPI GetLanguage(
        /* [out] */ Int32* language);

    CARAPI GetMaxMessages(
        /* [out] */ Int32* maxmessages);

    CARAPI GetAlertOption(
        /* [out] */ Int32* alertoption);

    CARAPI GetCategoryName(
        /* [out] */ String* categoryname);

    CARAPI SetOperation(
        /* [in] */ Int32 operation);

    CARAPI SetCategory(
        /* [in] */ Int32 category);

    CARAPI SetLanguage(
        /* [in] */ Int32 language);

    CARAPI SetMaxMessages(
        /* [in] */ Int32 maxmessages);

    CARAPI SetAlertOption(
        /* [in] */ Int32 alertoption);

    CARAPI SetCategoryName(
        /* [in] */ const String& categoryname);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    /** Service category operation (add/delete/clear). */
    Int32 mOperation;

    /** Service category to modify. */
    Int32 mCategory;

    /** Language used for service category name (defined in BearerData.LANGUAGE_*). */
    Int32 mLanguage;

    /** Maximum number of messages to store for this service category. */
    Int32 mMaxMessages;

    /** Service category alert option. */
    Int32 mAlertOption;

    /** Name of service category. */
    String mCategoryName;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDMA_CCDMASMSCBPROGRAMDATA_H__
