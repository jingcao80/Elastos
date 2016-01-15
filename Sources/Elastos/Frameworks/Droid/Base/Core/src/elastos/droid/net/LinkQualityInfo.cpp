
#include "elastos/droid/net/LinkQualityInfo.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Net::IConnectivityManager;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(LinkQualityInfo, Object, ILinkQualityInfo, IParcelable)

const Int32 LinkQualityInfo::sOBJECT_TYPE_LINK_QUALITY_INFO = 1;
const Int32 LinkQualityInfo::sOBJECT_TYPE_WIFI_LINK_QUALITY_INFO = 2;
const Int32 LinkQualityInfo::sOBJECT_TYPE_MOBILE_LINK_QUALITY_INFO = 3;

LinkQualityInfo::LinkQualityInfo()
    : mNetworkType(IConnectivityManager::TYPE_NONE)
    , mNormalizedSignalStrength(UNKNOWN_INT)
    , mPacketCount(UNKNOWN_LONG)
    , mPacketErrorCount(UNKNOWN_LONG)
    , mTheoreticalTxBandwidth(UNKNOWN_INT)
    , mTheoreticalRxBandwidth(UNKNOWN_INT)
    , mTheoreticalLatency(UNKNOWN_INT)
    , mLastDataSampleTime(UNKNOWN_LONG)
    , mDataSampleDuration(UNKNOWN_INT)
{}

ECode LinkQualityInfo::constructor()
{
    return NOERROR;
}

ECode LinkQualityInfo::GetNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetworkType;
    return NOERROR;
}

ECode LinkQualityInfo::SetNetworkType(
    /* [in] */ Int32 networkType)
{
    mNetworkType = networkType;
    return NOERROR;
}

ECode LinkQualityInfo::GetNormalizedSignalStrength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNormalizedSignalStrength;
    return NOERROR;
}

ECode LinkQualityInfo::SetNormalizedSignalStrength(
    /* [in] */ Int32 normalizedSignalStrength)
{
    mNormalizedSignalStrength = normalizedSignalStrength;
    return NOERROR;
}

ECode LinkQualityInfo::GetPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPacketCount;
    return NOERROR;
}

ECode LinkQualityInfo::SetPacketCount(
    /* [in] */ Int64 packetCount)
{
    mPacketCount = packetCount;
    return NOERROR;
}

ECode LinkQualityInfo::GetPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPacketErrorCount;
    return NOERROR;
}

ECode LinkQualityInfo::SetPacketErrorCount(
    /* [in] */ Int64 packetErrorCount)
{
    mPacketErrorCount = packetErrorCount;
    return NOERROR;
}

ECode LinkQualityInfo::GetTheoreticalTxBandwidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTheoreticalTxBandwidth;
    return NOERROR;
}

ECode LinkQualityInfo::SetTheoreticalTxBandwidth(
    /* [in] */ Int32 theoreticalTxBandwidth)
{
    mTheoreticalTxBandwidth = theoreticalTxBandwidth;
    return NOERROR;
}

ECode LinkQualityInfo::GetTheoreticalRxBandwidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTheoreticalRxBandwidth;
    return NOERROR;
}

ECode LinkQualityInfo::SetTheoreticalRxBandwidth(
    /* [in] */ Int32 theoreticalRxBandwidth)
{
    mTheoreticalRxBandwidth = theoreticalRxBandwidth;
    return NOERROR;
}

ECode LinkQualityInfo::GetTheoreticalLatency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTheoreticalLatency;
    return NOERROR;
}

ECode LinkQualityInfo::SetTheoreticalLatency(
    /* [in] */ Int32 theoreticalLatency)
{
    mTheoreticalLatency = theoreticalLatency;
    return NOERROR;
}

ECode LinkQualityInfo::GetLastDataSampleTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLastDataSampleTime;
    return NOERROR;
}

ECode LinkQualityInfo::SetLastDataSampleTime(
    /* [in] */ Int64 lastDataSampleTime)
{
    mLastDataSampleTime = lastDataSampleTime;
    return NOERROR;
}

ECode LinkQualityInfo::GetDataSampleDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDataSampleDuration;
    return NOERROR;
}

ECode LinkQualityInfo::SetDataSampleDuration(
    /* [in] */ Int32 dataSampleDuration)
{
    mDataSampleDuration = dataSampleDuration;
    return NOERROR;
}

ECode LinkQualityInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mNetworkType);
    parcel->ReadInt32(&mNormalizedSignalStrength);
    parcel->ReadInt64(&mPacketCount);
    parcel->ReadInt64(&mPacketErrorCount);
    parcel->ReadInt32(&mTheoreticalTxBandwidth);
    parcel->ReadInt32(&mTheoreticalRxBandwidth);
    parcel->ReadInt32(&mTheoreticalLatency);
    parcel->ReadInt64(&mLastDataSampleTime);
    parcel->ReadInt32(&mDataSampleDuration);
    return NOERROR;
}

ECode LinkQualityInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNetworkType);
    dest->WriteInt32(mNormalizedSignalStrength);
    dest->WriteInt64(mPacketCount);
    dest->WriteInt64(mPacketErrorCount);
    dest->WriteInt32(mTheoreticalTxBandwidth);
    dest->WriteInt32(mTheoreticalRxBandwidth);
    dest->WriteInt32(mTheoreticalLatency);
    dest->WriteInt64(mLastDataSampleTime);
    dest->WriteInt32(mDataSampleDuration);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
