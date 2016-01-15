
#include "elastos/droid/hardware/hdmi/HdmiPortInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiPortInfo, Object, IHdmiPortInfo, IParcelable)

HdmiPortInfo::HdmiPortInfo()
    : mId(0)
    , mType(0)
    , mAddress(0)
    , mCecSupported(FALSE)
    , mArcSupported(FALSE)
    , mMhlSupported(FALSE)
{
}

ECode HdmiPortInfo::constructor()
{
    return NOERROR;
}

ECode HdmiPortInfo::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 type,
    /* [in] */ Int32 address,
    /* [in] */ Boolean cec,
    /* [in] */ Boolean mhl,
    /* [in] */ Boolean arc)
{
    mId = id;
    mType = type;
    mAddress = address;
    mCecSupported = cec;
    mArcSupported = arc;
    mMhlSupported = mhl;
    return NOERROR;
}

ECode HdmiPortInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode HdmiPortInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode HdmiPortInfo::GetAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address);

    *address = mAddress;
    return NOERROR;
}

ECode HdmiPortInfo::IsCecSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCecSupported;
    return NOERROR;
}

ECode HdmiPortInfo::IsMhlSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMhlSupported;
    return NOERROR;
}

ECode HdmiPortInfo::IsArcSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mArcSupported;
    return NOERROR;
}

ECode HdmiPortInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mId);
    source->ReadInt32(&mType);
    source->ReadInt32(&mAddress);
    source->ReadBoolean(&mCecSupported);
    source->ReadBoolean(&mArcSupported);
    source->ReadBoolean(&mMhlSupported);

    return NOERROR;
}

ECode HdmiPortInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mId);
    dest->WriteInt32(mType);
    dest->WriteInt32(mAddress);
    dest->WriteBoolean(mCecSupported);
    dest->WriteBoolean(mArcSupported);
    dest->WriteBoolean(mMhlSupported);

    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
