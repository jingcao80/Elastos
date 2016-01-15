
#include "CServiceState.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CServiceState::LOG_TAG("CServiceState");

ECode CServiceState::constructor()
{
    return NOERROR;
}

ECode CServiceState::constructor(
    /* [in] */ IServiceState* s)
{
    return CopyFrom(s);
}

ECode CServiceState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mState);
    Int32 roaming;
    source->ReadInt32(&roaming);
    mRoaming = roaming;
    source->ReadString(&mOperatorAlphaLong);
    source->ReadString(&mOperatorAlphaShort);
    source->ReadString(&mOperatorNumeric);
    Int32 isManualNetworkSelection; source->ReadInt32(&isManualNetworkSelection); mIsManualNetworkSelection = isManualNetworkSelection;
    source->ReadInt32(&mRadioTechnology);
    Int32 cssIndicator; source->ReadInt32(&cssIndicator); mCssIndicator = cssIndicator;
    source->ReadInt32(&mNetworkId);
    source->ReadInt32(&mSystemId);
    source->ReadInt32(&mCdmaRoamingIndicator);
    source->ReadInt32(&mCdmaDefaultRoamingIndicator);
    source->ReadInt32(&mCdmaEriIconIndex);
    source->ReadInt32(&mCdmaEriIconMode);
    Int32 isEmergencyOnly; source->ReadInt32(&isEmergencyOnly); mIsEmergencyOnly = isEmergencyOnly;
    return NOERROR;
}

ECode CServiceState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mState);
    dest->WriteInt32(mRoaming ? 1 : 0);
    dest->WriteString(mOperatorAlphaLong);
    dest->WriteString(mOperatorAlphaShort);
    dest->WriteString(mOperatorNumeric);
    dest->WriteInt32(mIsManualNetworkSelection ? 1 : 0);
    dest->WriteInt32(mRadioTechnology);
    dest->WriteInt32(mCssIndicator ? 1 : 0);
    dest->WriteInt32(mNetworkId);
    dest->WriteInt32(mSystemId);
    dest->WriteInt32(mCdmaRoamingIndicator);
    dest->WriteInt32(mCdmaDefaultRoamingIndicator);
    dest->WriteInt32(mCdmaEriIconIndex);
    dest->WriteInt32(mCdmaEriIconMode);
    dest->WriteInt32(mIsEmergencyOnly ? 1 : 0);
    return NOERROR;
}

ECode CServiceState::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CServiceState::GetRoaming(
    /* [out] */ Boolean* roaming)
{
    VALIDATE_NOT_NULL(roaming);
    *roaming = mRoaming;
    return NOERROR;
}

ECode CServiceState::IsEmergencyOnly(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mIsEmergencyOnly;
    return NOERROR;
}

ECode CServiceState::GetCdmaRoamingIndicator(
    /* [out] */ Int32* cdmaRoamingIndicator)
{
    VALIDATE_NOT_NULL(cdmaRoamingIndicator);
    *cdmaRoamingIndicator = mCdmaRoamingIndicator;
    return NOERROR;
}

ECode CServiceState::GetCdmaDefaultRoamingIndicator(
    /* [out] */ Int32* cdmaDefaultRoamingIndicator)
{
    VALIDATE_NOT_NULL(cdmaDefaultRoamingIndicator);
    *cdmaDefaultRoamingIndicator = mCdmaDefaultRoamingIndicator;
    return NOERROR;
}

ECode CServiceState::GetCdmaEriIconIndex(
    /* [out] */ Int32* cdmaEriIconIndex)
{
    VALIDATE_NOT_NULL(cdmaEriIconIndex);
    *cdmaEriIconIndex = mCdmaEriIconIndex;
    return NOERROR;
}

ECode CServiceState::GetCdmaEriIconMode(
    /* [out] */ Int32* cdmaEriIconMode)
{
    VALIDATE_NOT_NULL(cdmaEriIconMode);
    *cdmaEriIconMode = mCdmaEriIconMode;
    return NOERROR;
}

ECode CServiceState::GetOperatorAlphaLong(
    /* [out] */ String* operatorAlphaLong)
{
    VALIDATE_NOT_NULL(operatorAlphaLong);
    *operatorAlphaLong = mOperatorAlphaLong;
    return NOERROR;
}

ECode CServiceState::GetOperatorAlphaShort(
    /* [out] */ String* operatorAlphaShort)
{
    VALIDATE_NOT_NULL(operatorAlphaShort);
    *operatorAlphaShort = mOperatorAlphaShort;
    return NOERROR;
}

ECode CServiceState::GetOperatorNumeric(
    /* [out] */ String* operatorNumeric)
{
    VALIDATE_NOT_NULL(operatorNumeric);
    *operatorNumeric = mOperatorNumeric;
    return NOERROR;
}

ECode CServiceState::GetIsManualSelection(
    /* [out] */ Boolean* isManualSelection)
{
    VALIDATE_NOT_NULL(isManualSelection);
    *isManualSelection = mIsManualNetworkSelection;
    return NOERROR;
}

ECode CServiceState::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = ((mState * 0x1234)
            + (mRoaming ? 1 : 0)
            + (mIsManualNetworkSelection ? 1 : 0)
            + ((mOperatorAlphaLong.IsNull()) ? 0 : mOperatorAlphaLong.GetHashCode())
            + ((mOperatorAlphaShort.IsNull()) ? 0 : mOperatorAlphaShort.GetHashCode())
            + ((mOperatorNumeric.IsNull()) ? 0 : mOperatorNumeric.GetHashCode())
            + mCdmaRoamingIndicator
            + mCdmaDefaultRoamingIndicator
            + (mIsEmergencyOnly ? 1 : 0));
    return NOERROR;
}

ECode CServiceState::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IServiceState> s = IServiceState::Probe(o);

    /*try {
        s = (ServiceState) o;
    } catch (ClassCastException ex) {
        return false;
    }*/

    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 state, radioTechnology, networkId, systemId, cdmaRoamingIndicator, cdmaDefaultRoamingIndicator, cdmaEriIconIndex, cdmaEriIconMode, cssIndicator;
    Boolean roaming, isManualNetworkSelection, isEmergencyOnly;
    String operatorAlphaLong, operatorAlphaShort, operatorNumeric;
    s->GetState(&state);
    s->GetRoaming(&roaming);
    s->GetIsManualSelection(&isManualNetworkSelection);
    s->GetOperatorAlphaLong(&operatorAlphaLong);
    s->GetOperatorAlphaShort(&operatorAlphaShort);
    s->GetOperatorNumeric(&operatorNumeric);
    s->GetRadioTechnology(&radioTechnology);
    s->GetCssIndicator(&cssIndicator);
    s->GetNetworkId(&networkId);
    s->GetSystemId(&systemId);
    s->GetCdmaRoamingIndicator(&cdmaRoamingIndicator);
    s->GetCdmaDefaultRoamingIndicator(&cdmaDefaultRoamingIndicator);
    s->IsEmergencyOnly(&isEmergencyOnly);

    *res = (mState == state
            && mRoaming == roaming
            && mIsManualNetworkSelection == isManualNetworkSelection
            && EqualsHandlesNulls(mOperatorAlphaLong, operatorAlphaLong)
            && EqualsHandlesNulls(mOperatorAlphaShort, operatorAlphaShort)
            && EqualsHandlesNulls(mOperatorNumeric, operatorNumeric)
            && mRadioTechnology == radioTechnology
            && mCssIndicator == cssIndicator
            && mNetworkId == networkId
            && mSystemId == systemId
            && mCdmaRoamingIndicator == cdmaRoamingIndicator
            && mCdmaDefaultRoamingIndicator ==
                    cdmaDefaultRoamingIndicator
            && mIsEmergencyOnly == isEmergencyOnly);

    return NOERROR;
}

ECode CServiceState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    String radioTechnology = RilRadioTechnologyToString(mRadioTechnology);

    sb.Append(mState); sb.Append(" "); sb.Append(mRoaming ? "roaming" : "home");
    sb.Append(" "); sb.Append(mOperatorAlphaLong);
    sb.Append(" "); sb.Append(mOperatorAlphaShort);
    sb.Append(" "); sb.Append(mOperatorNumeric);
    sb.Append(" "); sb.Append(mIsManualNetworkSelection ? "(manual)" : "");
    sb.Append(" "); sb.Append(radioTechnology);
    sb.Append(" "); sb.Append(mCssIndicator ? "CSS supported" : "CSS not supported");
    sb.Append(" "); sb.Append(mNetworkId);
    sb.Append(" "); sb.Append(mSystemId);
    sb.Append(" RoamInd="); sb.Append(mCdmaRoamingIndicator);
    sb.Append(" DefRoamInd="); sb.Append(mCdmaDefaultRoamingIndicator);
    sb.Append(" EmergOnly="); sb.AppendBoolean(mIsEmergencyOnly);
    *str = sb.ToString();
    return NOERROR;
}

ECode CServiceState::SetStateOutOfService()
{
    return SetNullState(IServiceState::STATE_OUT_OF_SERVICE);
}

ECode CServiceState::SetStateOff()
{
    return SetNullState(IServiceState::STATE_POWER_OFF);;
}

ECode CServiceState::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode CServiceState::SetRoaming(
    /* [in] */ Boolean roaming)
{
    mRoaming = roaming;
    return NOERROR;
}

ECode CServiceState::SetEmergencyOnly(
    /* [in] */ Boolean emergencyOnly)
{
    mIsEmergencyOnly = emergencyOnly;
    return NOERROR;
}

ECode CServiceState::SetCdmaRoamingIndicator(
    /* [in] */ Int32 roaming)
{
    mCdmaRoamingIndicator = roaming;
    return NOERROR;
}

ECode CServiceState::SetCdmaDefaultRoamingIndicator(
    /* [in] */ Int32 roaming)
{
    mCdmaDefaultRoamingIndicator = roaming;
    return NOERROR;
}

ECode CServiceState::SetCdmaEriIconIndex(
    /* [in] */ Int32 index)
{
    mCdmaEriIconIndex = index;
    return NOERROR;
}

ECode CServiceState::SetCdmaEriIconMode(
    /* [in] */ Int32 mode)
{
    mCdmaEriIconMode = mode;
    return NOERROR;
}

ECode CServiceState::SetOperatorName(
    /* [in] */ const String& longName,
    /* [in] */ const String& shortName,
    /* [in] */ const String& numeric)
{

    mOperatorAlphaLong = longName;
    mOperatorAlphaShort = shortName;
    mOperatorNumeric = numeric;
    return NOERROR;
}

ECode CServiceState::SetOperatorAlphaLong(
    /* [in] */ const String& longName)
{
    mOperatorAlphaLong = longName;
    return NOERROR;
}

ECode CServiceState::SetIsManualSelection(
    /* [in] */ Boolean isManual)
{
    mIsManualNetworkSelection = isManual;
    return NOERROR;
}

ECode CServiceState::FillInNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->PutInt32(String("state"), mState);
    m->PutBoolean(String("roaming"), mRoaming);
    m->PutString(String("operator-alpha-long"), mOperatorAlphaLong);
    m->PutString(String("operator-alpha-short"), mOperatorAlphaShort);
    m->PutString(String("operator-numeric"), mOperatorNumeric);
    m->PutBoolean(String("manual"), mIsManualNetworkSelection);
    m->PutInt32(String("radioTechnology"), mRadioTechnology);
    m->PutInt32(String("cssIndicator"), mCssIndicator);
    m->PutInt32(String("networkId"), mNetworkId);
    m->PutInt32(String("systemId"), mSystemId);
    m->PutInt32(String("cdmaRoamingIndicator"), mCdmaRoamingIndicator);
    m->PutInt32(String("cdmaDefaultRoamingIndicator"), mCdmaDefaultRoamingIndicator);
    m->PutBoolean(String("emergencyOnly"), mIsEmergencyOnly);
    return NOERROR;
}

ECode CServiceState::SetRadioTechnology(
    /* [in] */ Int32 state)
{
    mRadioTechnology = state;
    return NOERROR;
}

ECode CServiceState::SetCssIndicator(
    /* [in] */ Int32 css)
{
    mCssIndicator = (css != 0);
    return NOERROR;
}

ECode CServiceState::SetSystemAndNetworkId(
    /* [in] */ Int32 systemId,
    /* [in] */ Int32 networkId)
{
    mSystemId = systemId;
    mNetworkId = networkId;
    return NOERROR;
}

ECode CServiceState::GetRilRadioTechnology(
    /* [out] */ Int32* rilRadioTechnology)
{
    VALIDATE_NOT_NULL(rilRadioTechnology);
    *rilRadioTechnology = mRadioTechnology;
    return NOERROR;
}

ECode CServiceState::GetRadioTechnology(
    /* [out] */ Int32* radioTechnology)
{
    VALIDATE_NOT_NULL(radioTechnology);
    GetRilRadioTechnology(radioTechnology);
}

ECode CServiceState::GetNetworkType(
    /* [out] */ Int32* networkType)
{
    VALIDATE_NOT_NULL(networkType);
    switch(mRadioTechnology) {

        case IServiceState::RIL_RADIO_TECHNOLOGY_GPRS:
            *networkType = ITelephonyManager::NETWORK_TYPE_GPRS;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EDGE:
            *networkType = ITelephonyManager::NETWORK_TYPE_EDGE;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_UMTS:
            *networkType = ITelephonyManager::NETWORK_TYPE_UMTS;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA:
            *networkType = ITelephonyManager::NETWORK_TYPE_HSDPA;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA:
            *networkType = ITelephonyManager::NETWORK_TYPE_HSUPA;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPA:
            *networkType = ITelephonyManager::NETWORK_TYPE_HSPA;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95A:
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95B:
            *networkType = ITelephonyManager::NETWORK_TYPE_CDMA;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT:
            *networkType = ITelephonyManager::NETWORK_TYPE_1xRTT;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0:
            *networkType = ITelephonyManager::NETWORK_TYPE_EVDO_0;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A:
            *networkType = ITelephonyManager::NETWORK_TYPE_EVDO_A;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B:
            *networkType = ITelephonyManager::NETWORK_TYPE_EVDO_B;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD:
            *networkType = ITelephonyManager::NETWORK_TYPE_EHRPD;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_LTE:
            *networkType = ITelephonyManager::NETWORK_TYPE_LTE;
            return NOERROR;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP:
            *networkType = ITelephonyManager::NETWORK_TYPE_HSPAP;
            return NOERROR;
        default:
            *networkType = ITelephonyManager::NETWORK_TYPE_UNKNOWN;
            return NOERROR;
    }
    return NOERROR;
}

ECode CServiceState::GetCssIndicator(
    /* [out] */ Int32* cssIndicator)
{
    VALIDATE_NOT_NULL(cssIndicator);
    *cssIndicator = (mCssIndicator ? 1 : 0);
    return NOERROR;
}

ECode CServiceState::GetNetworkId(
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);
    *networkId = mNetworkId;
    return NOERROR;
}

ECode CServiceState::GetSystemId(
    /* [out] */ Int32* systemId)
{
    VALIDATE_NOT_NULL(systemId);
    *systemId = mSystemId;
    return NOERROR;
}

ECode CServiceState::NewFromBundle(
    /* [in] */ IBundle* m,
    /* [out] */ IServiceState** res)
{

    AutoPtr<CServiceState> ret;
    CServiceState::NewByFriend((CServiceState**)&ret);
    ret->SetFromNotifierBundle(m);
    *res = IServiceState::Probe(ret);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

Boolean CServiceState::EqualsHandlesNulls(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    return (a.IsNull()) ? (b.IsNull()) : a.Equals(b);
}

String CServiceState::RilRadioTechnologyToString(
    /* [in] */ Int32 rt)
{

    String rtString;

    switch(rt) {
        case IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN:
            rtString = String("Unknown");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_GPRS:
            rtString = String("GPRS");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EDGE:
            rtString = String("EDGE");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_UMTS:
            rtString = String("UMTS");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95A:
            rtString = String("CDMA-IS95A");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95B:
            rtString = String("CDMA-IS95B");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT:
            rtString = String("1xRTT");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0:
            rtString = String("EvDo-rev.0");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A:
            rtString = String("EvDo-rev.A");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA:
            rtString = String("HSDPA");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA:
            rtString = String("HSUPA");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPA:
            rtString = String("HSPA");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B:
            rtString = String("EvDo-rev.B");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD:
            rtString = String("eHRPD");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_LTE:
            rtString = String("LTE");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP:
            rtString = String("HSPAP");
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_GSM:
            rtString = String("GSM");
            break;
        default:
            rtString = String("Unexpected");
            Slogger::W(LOG_TAG, "Unexpected radioTechnology=%d", rt);
            break;
    }
    return rtString;
}

ECode CServiceState::SetNullState(
    /* [in] */ Int32 state)
{
    // END privacy-modified
     mState = state;
     mRoaming = FALSE;
     mOperatorAlphaLong = String(NULL);
     mOperatorAlphaShort = String(NULL);
     mOperatorNumeric = String(NULL);
     mIsManualNetworkSelection = FALSE;
     mRadioTechnology = 0;
     mCssIndicator = FALSE;
     mNetworkId = -1;
     mSystemId = -1;
     mCdmaRoamingIndicator = -1;
     mCdmaDefaultRoamingIndicator = -1;
     mCdmaEriIconIndex = -1;
     mCdmaEriIconMode = -1;
     mIsEmergencyOnly = FALSE;

    return NOERROR;
}

ECode CServiceState::CopyFrom(
    /* [in] */ IServiceState* s)
{
    Int32 state, radioTechnology, networkId, systemId, cdmaRoamingIndicator, cdmaDefaultRoamingIndicator, cdmaEriIconIndex, cdmaEriIconMode, cssIndicator;
    Boolean roaming, isManualNetworkSelection, isEmergencyOnly;
    String operatorAlphaLong, operatorAlphaShort, operatorNumeric;
    s->GetState(&state);
    s->GetRoaming(&roaming);
    s->GetIsManualSelection(&isManualNetworkSelection);
    s->GetOperatorAlphaLong(&operatorAlphaLong);
    s->GetOperatorAlphaShort(&operatorAlphaShort);
    s->GetOperatorNumeric(&operatorNumeric);
    s->GetRadioTechnology(&radioTechnology);
    s->GetCssIndicator(&cssIndicator);
    s->GetNetworkId(&networkId);
    s->GetSystemId(&systemId);
    s->GetCdmaRoamingIndicator(&cdmaRoamingIndicator);
    s->GetCdmaDefaultRoamingIndicator(&cdmaDefaultRoamingIndicator);
    s->GetCdmaEriIconIndex(&cdmaEriIconIndex);
    s->GetCdmaEriIconMode(&cdmaEriIconMode);
    s->IsEmergencyOnly(&isEmergencyOnly);
    mState = state;
    mRoaming = roaming;
    mOperatorAlphaLong = operatorAlphaLong;
    mOperatorAlphaShort = operatorAlphaShort;
    mOperatorNumeric = operatorNumeric;
    mIsManualNetworkSelection = isManualNetworkSelection;
    mRadioTechnology = radioTechnology;
    mCssIndicator = cssIndicator;
    mNetworkId = networkId;
    mSystemId = systemId;
    mCdmaRoamingIndicator = cdmaRoamingIndicator;
    mCdmaDefaultRoamingIndicator = cdmaDefaultRoamingIndicator;
    mCdmaEriIconIndex = cdmaEriIconIndex;
    mCdmaEriIconMode = cdmaEriIconMode;
    mIsEmergencyOnly = isEmergencyOnly;
    return NOERROR;
}

CARAPI CServiceState::SetFromNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->GetInt32(String("state"), &mState);
    m->GetBoolean(String("roaming"), &mRoaming);
    m->GetString(String("operator-alpha-long"), &mOperatorAlphaLong);
    m->GetString(String("operator-alpha-short"), &mOperatorAlphaShort);
    m->GetString(String("operator-numeric"), &mOperatorNumeric);
    m->GetBoolean(String("manual"), &mIsManualNetworkSelection);
    m->GetInt32(String("radioTechnology"), &mRadioTechnology);
    m->GetBoolean(String("cssIndicator"), &mCssIndicator);
    m->GetInt32(String("networkId"), &mNetworkId);
    m->GetInt32(String("systemId"), &mSystemId);
    m->GetInt32(String("cdmaRoamingIndicator"), &mCdmaRoamingIndicator);
    m->GetInt32(String("cdmaDefaultRoamingIndicator"), &mCdmaDefaultRoamingIndicator);
    m->GetBoolean(String("emergencyOnly"), &mIsEmergencyOnly);
    return NOERROR;
}

Boolean CServiceState::IsGsm(
    /* [in] */ Int32 radioTechnology)
{
    return radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_GPRS
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EDGE
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_UMTS
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_LTE
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_GSM;

}

Boolean CServiceState::IsCdma(
    /* [in] */ Int32  radioTechnology)
{
    return radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_IS95A
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_IS95B
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B
                    || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD;
}

}
}
}

