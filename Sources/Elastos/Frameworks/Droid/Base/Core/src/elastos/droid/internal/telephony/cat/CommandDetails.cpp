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
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                CommandDetails::InnerParcelableCreator
////=====================================================================
//CommandDetails::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ CommandDetails* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode CommandDetails::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ CommandDetails** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CommandDetails(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode CommandDetails::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ CommandDetails[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CommandDetails[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                            CommandDetails
//=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<CommandDetails> > CommandDetails::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL(CommandDetails, ValueObject, IParcelable);

CommandDetails::CommandDetails()
{
}

ECode CommandDetails::constructor()
{
    return NOERROR;
}

ECode CommandDetails::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ComprehensionTlvTag_COMMAND_DETAILS;
    return NOERROR;
}

ECode CommandDetails::CompareTo(
    /* [in] */ CommandDetails* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mCompRequired == other->mCompRequired &&
            mCommandNumber == other->mCommandNumber &&
            mCommandQualifier == other->mCommandQualifier &&
            mTypeOfCommand == other->mTypeOfCommand);
    return NOERROR;
}

ECode CommandDetails::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 compRequired = 0;
    in->ReadInt32(&compRequired);
    mCompRequired = compRequired != 0;
    in->ReadInt32(&mCommandNumber);
    in->ReadInt32(&mTypeOfCommand);
    in->ReadInt32(&mCommandQualifier);
    return NOERROR;
}

ECode CommandDetails::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCompRequired ? 1 : 0);
    dest->WriteInt32(mCommandNumber);
    dest->WriteInt32(mTypeOfCommand);
    dest->WriteInt32(mCommandQualifier);
    return NOERROR;
}

//ECode CommandDetails::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode CommandDetails::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CmdDetails: compRequired=") + StringUtils::ToString(mCompRequired) +
            String(" commandNumber=") + StringUtils::ToString(mCommandNumber) +
            String(" typeOfCommand=") + StringUtils::ToString(mTypeOfCommand) +
            String(" commandQualifier=") + StringUtils::ToString(mCommandQualifier);
    return NOERROR;
}

//=====================================================================
//                           DeviceIdentities
//=====================================================================
ECode DeviceIdentities::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ComprehensionTlvTag_DEVICE_IDENTITIES;
    return NOERROR;
}

//=====================================================================
//                                IconId
//=====================================================================
ECode IconId::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ComprehensionTlvTag_ICON_ID;
    return NOERROR;
}

//=====================================================================
//                             ItemsIconId
//=====================================================================
ECode ItemsIconId::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ComprehensionTlvTag_ITEM_ICON_ID_LIST;
    return NOERROR;
}

//=====================================================================
//                          ActivateDescriptor
//=====================================================================
ECode ActivateDescriptor::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ComprehensionTlvTag_ACTIVATE_DESCRIPTOR;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
