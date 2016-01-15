
#include "elastos/droid/wifi/CWifiActivityEnergyInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiActivityEnergyInfo, Object, IParcelable, IWifiActivityEnergyInfo)

CAR_OBJECT_IMPL(CWifiActivityEnergyInfo)

ECode CWifiActivityEnergyInfo::constructor(
    /* [in] */ Int32 stackState,
    /* [in] */ Int32 txTime,
    /* [in] */ Int32 rxTime,
    /* [in] */ Int32 idleTime,
    /* [in] */ Int32 energyUsed)
{
    mStackState = stackState;
    mControllerTxTimeMs = txTime;
    mControllerRxTimeMs = rxTime;
    mControllerIdleTimeMs = idleTime;
    mControllerEnergyUsed = energyUsed;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTimestamp);
    return NOERROR;
}

/**
 * @return bt stack reported state
 */
ECode CWifiActivityEnergyInfo::GetStackState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStackState;
    return NOERROR;
}

/**
 * @return tx time in ms
 */
ECode CWifiActivityEnergyInfo::GetControllerTxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerTxTimeMs;
    return NOERROR;
}

/**
 * @return rx time in ms
 */
ECode CWifiActivityEnergyInfo::GetControllerRxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerRxTimeMs;
    return NOERROR;
}

/**
 * @return idle time in ms
 */
ECode CWifiActivityEnergyInfo::GetControllerIdleTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerIdleTimeMs;
    return NOERROR;
}

/**
 * product of current(mA), voltage(V) and time(ms)
 * @return energy used
 */
ECode CWifiActivityEnergyInfo::GetControllerEnergyUsed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerEnergyUsed;
    return NOERROR;
}

/**
 * @return timestamp(wall clock) of record creation
 */
ECode CWifiActivityEnergyInfo::GetTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTimestamp;
    return NOERROR;
}

/**
 * @return if the record is valid
 */
ECode CWifiActivityEnergyInfo::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 txTimeMillis, rxTimeMillis, idleTimeMillis;
    *result = ((GetControllerTxTimeMillis(&txTimeMillis), txTimeMillis !=0) ||
                (GetControllerRxTimeMillis(&rxTimeMillis), rxTimeMillis !=0) ||
                (GetControllerIdleTimeMillis(&idleTimeMillis), idleTimeMillis !=0));
    return NOERROR;
}

//@Override
ECode CWifiActivityEnergyInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("WifiActivityEnergyInfo{");
    sb.Append(" mTimestamp=");
    sb.Append(mTimestamp);
    sb.Append(" mStackState=");
    sb.Append(mStackState);
    sb.Append(" mControllerTxTimeMs=");
    sb.Append(mControllerTxTimeMs);
    sb.Append(" mControllerRxTimeMs=");
    sb.Append(mControllerRxTimeMs);
    sb.Append(" mControllerIdleTimeMs=");
    sb.Append(mControllerIdleTimeMs);
    sb.Append(" mControllerEnergyUsed=");
    sb.Append(mControllerEnergyUsed);
    sb.Append(" }");
    return ToString(str);
}

ECode CWifiActivityEnergyInfo::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    out->WriteInt32(mStackState);
    out->WriteInt32(mControllerTxTimeMs);
    out->WriteInt32(mControllerRxTimeMs);
    out->WriteInt32(mControllerIdleTimeMs);
    out->WriteInt32(mControllerEnergyUsed);
    return NOERROR;
}

ECode CWifiActivityEnergyInfo::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

ECode CWifiActivityEnergyInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiActivityEnergyInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
