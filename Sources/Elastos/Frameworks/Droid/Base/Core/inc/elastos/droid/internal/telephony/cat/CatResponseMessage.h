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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class CatResponseMessage
    : public Object
    , public ICatResponseMessage
{
public:
    CAR_INTERFACE_DECL();

    CatResponseMessage();

    CARAPI constructor(
        /* [in] */ ICatCmdMessage* cmdMsg);

    virtual CARAPI SetResultCode(
        /* [in] */ ResultCode resCode);

    virtual CARAPI SetMenuSelection(
        /* [in] */ Int32 selection);

    virtual CARAPI SetInput(
        /* [in] */ const String& input);

    virtual CARAPI SetEventDownload(
        /* [in] */ Int32 event,
        /* [in] */ ArrayOf<Byte>* addedInfo);

    virtual CARAPI SetYesNo(
        /* [in] */ Boolean yesNo);

    virtual CARAPI SetConfirmation(
        /* [in] */ Boolean confirm);

    virtual CARAPI SetAdditionalInfo(
        /* [in] */ Int32 info);

    virtual CARAPI GetCmdDetails(
        /* [out] */ CommandDetails** result);

public:
    AutoPtr<CommandDetails> mCmdDet;
    ResultCode mResCode;
    Int32 mUsersMenuSelection;
    String mUsersInput;
    Boolean mUsersYesNoSelection;
    Boolean mUsersConfirm;
    Boolean mIncludeAdditionalInfo;
    Int32 mAdditionalInfo;
    Int32 mEventValue;
    AutoPtr<ArrayOf<Byte> > mAddedInfo;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__
