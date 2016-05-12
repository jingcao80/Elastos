
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CScanResult.h"
#include "elastos/droid/wifi/CScanResultHelper.h"
#include "elastos/droid/wifi/CWifiSsid.h"
#include "elastos/droid/wifi/CScanResultInformationElement.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::Wifi::CScanResultInformationElement;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CScanResult, Object, IScanResult, IParcelable)

CAR_OBJECT_IMPL(CScanResult)

ECode CScanResult::constructor()
{
    return NOERROR;
}

ECode CScanResult::constructor(
    /* [in] */ IWifiSsid* wifiSsid,
    /* [in] */ const String& BSSID,
    /* [in] */ const String& caps,
    /* [in] */ Int32 level,
    /* [in] */ Int32 frequency,
    /* [in] */ Int64 tsf)
{
    mWifiSsid = wifiSsid;
    if (wifiSsid != NULL) {
        IObject::Probe(wifiSsid)->ToString(&mSSID);
    }
    else {
        mSSID = IWifiSsid::NONE;
    }
    mBSSID = BSSID;
    mCapabilities = caps;
    mLevel = level;
    mFrequency = frequency;
    mTimestamp = tsf;
    mDistanceCm = UNSPECIFIED;
    mDistanceSdCm = UNSPECIFIED;

    return NOERROR;
}

ECode CScanResult::constructor(
    /* [in] */ IWifiSsid* wifiSsid,
    /* [in] */ const String& BSSID,
    /* [in] */ const String& caps,
    /* [in] */ Int32 level,
    /* [in] */ Int32 frequency,
    /* [in] */ Int64 tsf,
    /* [in] */ Int32 distCm,
    /* [in] */ Int32 distSdCm)
{
    mWifiSsid = wifiSsid;
    if (wifiSsid != NULL) {
        IObject::Probe(wifiSsid)->ToString(&mSSID);
    }
    else {
        mSSID = IWifiSsid::NONE;
    }
    mBSSID = BSSID;
    mCapabilities = caps;
    mLevel = level;
    mFrequency = frequency;
    mTimestamp = tsf;
    mDistanceCm = distCm;
    mDistanceSdCm = distSdCm;

    return NOERROR;
}

ECode CScanResult::constructor(
    /* [in] */ IScanResult* source)
{
    if (source != NULL) {
        source->GetWifiSsid((IWifiSsid**)&mWifiSsid);
        source->GetSSID(&mSSID);
        source->GetBSSID(&mBSSID);
        source->GetCapabilities(&mCapabilities);
        source->GetLevel(&mLevel);
        source->GetFrequency(&mFrequency);
        source->GetTimestamp(&mTimestamp);
        source->GetDistanceCm(&mDistanceCm);
        source->GetDistanceSdCm(&mDistanceSdCm);
        source->GetSeen(&mSeen);
        source->GetAutoJoinStatus(&mAutoJoinStatus);
        source->GetUntrusted(&mUntrusted);
        source->GetNumConnection(&mNumConnection);
        source->GetNumUsage(&mNumUsage);
        source->GetNumIpConfigFailures(&mNumIpConfigFailures);
        source->GetIsAutoJoinCandidate(&mIsAutoJoinCandidate);
    }
    return NOERROR;
}

ECode CScanResult::GetSSID(
    /* [out] */ String* ssid)
{
    VALIDATE_NOT_NULL(ssid)
    *ssid = mSSID;
    return NOERROR;
}

ECode CScanResult::SetSSID(
    /* [in] */ const String& ssid)
{
    mSSID = ssid;
    return NOERROR;
}

ECode CScanResult::GetWifiSsid(
    /* [out] */ IWifiSsid** ssid)
{
    VALIDATE_NOT_NULL(ssid)
    *ssid = mWifiSsid;
    REFCOUNT_ADD(*ssid)
    return NOERROR;
}

ECode CScanResult::SetWifiSsid(
    /* [in] */ IWifiSsid* ssid)
{
    mWifiSsid = ssid;
    return NOERROR;
}

ECode CScanResult::GetBSSID(
    /* [out] */ String* bssid)
{
    VALIDATE_NOT_NULL(bssid)
    *bssid = mBSSID;
    return NOERROR;
}

ECode CScanResult::SetBSSID(
    /* [in] */ const String& bssid)
{
    mBSSID = bssid;
    return NOERROR;
}

ECode CScanResult::GetCapabilities(
    /* [out] */ String* capabilities)
{
    VALIDATE_NOT_NULL(capabilities)
    *capabilities = mCapabilities;
    return NOERROR;
}

ECode CScanResult::SetCapabilities(
    /* [in] */ const String& capabilities)
{
    mCapabilities = capabilities;
    return NOERROR;
}

ECode CScanResult::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level)
    *level = mLevel;
    return NOERROR;
}

ECode CScanResult::SetLevel(
    /* [in] */ Int32 level)
{
    mLevel = level;
    return NOERROR;
}

ECode CScanResult::GetFrequency(
    /* [out] */ Int32* frequency)
{
    VALIDATE_NOT_NULL(frequency)
    *frequency = mFrequency;
    return NOERROR;
}

ECode CScanResult::SetFrequency(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    return NOERROR;
}

ECode CScanResult::GetTimestamp(
    /* [out] */ Int64* timestamp)
{
    VALIDATE_NOT_NULL(timestamp)
    *timestamp = mTimestamp;
    return NOERROR;
}

ECode CScanResult::SetTimestamp(
    /* [in] */ Int64 timestamp)
{
    mTimestamp = timestamp;
    return NOERROR;
}

ECode CScanResult::GetSeen(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSeen;
    return NOERROR;
}

ECode CScanResult::SetSeen(
    /* [in] */ Int64 seen)
{
    mSeen = seen;
    return NOERROR;
}

ECode CScanResult::GetIsAutoJoinCandidate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsAutoJoinCandidate;
    return NOERROR;
}

ECode CScanResult::SetIsAutoJoinCandidate(
    /* [in] */ Int32 isAutoJoinCandidate)
{
    mIsAutoJoinCandidate = isAutoJoinCandidate;
    return NOERROR;
}

ECode CScanResult::GetAutoJoinStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAutoJoinStatus;
    return NOERROR;
}

ECode CScanResult::GetNumIpConfigFailures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumIpConfigFailures;
    return NOERROR;
}

ECode CScanResult::SetNumIpConfigFailures(
    /* [in] */ Int32 numIpConfigFailures)
{
    mNumIpConfigFailures = mNumIpConfigFailures;
    return NOERROR;
}

ECode CScanResult::GetBlackListTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBlackListTimestamp;
    return NOERROR;
}

ECode CScanResult::SetBlackListTimestamp(
    /* [in] */ Int64 blackListTimestamp)
{
    mBlackListTimestamp = blackListTimestamp;
    return NOERROR;
}

ECode CScanResult::GetUntrusted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUntrusted;
    return NOERROR;
}

ECode CScanResult::SetUntrusted(
    /* [in] */ Boolean untrusted)
{
    mUntrusted = untrusted;
    return NOERROR;
}

ECode CScanResult::GetNumConnection(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumConnection;
    return NOERROR;
}

ECode CScanResult::SetNumConnection(
    /* [in] */ Int32 numConnection)
{
    mNumConnection = numConnection;
    return NOERROR;
}

ECode CScanResult::GetNumUsage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUsage;
    return NOERROR;
}

ECode CScanResult::SetNumUsage(
    /* [in] */ Int32 numUsage)
{
    mNumUsage = numUsage;
    return NOERROR;
}

ECode CScanResult::GetDistanceCm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDistanceCm;
    return NOERROR;
}

ECode CScanResult::SetDistanceCm(
    /* [in] */ Int32 distanceCm)
{
    mDistanceCm = distanceCm;
    return NOERROR;
}

ECode CScanResult::GetDistanceSdCm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDistanceSdCm;
    return NOERROR;
}

ECode CScanResult::SetDistanceSdCm(
    /* [in] */ Int32 distanceSdCm)
{
    mDistanceSdCm = distanceSdCm;
    return NOERROR;
}

ECode CScanResult::GetInformationElements(
    /* [out, callee] */ ArrayOf<IScanResultInformationElement*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInformationElements;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CScanResult::SetInformationElements(
    /* [in] */ ArrayOf<IScanResultInformationElement*>* informationElements)
{
    mInformationElements = informationElements;
    return NOERROR;
}

ECode CScanResult::AverageRssi(
    /* [in] */ Int32 previousRssi,
    /* [in] */ Int64 previousSeen,
    /* [in] */ Int32 maxAge)
{
    if (mSeen == 0) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mSeen);
    }

    Int64 age = mSeen - previousSeen;

    if (previousSeen > 0 && age > 0 && age < maxAge/2) {
        // Average the RSSI with previously seen instances of this scan result
        Double alpha = 0.5 - (Double) age / (Double) maxAge;
        mLevel = (Int32) ((Double) mLevel * (1 - alpha) + (Double) previousRssi * alpha);
    }

    return NOERROR;
}

ECode CScanResult::SetAutoJoinStatus(
    /* [in] */ Int32 status)
{
    if (status < 0) status = 0;
    if (status == 0) {
        mBlackListTimestamp = 0;
    }
    else if (status > mAutoJoinStatus) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mBlackListTimestamp);
    }
    mAutoJoinStatus = status;
    return NOERROR;
}

ECode CScanResult::Is24GHz(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IScanResultHelper> helper;
    CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helper);
    return helper->Is24GHz(mFrequency, result);
}

ECode CScanResult::Is5GHz(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IScanResultHelper> helper;
    CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helper);
    return helper->Is5GHz(mFrequency, result);
}

ECode CScanResult::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    StringBuffer sb;
    String none("<none>");

    sb.Append("SSID: ");
    if (mWifiSsid == NULL) {
        sb.Append(IWifiSsid::NONE);
    }
    else {
        String temp;
        IObject::Probe(mWifiSsid)->ToString(&temp);
        sb.Append(temp);
    }

    sb.Append(", BSSID: ");
    sb.Append(mBSSID.IsNull() ? none : mBSSID);
    sb.Append(", capabilities: ");
    sb.Append(mCapabilities.IsNull() ? none : mCapabilities);
    sb.Append(", level: ");
    sb.Append(mLevel);
    sb.Append(", frequency: ");
    sb.Append(mFrequency);
    sb.Append(", timestamp: ");
    sb.Append(mTimestamp);
    sb.Append(", distance: ");
    if (mDistanceCm != UNSPECIFIED) {
        sb.Append(mDistanceCm);
    }
    else {
        sb.Append("?");
    }
    sb.Append("(cm)");
    sb.Append(", distanceSd: ");
    if (mDistanceSdCm != UNSPECIFIED) {
        sb.Append(mDistanceSdCm);
    }
    else {
        sb.Append("?");
    }
    sb.Append("(cm)");

    if (mAutoJoinStatus != 0) {
        sb.Append(", status: ");
        sb.Append(mAutoJoinStatus);
    }

    return sb.ToString(value);
}

ECode CScanResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mWifiSsid = IWifiSsid::Probe(obj);
    if (mWifiSsid != NULL) {
        IObject::Probe(mWifiSsid)->ToString(&mSSID);
    }
    else {
        mSSID = IWifiSsid::NONE;
    }
    source->ReadString(&mBSSID);
    source->ReadString(&mCapabilities);
    source->ReadInt32(&mLevel);
    source->ReadInt32(&mFrequency);
    source->ReadInt64(&mTimestamp);
    source->ReadInt32(&mDistanceCm);
    source->ReadInt32(&mDistanceSdCm);

    source->ReadInt64(&mSeen);
    source->ReadInt32(&mAutoJoinStatus);
    Int32 untrusted = 0;
    source->ReadInt32(&untrusted);
    mUntrusted = untrusted == 1 ? TRUE : FALSE;
    source->ReadInt32(&mNumConnection);
    source->ReadInt32(&mNumUsage);
    source->ReadInt32(&mNumIpConfigFailures);
    source->ReadInt32(&mIsAutoJoinCandidate);

    Int32 n = 0;
    source->ReadInt32(&n);
    if (n != 0) {
        mInformationElements = ArrayOf<IScanResultInformationElement*>::Alloc(n);
        for (Int32 i = 0; i < n; i++) {
            CScanResultInformationElement::New(&((*mInformationElements)[i]));
            Int32 id = 0;
            source->ReadInt32(&id);
            (*mInformationElements)[i]->SetId(id);

            Int32 len = 0;
            source->ReadInt32(&len);
            AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(len);
            assert(0 && "TODO");
            // source->ReadByteArray(&bytes);
            (*mInformationElements)[i]->SetBytes(bytes);
        }
    }
    return NOERROR;
}

ECode CScanResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mWifiSsid.Get());
    dest->WriteString(mBSSID);
    dest->WriteString(mCapabilities);
    dest->WriteInt32(mLevel);
    dest->WriteInt32(mFrequency);
    dest->WriteInt64(mTimestamp);
    dest->WriteInt32(mDistanceCm);
    dest->WriteInt32(mDistanceSdCm);
    dest->WriteInt64(mSeen);
    dest->WriteInt32(mAutoJoinStatus);
    dest->WriteInt32(mUntrusted ? 1 : 0);
    dest->WriteInt32(mNumConnection);
    dest->WriteInt32(mNumUsage);
    dest->WriteInt32(mNumIpConfigFailures);
    dest->WriteInt32(mIsAutoJoinCandidate);
    if (mInformationElements != NULL) {
        Int32 length = mInformationElements->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            Int32 id;
            (*mInformationElements)[i]->GetId(&id);
            AutoPtr< ArrayOf<Byte> > bytes;
            (*mInformationElements)[i]->GetBytes((ArrayOf<Byte>**)&bytes);
            Int32 len = bytes->GetLength();
            dest->WriteInt32(id);
            dest->WriteInt32(len);
            dest->WriteArrayOf((Handle32)bytes.Get());
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
