
#include "elastos/droid/wifi/CWifiInfo.h"
#include "elastos/droid/wifi/CScanResultHelper.h"
#include "elastos/droid/wifi/CSupplicantState.h"
#include "elastos/droid/wifi/CWifiSsidHelper.h"
#include "elastos/droid/wifi/CWifiSsid.h"
#include "elastos/droid/net/CNetworkUtils.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::CNetworkUtils;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_IDLE;
using Elastos::Droid::Net::NetworkInfoDetailedState_SCANNING;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_AUTHENTICATING;
using Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {

CWifiInfo::StaticInitializer::StaticInitializer()
{
    sStateMap[SupplicantState_DISCONNECTED] = NetworkInfoDetailedState_DISCONNECTED;
    sStateMap[SupplicantState_INTERFACE_DISABLED] = NetworkInfoDetailedState_DISCONNECTED;
    sStateMap[SupplicantState_INACTIVE] = NetworkInfoDetailedState_IDLE;
    sStateMap[SupplicantState_SCANNING] = NetworkInfoDetailedState_SCANNING;
    sStateMap[SupplicantState_AUTHENTICATING] = NetworkInfoDetailedState_CONNECTING;
    sStateMap[SupplicantState_ASSOCIATING] = NetworkInfoDetailedState_CONNECTING;
    sStateMap[SupplicantState_ASSOCIATED] = NetworkInfoDetailedState_CONNECTING;
    sStateMap[SupplicantState_FOUR_WAY_HANDSHAKE] = NetworkInfoDetailedState_AUTHENTICATING;
    sStateMap[SupplicantState_GROUP_HANDSHAKE] = NetworkInfoDetailedState_AUTHENTICATING;
    sStateMap[SupplicantState_COMPLETED] = NetworkInfoDetailedState_OBTAINING_IPADDR;
    sStateMap[SupplicantState_DORMANT] = NetworkInfoDetailedState_DISCONNECTED;
    sStateMap[SupplicantState_UNINITIALIZED] = NetworkInfoDetailedState_IDLE;
    sStateMap[SupplicantState_INVALID] = NetworkInfoDetailedState_FAILED;
}

String CWifiInfo::TAG("WifiInfo");
HashMap<SupplicantState, NetworkInfoDetailedState> CWifiInfo::sStateMap;
String CWifiInfo::LINK_SPEED_UNITS("Mbps");
const CWifiInfo::StaticInitializer CWifiInfo::sInitializer;

CAR_INTERFACE_IMPL_2(CWifiInfo, Object, IWifiInfo, IParcelable)

CAR_OBJECT_IMPL(CWifiInfo)

CWifiInfo::CWifiInfo()
    : mNetworkId(0)
    , mRssi(0)
    , mLinkSpeed(0)
    , mMeteredHint(FALSE)
{}

ECode CWifiInfo::constructor()
{
    mWifiSsid = NULL;
    mBSSID = NULL;
    mNetworkId = -1;
    mSupplicantState = SupplicantState_UNINITIALIZED;
    mRssi = INVALID_RSSI;
    mLinkSpeed = -1;
    mFrequency = -1;
    return NOERROR;
}

ECode CWifiInfo::constructor(
    /* [in] */ Boolean fake)
{
    mNetworkId = (-1);
    mRssi = (-9999);
    mLinkSpeed = (-1);
    mMeteredHint = (FALSE);

    AutoPtr<IWifiSsidHelper> helper;
    CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
    helper->CreateFromAsciiEncoded(String(""), (IWifiSsid**)&mWifiSsid);
    mBSSID = String("");
    mSupplicantState = SupplicantState_UNINITIALIZED;
    return NOERROR;
}

ECode CWifiInfo::constructor(
    /* [in] */ IWifiInfo* source)
{
    if (source != NULL) {
        AutoPtr<ISupplicantState> supplicantState;
        source->GetSupplicantState((ISupplicantState**)&supplicantState);
        supplicantState->Get(&mSupplicantState);

        source->GetBSSID(&mBSSID);
        source->GetWifiSsid((IWifiSsid**)&mWifiSsid);
        source->GetNetworkId(&mNetworkId);
        source->GetRssi(&mRssi);
        source->GetLinkSpeed(&mLinkSpeed);
        source->GetFrequency(&mFrequency);
        Int32 ipAddres;
        source->GetIpAddress(&ipAddres);
        if (ipAddres != 0) {
            NetworkUtils::IntToInetAddress(ipAddres, (IInetAddress**)&mIpAddress);
        }
        source->GetMacAddress(&mMacAddress);
        source->GetMeteredHint(&mMeteredHint);
        source->GetTxBad(&mTxBad);
        source->GetTxRetries(&mTxRetries);
        source->GetTxSuccess(&mTxSuccess);
        source->GetRxSuccess(&mRxSuccess);
        source->GetTxBadRate(&mTxBadRate);
        source->GetTxRetriesRate(&mTxRetriesRate);
        source->GetTxSuccessRate(&mTxSuccessRate);
        source->GetRxSuccessRate(&mRxSuccessRate);
        source->GetScore(&mScore);
        source->GetBadRssiCount(&mBadRssiCount);
        source->GetLowRssiCount(&mLowRssiCount);
        source->GetLinkStuckCount(&mLinkStuckCount);
    }
    return NOERROR;
}

ECode CWifiInfo::GetTxBad(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxBad;
    return NOERROR;
}

ECode CWifiInfo::SetTxBad(
    /* [in] */ Int64 txBad)
{
    mTxBad = txBad;
    return NOERROR;
}

ECode CWifiInfo::GetTxRetries(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxRetries;
    return NOERROR;
}

ECode CWifiInfo::SetTxRetries(
    /* [in] */ Int64 txRetries)
{
    mTxRetries = txRetries;
    return NOERROR;
}

ECode CWifiInfo::GetTxSuccess(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxSuccess;
    return NOERROR;
}

ECode CWifiInfo::SetTxSuccess(
    /* [in] */ Int64 txSuccess)
{
    mTxSuccess = txSuccess;
    return NOERROR;
}

ECode CWifiInfo::GetRxSuccess(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxSuccess;
    return NOERROR;
}

ECode CWifiInfo::SetRxSuccess(
    /* [in] */ Int64 rxSuccess)
{
    mRxSuccess = rxSuccess;
    return NOERROR;
}

ECode CWifiInfo::GetTxBadRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxBadRate;
    return NOERROR;
}

ECode CWifiInfo::SetTxBadRate(
    /* [in] */ Double txBadRate)
{
    mTxBadRate = txBadRate;
    return NOERROR;
}

ECode CWifiInfo::GetTxRetriesRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxRetriesRate;
    return NOERROR;
}

ECode CWifiInfo::SetTxRetriesRate(
    /* [in] */ Double txRetriesRate)
{
    mTxRetriesRate = txRetriesRate;
    return NOERROR;
}

ECode CWifiInfo::GetTxSuccessRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxSuccessRate;
    return NOERROR;
}

ECode CWifiInfo::SetTxSuccessRate(
    /* [in] */ Double txSuccessRate)
{
    mTxSuccessRate = txSuccessRate;
    return NOERROR;
}

ECode CWifiInfo::GetRxSuccessRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxSuccessRate;
    return NOERROR;
}

ECode CWifiInfo::SetRxSuccessRate(
    /* [in] */ Double rxSuccessRate)
{
    mRxSuccessRate = rxSuccessRate;
    return NOERROR;
}

ECode CWifiInfo::GetBadRssiCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBadRssiCount;
    return NOERROR;
}

ECode CWifiInfo::SetBadRssiCount(
    /* [in] */ Int32 badRssiCount)
{
    mBadRssiCount = badRssiCount;
    return NOERROR;
}

ECode CWifiInfo::GetLinkStuckCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLinkStuckCount;
    return NOERROR;
}

ECode CWifiInfo::SetLinkStuckCount(
    /* [in] */ Int32 linkStuckCount)
{
    mLinkStuckCount = linkStuckCount;
    return NOERROR;
}

ECode CWifiInfo::GetLowRssiCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLowRssiCount;
    return NOERROR;
}

ECode CWifiInfo::SetLowRssiCount(
    /* [in] */ Int32 lowRssiCount)
{
    mLowRssiCount = lowRssiCount;
    return NOERROR;
}

ECode CWifiInfo::GetScore(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScore;
    return NOERROR;
}

ECode CWifiInfo::SetScore(
    /* [in] */ Int32 score)
{
    mScore = score;
    return NOERROR;
}

ECode CWifiInfo::UpdatePacketRates(
    /* [in] */ IWifiLinkLayerStats* stats)
{
    if (stats != NULL) {
        Int64 txmpdu_be, txmpdu_bk, txmpdu_vi, txmpdu_vo;
        stats->GetTxmpdu_be(&txmpdu_be);
        stats->GetTxmpdu_bk(&txmpdu_bk);
        stats->GetTxmpdu_vi(&txmpdu_vi);
        stats->GetTxmpdu_vo(&txmpdu_vo);
        Int64 txgood = txmpdu_be + txmpdu_bk + txmpdu_vi + txmpdu_vo;

        Int64 retries_be, retries_bk, retries_vi, retries_vo;
        stats->GetRetries_be(&retries_be);
        stats->GetRetries_bk(&retries_bk);
        stats->GetRetries_vi(&retries_vi);
        stats->GetRetries_vo(&retries_vo);
        Int64 txretries = retries_be + retries_bk + retries_vi + retries_vo;

        Int64 rxmpdu_be, rxmpdu_bk, rxmpdu_vi, rxmpdu_vo;
        stats->GetRxmpdu_be(&rxmpdu_be);
        stats->GetRxmpdu_bk(&rxmpdu_bk);
        stats->GetRxmpdu_vi(&rxmpdu_vi);
        stats->GetRxmpdu_vo(&rxmpdu_vo);
        Int64 rxgood = rxmpdu_be + rxmpdu_bk + rxmpdu_vi + rxmpdu_vo;

        Int64 lostmpdu_be, lostmpdu_bk, lostmpdu_vi, lostmpdu_vo;
        stats->GetLostmpdu_be(&lostmpdu_be);
        stats->GetLostmpdu_bk(&lostmpdu_bk);
        stats->GetLostmpdu_vi(&lostmpdu_vi);
        stats->GetLostmpdu_vo(&lostmpdu_vo);
        Int64 txbad = lostmpdu_be + lostmpdu_bk + lostmpdu_vi + lostmpdu_vo;

        mTxBadRate = (mTxBadRate * 0.5)
            + ((Double) (txbad - mTxBad) * 0.5);
        mTxSuccessRate = (mTxSuccessRate * 0.5)
            + ((Double) (txgood - mTxSuccess) * 0.5);
        mRxSuccessRate = (mRxSuccessRate * 0.5)
            + ((Double) (rxgood - mRxSuccess) * 0.5);
        mTxRetriesRate = (mTxRetriesRate * 0.5)
            + ((Double) (txretries - mTxRetries) * 0.5);

        mTxBad = txbad;
        mTxSuccess = txgood;
        mRxSuccess = rxgood;
        mTxRetries = txretries;
    }
    else {
        mTxBad = 0;
        mTxSuccess = 0;
        mRxSuccess = 0;
        mTxRetries = 0;
        mTxBadRate = 0;
        mTxSuccessRate = 0;
        mRxSuccessRate = 0;
        mTxRetriesRate = 0;
    }

    return NOERROR;
}

ECode CWifiInfo::UpdatePacketRates(
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 rxPackets)
{
    //paranoia
    mTxBad = 0;
    mTxRetries = 0;
    mTxBadRate = 0;
    mTxRetriesRate = 0;

    mTxSuccessRate = (mTxSuccessRate * 0.5)
            + ((Double) (txPackets - mTxSuccess) * 0.5);
    mRxSuccessRate = (mRxSuccessRate * 0.5)
            + ((Double) (rxPackets - mRxSuccess) * 0.5);
    mTxSuccess = txPackets;
    mRxSuccess = rxPackets;

    return NOERROR;
}

ECode CWifiInfo::Reset()
{
    SetInetAddress(NULL);
    SetBSSID(String(NULL));
    SetSSID(NULL);
    SetNetworkId(-1);
    SetRssi(INVALID_RSSI);
    SetLinkSpeed(-1);
    SetFrequency(-1);
    SetMeteredHint(FALSE);
    mTxBad = 0;
    mTxSuccess = 0;
    mRxSuccess = 0;
    mTxRetries = 0;
    mTxBadRate = 0;
    mTxSuccessRate = 0;
    mRxSuccessRate = 0;
    mTxRetriesRate = 0;
    mLowRssiCount = 0;
    mBadRssiCount = 0;
    mLinkStuckCount = 0;
    mScore = 0;
    return NOERROR;
}

ECode CWifiInfo::GetFrequency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFrequency;
    return NOERROR;
}

ECode CWifiInfo::SetFrequency(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    return NOERROR;
}

ECode CWifiInfo::Is24GHz(
    /* [out] */ Boolean* result)
{
    AutoPtr<IScanResultHelper> helpler;
    CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helpler);
    return helpler->Is24GHz(mFrequency, result);
}

ECode CWifiInfo::Is5GHz(
    /* [out] */ Boolean* result)
{
    AutoPtr<IScanResultHelper> helpler;
    CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helpler);
    return helpler->Is5GHz(mFrequency, result);
}

ECode CWifiInfo::SetSSID(
    /* [in] */ IWifiSsid* wifiSsid)
{
    mWifiSsid = wifiSsid;
    return NOERROR;
}

ECode CWifiInfo::GetSSID(
    /* [out] */ String* ssid)
{
    VALIDATE_NOT_NULL(ssid);

    if (mWifiSsid != NULL) {
        String unicode;
        IObject::Probe(mWifiSsid)->ToString(&unicode);

        AutoPtr<ICharSequence> tmp;
        CString::New(unicode, (ICharSequence**)&tmp);
        if (!TextUtils::IsEmpty(tmp)) {
            *ssid = String("\"") + unicode + String("\"");
            return NOERROR;
        } else {
            return mWifiSsid->GetHexString(ssid);
        }
    }

    *ssid = IWifiSsid::NONE;
    return NOERROR;
}

ECode CWifiInfo::GetWifiSsid(
    /* [in] */ IWifiSsid** ssid)
{
    VALIDATE_NOT_NULL(ssid);
    *ssid = mWifiSsid;
    REFCOUNT_ADD(*ssid);
    return NOERROR;
}

ECode CWifiInfo::SetBSSID(
    /* [in] */ const String& BSSID)
{
    mBSSID = BSSID;
    return NOERROR;
}

ECode CWifiInfo::GetBSSID(
    /* [out] */ String* bssid)
{
    VALIDATE_NOT_NULL(bssid);
    *bssid = mBSSID;
    return NOERROR;
}

ECode CWifiInfo::GetRssi(
    /* [out] */ Int32* rssi)
{
    VALIDATE_NOT_NULL(rssi);
    *rssi = mRssi;
    return NOERROR;
}

ECode CWifiInfo::SetRssi(
    /* [in] */ Int32 rssi)
{
    if (rssi < INVALID_RSSI)
        rssi = INVALID_RSSI;
    if (rssi > MAX_RSSI)
        rssi = MAX_RSSI;
    mRssi = rssi;
    return NOERROR;
}

ECode CWifiInfo::GetLinkSpeed(
    /* [out] */ Int32* speed)
{
    VALIDATE_NOT_NULL(speed);
    *speed = mLinkSpeed;
    return NOERROR;
}

ECode CWifiInfo::SetLinkSpeed(
    /* [in] */ Int32 linkSpeed)
{
    mLinkSpeed = linkSpeed;
    return NOERROR;
}

ECode CWifiInfo::SetMacAddress(
    /* [in] */ const String& macAddress)
{
    mMacAddress = macAddress;
    return NOERROR;
}

ECode CWifiInfo::GetMacAddress(
    /* [in] */ String* address)
{
    VALIDATE_NOT_NULL(address);
    *address = mMacAddress;
    return NOERROR;
}

ECode CWifiInfo::SetMeteredHint(
    /* [in] */ Boolean meteredHint)
{
    mMeteredHint = meteredHint;
    return NOERROR;
}

ECode CWifiInfo::GetMeteredHint(
    /* [out] */ Boolean* hint)
{
    VALIDATE_NOT_NULL(hint);
    *hint = mMeteredHint;
    return NOERROR;
}

ECode CWifiInfo::SetNetworkId(
    /* [in] */ Int32 id)
{
    mNetworkId = id;
    return NOERROR;
}

ECode CWifiInfo::GetNetworkId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mNetworkId;
    return NOERROR;
}

ECode CWifiInfo::GetSupplicantState(
    /* [out] */ ISupplicantState** state)
{
    VALIDATE_NOT_NULL(state);
    AutoPtr<ISupplicantState> iSs;
    CSupplicantState::New(mSupplicantState, (ISupplicantState**)&iSs);
    *state = iSs;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode CWifiInfo::SetSupplicantState(
    /* [in] */ ISupplicantState* _state)
{
    SupplicantState state;
    _state->Get(&state);
    mSupplicantState = state;
    return E_NOT_IMPLEMENTED;
}

ECode CWifiInfo::SetInetAddress(
    /* [in] */ IInetAddress* address)
{
    mIpAddress = address;
    return NOERROR;
}

ECode CWifiInfo::GetIpAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address);

    *address = 0;
    if (IInet4Address::Probe(mIpAddress) != NULL) {
        AutoPtr<INetworkUtils> utils;
        CNetworkUtils::AcquireSingleton((INetworkUtils**)&utils);
        utils->InetAddressToInt(IInet4Address::Probe(mIpAddress), address);
    }

    return NOERROR;
}

ECode CWifiInfo::GetHiddenSSID(
    /* [out] */ Boolean* ssid)
{
    VALIDATE_NOT_NULL(ssid);
    if (mWifiSsid == NULL) {
        *ssid = FALSE;
        return NOERROR;
    }

    return mWifiSsid->IsHidden(ssid);
}

NetworkInfoDetailedState CWifiInfo::GetDetailedStateOf(
    /* [in] */ SupplicantState suppState)
{
    return sStateMap[suppState];
}

void CWifiInfo::SetSupplicantState(
    /* [in] */ const String& stateName)
{
    mSupplicantState = ValueOf(stateName);
}

SupplicantState CWifiInfo::ValueOf(
    /* [in] */ const String& stateName)
{
    if (String("4WAY_HANDSHAKE").EqualsIgnoreCase(stateName) == 0) {
        return SupplicantState_FOUR_WAY_HANDSHAKE;
    }
    else {
        // try {
        /* SupplicantState.valueOf(stateName.ToUpperCase(Locale.ROOT))*/
        String tmp = stateName.ToUpperCase();
        return StringUtils::ParseInt32(tmp);
        // } catch (IllegalArgumentException e) {
        //     return SupplicantState.INVALID;
        // }
    }
}

ECode CWifiInfo::RemoveDoubleQuotes(
    /* [in] */ const String& str,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (str.IsNull()) return NOERROR;
    AutoPtr<ArrayOf<Char32> > charArray = str.GetChars();
    const Int32 length = charArray->GetLength();
    if ((length > 1) && ((*charArray)[0] == '"') && ((*charArray)[length - 1] == '"')) {
        *value = str.Substring(1, length - 1);
        return NOERROR;
    }
    *value = str;
    return NOERROR;
}

// @Override
ECode CWifiInfo::ToString(
    /* [out] */ String* str)
{
    StringBuffer sb;
    String none("<none>");

    sb.Append("SSID: ");
    String sidStr;
    IObject::Probe(mWifiSsid)->ToString(&sidStr);
    sb.Append(mWifiSsid == NULL ? IWifiSsid::NONE : sidStr);
    sb.Append(", BSSID: ");
    sb.Append(mBSSID.IsNull() ? none : mBSSID);
    sb.Append(", MAC: ");
    sb.Append(mMacAddress.IsNull() ? none : mMacAddress);
    sb.Append(", Supplicant state: ");
    if (mSupplicantState == SupplicantState_NONE) sb.Append(none);
    sb.Append(mSupplicantState);
    sb.Append(", RSSI: ");
    sb.Append(mRssi);
    sb.Append(", Link speed: ");
    sb.Append(mLinkSpeed);
    sb.Append(LINK_SPEED_UNITS);
    sb.Append(", Frequency: ");
    sb.Append(mFrequency);
    sb.Append(FREQUENCY_UNITS);
    sb.Append(", Net ID: ");
    sb.Append(mNetworkId);
    sb.Append(", Metered hint: ");
    sb.Append(mMeteredHint);
    sb.Append(", score: ");
    sb.Append(mScore);

    return sb.ToString(str);
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNetworkId);
    dest->WriteInt32(mRssi);
    dest->WriteInt32(mLinkSpeed);
    dest->WriteInt32(mFrequency);
    if (mIpAddress != NULL) {
        dest->WriteByte((byte)1);
        AutoPtr<ArrayOf<Byte> > address;
        mIpAddress->GetAddress((ArrayOf<Byte>**)&address);
        dest->WriteArrayOf((Handle32)address.Get());
    }
    else {
        dest->WriteByte((byte)0);
    }

    if (mWifiSsid != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mWifiSsid.Get())->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteString(mBSSID);
    dest->WriteString(mMacAddress);
    dest->WriteInt32(mMeteredHint ? 1 : 0);
    dest->WriteInt32(mScore);
    dest->WriteDouble(mTxSuccessRate);
    dest->WriteDouble(mTxRetriesRate);
    dest->WriteDouble(mTxBadRate);
    dest->WriteDouble(mRxSuccessRate);
    dest->WriteInt32(mBadRssiCount);
    dest->WriteInt32(mLowRssiCount);
    dest->WriteInt32(mSupplicantState);

    return NOERROR;
}

ECode CWifiInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNetworkId);
    source->ReadInt32(&mRssi);
    source->ReadInt32(&mLinkSpeed);
    source->ReadInt32(&mFrequency);
    Byte b;
    source->ReadByte(&b);
    if (b == (byte)1) {
        AutoPtr<ArrayOf<Byte> > address;
        source->ReadArrayOf((Handle32*)&address);

        AutoPtr<IInetAddressHelper> helpler;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helpler);
        helpler->GetByAddress(address, (IInetAddress**)&mIpAddress);
    }
    Int32 value;
    source->ReadInt32(&value);
    mWifiSsid = NULL;
    if (value == 1) {
        CWifiSsid::New((IWifiSsid**)&mWifiSsid);
        IParcelable::Probe(mWifiSsid.Get())->ReadFromParcel(source);
    }

    source->ReadString(&mBSSID);
    source->ReadString(&mMacAddress);
    Int32 hint;
    source->ReadInt32(&hint);
    mMeteredHint = hint? TRUE : FALSE;
    source->ReadInt32(&mScore);
    source->ReadDouble(&mTxSuccessRate);
    source->ReadDouble(&mTxRetriesRate);
    source->ReadDouble(&mTxBadRate);
    source->ReadDouble(&mRxSuccessRate);
    source->ReadInt32(&mBadRssiCount);
    source->ReadInt32(&mLowRssiCount);
    source->ReadInt32(&mSupplicantState);

    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
