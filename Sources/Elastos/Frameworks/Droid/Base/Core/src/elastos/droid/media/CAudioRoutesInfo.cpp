
#include "elastos/droid/media/CAudioRoutesInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CAudioRoutesInfo::MAIN_SPEAKER = 0;
const Int32 CAudioRoutesInfo::MAIN_HEADSET = 1 << 0;
const Int32 CAudioRoutesInfo::MAIN_HEADPHONES = 1 << 1;
const Int32 CAudioRoutesInfo::MAIN_DOCK_SPEAKERS = 1 << 2;
const Int32 CAudioRoutesInfo::MAIN_HDMI = 1 << 3;

CAR_INTERFACE_IMPL_2(CAudioRoutesInfo, Object, IAudioRoutesInfo, IParcelable)

CAR_OBJECT_IMPL(CAudioRoutesInfo)

CAudioRoutesInfo::CAudioRoutesInfo()
    : mMainType(MAIN_SPEAKER)
{}

CAudioRoutesInfo::~CAudioRoutesInfo()
{}

ECode CAudioRoutesInfo::constructor()
{
    return NOERROR;
}

ECode CAudioRoutesInfo::constructor(
    /* [in] */ IAudioRoutesInfo* o)
{
    VALIDATE_NOT_NULL(o);
    o->GetBluetoothName((ICharSequence**)&mBluetoothName);
    o->GetMainType(&mMainType);
    return NOERROR;
}

ECode CAudioRoutesInfo::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    src->ReadInt32(&mMainType);
    AutoPtr<IInterface> obj;
    src->ReadInterfacePtr((Handle32*)&obj);
    mBluetoothName = ICharSequence::Probe(obj);
    return NOERROR;
}

ECode CAudioRoutesInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mMainType);
    dest->WriteInterfacePtr(mBluetoothName);
    return NOERROR;
}

ECode CAudioRoutesInfo::GetBluetoothName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBluetoothName;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAudioRoutesInfo::SetBluetoothName(
    /* [in] */ ICharSequence* bluetoothName)
{
    mBluetoothName = bluetoothName;
    return NOERROR;
}

ECode CAudioRoutesInfo::GetMainType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMainType;
    return NOERROR;
}

ECode CAudioRoutesInfo::SetMainType(
    /* [in] */ Int32 mainType)
{
    mMainType = mainType;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
