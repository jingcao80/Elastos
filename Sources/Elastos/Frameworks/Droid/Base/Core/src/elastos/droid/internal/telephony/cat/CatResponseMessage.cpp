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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatResponseMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                          CatResponseMessage
//=====================================================================
CAR_INTERFACE_IMPL(CatResponseMessage, Object, ICatResponseMessage);

CatResponseMessage::CatResponseMessage()
    : mCmdDet(NULL)
    , mResCode(ResultCode_OK)
    , mUsersMenuSelection(0)
    , mUsersInput(String(NULL))
    , mUsersYesNoSelection(FALSE)
    , mUsersConfirm(FALSE)
    , mIncludeAdditionalInfo(FALSE)
    , mAdditionalInfo(0)
    , mEventValue(-1)
{
}

ECode CatResponseMessage::constructor(
    /* [in] */ ICatCmdMessage* cmdMsg)
{
    assert(0 && "TODO");
    // cmdMsg->GetCmdType(&mCmdDet);
    return NOERROR;
}

ECode CatResponseMessage::SetResultCode(
    /* [in] */ ResultCode resCode)
{
    mResCode = resCode;
    return NOERROR;
}

ECode CatResponseMessage::SetMenuSelection(
    /* [in] */ Int32 selection)
{
    mUsersMenuSelection = selection;
    return NOERROR;
}

ECode CatResponseMessage::SetInput(
    /* [in] */ const String& input)
{
    mUsersInput = input;
    return NOERROR;
}

ECode CatResponseMessage::SetEventDownload(
    /* [in] */ Int32 event,
    /* [in] */ ArrayOf<Byte>* addedInfo)
{
    mEventValue = event;
    mAddedInfo = addedInfo;
    return NOERROR;
}

ECode CatResponseMessage::SetYesNo(
    /* [in] */ Boolean yesNo)
{
    mUsersYesNoSelection = yesNo;
    return NOERROR;
}

ECode CatResponseMessage::SetConfirmation(
    /* [in] */ Boolean confirm)
{
    mUsersConfirm = confirm;
    return NOERROR;
}

ECode CatResponseMessage::SetAdditionalInfo(
    /* [in] */ Int32 info)
{
    mIncludeAdditionalInfo = TRUE;
    mAdditionalInfo = info;
    return NOERROR;
}

ECode CatResponseMessage::GetCmdDetails(
    /* [out] */ CommandDetails** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCmdDet;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
