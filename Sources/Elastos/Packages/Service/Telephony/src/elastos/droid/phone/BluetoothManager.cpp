
#include "elastos/droid/phone/BluetoothManager.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(BluetoothManager, Object, IBluetoothManager)

ECode BluetoothManager::IsBluetoothHeadsetAudioOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnectedOrPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::ShowBluetoothIndication(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::UpdateBluetoothIndication()
{
    return NOERROR;
}

ECode BluetoothManager::AddBluetoothIndicatorListener(
    /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener)
{
    return NOERROR;
}

ECode BluetoothManager::RemoveBluetoothIndicatorListener(
    /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener)
{
    return NOERROR;
}

ECode BluetoothManager::ConnectBluetoothAudio()
{
    return NOERROR;
}

ECode BluetoothManager::DisconnectBluetoothAudio()
{
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos