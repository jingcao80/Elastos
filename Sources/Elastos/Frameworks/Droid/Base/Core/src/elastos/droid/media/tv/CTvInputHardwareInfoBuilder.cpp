#include "elastos/droid/media/tv/CTvInputHardwareInfoBuilder.h"
#include "elastos/droid/media/tv/CTvInputHardwareInfo.h"

using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvInputHardwareInfoBuilder, Object, ITvInputHardwareInfoBuilder)

CAR_OBJECT_IMPL(CTvInputHardwareInfoBuilder)

CTvInputHardwareInfoBuilder::CTvInputHardwareInfoBuilder()
    : mAudioType(0)
{
}

CTvInputHardwareInfoBuilder::~CTvInputHardwareInfoBuilder()
{
}

ECode CTvInputHardwareInfoBuilder::constructor()
{
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::DeviceId(
    /* [in] */ Int32 deviceId)
{
    CInteger32::New(deviceId, (IInteger32**)&mDeviceId);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::Type(
    /* [in] */ Int32 type)
{
    CInteger32::New(type, (IInteger32**)&mType);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::AudioType(
    /* [in] */ Int32 audioType)
{
    mAudioType = audioType;
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::AudioAddress(
    /* [in] */ const String& audioAddress)
{
    mAudioAddress = audioAddress;
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::HdmiPortId(
    /* [in] */ Int32 hdmiPortId)
{
    CInteger32::New(hdmiPortId, (IInteger32**)&mHdmiPortId);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::Build(
    /* [out] */ ITvInputHardwareInfo ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mDeviceId == NULL || mType == NULL) {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Int32 type;
    mType->GetValue(&type);
    if ((type == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI && mHdmiPortId == NULL) ||
            (type != ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI && mHdmiPortId != NULL)) {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<CTvInputHardwareInfo> info;
    CTvInputHardwareInfo::NewByFriend((CTvInputHardwareInfo**)&info);
    mDeviceId->GetValue(&info->mDeviceId);
    mType->GetValue(&info->mType);
    info->mAudioType = mAudioType;
    if (info->mAudioType != IAudioManager::DEVICE_NONE) {
        info->mAudioAddress = mAudioAddress;
    }
    if (mHdmiPortId != NULL) {
        mHdmiPortId->GetValue(&info->mHdmiPortId);
    }
    *result = ITvInputHardwareInfo::Probe(info);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
