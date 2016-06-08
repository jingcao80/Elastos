
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

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

ECode CommandDetails::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // compRequired = in.readInt() != 0;
    // commandNumber = in.readInt();
    // typeOfCommand = in.readInt();
    // commandQualifier = in.readInt();
    return NOERROR;
}

ECode CommandDetails::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ComprehensionTlvTag.COMMAND_DETAILS;
    assert(0);
    return NOERROR;
}

ECode CommandDetails::CompareTo(
    /* [in] */ CommandDetails* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (this.compRequired == other.compRequired &&
    //         this.commandNumber == other.commandNumber &&
    //         this.commandQualifier == other.commandQualifier &&
    //         this.typeOfCommand == other.typeOfCommand);
    assert(0);
    return NOERROR;
}

ECode CommandDetails::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    // ==================before translated======================
    // dest.writeInt(compRequired ? 1 : 0);
    // dest.writeInt(commandNumber);
    // dest.writeInt(typeOfCommand);
    // dest.writeInt(commandQualifier);
    assert(0);
    return NOERROR;
}

ECode CommandDetails::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
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
    // ==================before translated======================
    // return "CmdDetails: compRequired=" + compRequired +
    //         " commandNumber=" + commandNumber +
    //         " typeOfCommand=" + typeOfCommand +
    //         " commandQualifier=" + commandQualifier;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                           DeviceIdentities
//=====================================================================
ECode DeviceIdentities::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ComprehensionTlvTag.DEVICE_IDENTITIES;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                                IconId
//=====================================================================
ECode IconId::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ComprehensionTlvTag.ICON_ID;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                             ItemsIconId
//=====================================================================
ECode ItemsIconId::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ComprehensionTlvTag.ITEM_ICON_ID_LIST;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          ActivateDescriptor
//=====================================================================
ECode ActivateDescriptor::GetTag(
    /* [out] */ ComprehensionTlvTag* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ComprehensionTlvTag.ACTIVATE_DESCRIPTOR;
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
