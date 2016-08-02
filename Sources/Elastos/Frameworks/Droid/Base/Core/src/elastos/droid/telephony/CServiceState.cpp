
#include "elastos/droid/telephony/CServiceState.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CServiceState::TAG("PHONE");
const Boolean CServiceState::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CServiceState, Object, IServiceState, IParcelable)

CAR_OBJECT_IMPL(CServiceState)

CServiceState::CServiceState()
    : mVoiceRegState(STATE_OUT_OF_SERVICE)
    , mDataRegState(STATE_OUT_OF_SERVICE)
    , mRoaming(FALSE)
    , mIsManualNetworkSelection(FALSE)
    , mIsEmergencyOnly(FALSE)
    , mRilVoiceRadioTechnology(0)
    , mRilDataRadioTechnology(0)
    , mCssIndicator(FALSE)
    , mNetworkId(0)
    , mSystemId(0)
    , mCdmaRoamingIndicator(0)
    , mCdmaDefaultRoamingIndicator(0)
    , mCdmaEriIconIndex(0)
    , mCdmaEriIconMode(0)
{
}

CServiceState::~CServiceState()
{
}

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
    source->ReadInt32(&mVoiceRegState);
    source->ReadInt32(&mDataRegState);
    source->ReadBoolean(&mRoaming);
    source->ReadString(&mOperatorAlphaLong);
    source->ReadString(&mOperatorAlphaShort);
    source->ReadString(&mOperatorNumeric);
    source->ReadBoolean(&mIsManualNetworkSelection);
    source->ReadBoolean(&mIsEmergencyOnly);
    source->ReadInt32(&mRilVoiceRadioTechnology);
    source->ReadInt32(&mRilDataRadioTechnology);
    source->ReadBoolean(&mCssIndicator);
    source->ReadInt32(&mNetworkId);
    source->ReadInt32(&mSystemId);
    source->ReadInt32(&mCdmaRoamingIndicator);
    source->ReadInt32(&mCdmaDefaultRoamingIndicator);
    source->ReadInt32(&mCdmaEriIconIndex);
    source->ReadInt32(&mCdmaEriIconMode);
    return NOERROR;
}

ECode CServiceState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mVoiceRegState);
    dest->WriteInt32(mDataRegState);
    dest->WriteBoolean(mRoaming);
    dest->WriteString(mOperatorAlphaLong);
    dest->WriteString(mOperatorAlphaShort);
    dest->WriteString(mOperatorNumeric);
    dest->WriteBoolean(mIsManualNetworkSelection);
    dest->WriteBoolean(mIsEmergencyOnly);
    dest->WriteInt32(mRilVoiceRadioTechnology);
    dest->WriteInt32(mRilDataRadioTechnology);
    dest->WriteBoolean(mCssIndicator);
    dest->WriteInt32(mNetworkId);
    dest->WriteInt32(mSystemId);
    dest->WriteInt32(mCdmaRoamingIndicator);
    dest->WriteInt32(mCdmaDefaultRoamingIndicator);
    dest->WriteInt32(mCdmaEriIconIndex);
    dest->WriteInt32(mCdmaEriIconMode);
    return NOERROR;
}

ECode CServiceState::GetState(
    /* [out] */ Int32* state)
{
    return GetVoiceRegState(state);
}

ECode CServiceState::GetVoiceRegState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mVoiceRegState;
    return NOERROR;
}

ECode CServiceState::GetDataRegState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mDataRegState;
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
    *hashCode = ((mVoiceRegState * 31)
            + (mDataRegState * 37)
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

    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    AutoPtr<IServiceState> s = IServiceState::Probe(o);
    if (s == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 voiceRegState, dataRegState;
    Int32 rilVoiceRadioTechnology, rilDataRadioTechnology;
    Int32 networkId, systemId;
    Int32 cdmaRoamingIndicator, cdmaDefaultRoamingIndicator, cdmaEriIconIndex, cdmaEriIconMode;
    Boolean roaming, isManualNetworkSelection, isEmergencyOnly, cssIndicator;
    String operatorAlphaLong, operatorAlphaShort, operatorNumeric;

    s->GetVoiceRegState(&voiceRegState);
    s->GetDataRegState(&dataRegState);
    s->GetRoaming(&roaming);
    s->GetIsManualSelection(&isManualNetworkSelection);
    s->GetOperatorAlphaLong(&operatorAlphaLong);
    s->GetOperatorAlphaShort(&operatorAlphaShort);
    s->GetOperatorNumeric(&operatorNumeric);
    s->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
    s->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    Int32 val;
    s->GetCssIndicator(&val);
    cssIndicator = val;
    s->GetNetworkId(&networkId);
    s->GetSystemId(&systemId);
    s->GetCdmaRoamingIndicator(&cdmaRoamingIndicator);
    s->GetCdmaDefaultRoamingIndicator(&cdmaDefaultRoamingIndicator);
    s->GetCdmaEriIconIndex(&cdmaEriIconIndex);
    s->GetCdmaEriIconMode(&cdmaEriIconMode);
    s->IsEmergencyOnly(&isEmergencyOnly);

    *res = (mVoiceRegState == voiceRegState
            && mDataRegState == dataRegState
            && mRoaming == roaming
            && mIsManualNetworkSelection == isManualNetworkSelection
            && EqualsHandlesNulls(mOperatorAlphaLong, operatorAlphaLong)
            && EqualsHandlesNulls(mOperatorAlphaShort, operatorAlphaShort)
            && EqualsHandlesNulls(mOperatorNumeric, operatorNumeric)
            && mRilVoiceRadioTechnology == rilVoiceRadioTechnology
            && mRilDataRadioTechnology == rilDataRadioTechnology
            && mCssIndicator == cssIndicator
            && mNetworkId == networkId
            && mSystemId == systemId
            && mCdmaRoamingIndicator == cdmaRoamingIndicator
            && mCdmaDefaultRoamingIndicator == cdmaDefaultRoamingIndicator
            && mIsEmergencyOnly == isEmergencyOnly);

    return NOERROR;
}

ECode CServiceState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    String radioTechnology;
    RilRadioTechnologyToString(mRilVoiceRadioTechnology, &radioTechnology);
    String dataRadioTechnology;
    RilRadioTechnologyToString(mRilDataRadioTechnology, &dataRadioTechnology);

    sb.Append(mVoiceRegState);
    sb.Append(" ");
    sb.Append(mDataRegState);
    sb.Append(" ");
    sb.Append(mRoaming ? "roaming" : "home");
    sb.Append(" ");
    sb.Append(mOperatorAlphaLong);
    sb.Append(" ");
    sb.Append(mOperatorAlphaShort);
    sb.Append(" ");
    sb.Append(mOperatorNumeric);
    sb.Append(" ");
    sb.Append(mIsManualNetworkSelection ? "(manual)" : "");
    sb.Append(" ");
    sb.Append(radioTechnology);
    sb.Append(" ");
    sb.Append(dataRadioTechnology);
    sb.Append(" ");
    sb.Append(mCssIndicator ? "CSS supported" : "CSS not supported");
    sb.Append(" ");
    sb.Append(mNetworkId);
    sb.Append(" ");
    sb.Append(mSystemId);
    sb.Append(" RoamInd=");
    sb.Append(mCdmaRoamingIndicator);
    sb.Append(" DefRoamInd=");
    sb.Append(mCdmaDefaultRoamingIndicator);
    sb.Append(" EmergOnly=");
    sb.Append(mIsEmergencyOnly);
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
    return SetVoiceRegState(state);
}

ECode CServiceState::SetVoiceRegState(
    /* [in] */ Int32 state)
{
    mVoiceRegState = state;
    return NOERROR;
}

ECode CServiceState::SetDataRegState(
    /* [in] */ Int32 state)
{
    mDataRegState = state;
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
    m->PutInt32(String("voiceRegState"), mVoiceRegState);
    m->PutInt32(String("dataRegState"), mDataRegState);
    m->PutBoolean(String("roaming"), mRoaming);
    m->PutString(String("operator-alpha-long"), mOperatorAlphaLong);
    m->PutString(String("operator-alpha-short"), mOperatorAlphaShort);
    m->PutString(String("operator-numeric"), mOperatorNumeric);
    m->PutBoolean(String("manual"), mIsManualNetworkSelection);
    m->PutInt32(String("radioTechnology"), mRilVoiceRadioTechnology);
    m->PutInt32(String("dataRadioTechnology"), mRilDataRadioTechnology);
    m->PutBoolean(String("cssIndicator"), mCssIndicator);
    m->PutInt32(String("networkId"), mNetworkId);
    m->PutInt32(String("systemId"), mSystemId);
    m->PutInt32(String("cdmaRoamingIndicator"), mCdmaRoamingIndicator);
    m->PutInt32(String("cdmaDefaultRoamingIndicator"), mCdmaDefaultRoamingIndicator);
    m->PutBoolean(String("emergencyOnly"), mIsEmergencyOnly);
    return NOERROR;
}

ECode CServiceState::SetRilVoiceRadioTechnology(
    /* [in] */ Int32 state)
{
    mRilVoiceRadioTechnology = state;
    return NOERROR;
}

ECode CServiceState::SetRilDataRadioTechnology(
    /* [in] */ Int32 state)
{
    mRilDataRadioTechnology = state;
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

ECode CServiceState::GetRilVoiceRadioTechnology(
    /* [out] */ Int32* rilVoiceRadioTechnology)
{
    VALIDATE_NOT_NULL(rilVoiceRadioTechnology);
    *rilVoiceRadioTechnology = mRilVoiceRadioTechnology;
    return NOERROR;
}

ECode CServiceState::GetRilDataRadioTechnology(
    /* [out] */ Int32* rilDataRadioTechnology)
{
    VALIDATE_NOT_NULL(rilDataRadioTechnology);
    *rilDataRadioTechnology = mRilDataRadioTechnology;
    return NOERROR;
}

ECode CServiceState::GetRadioTechnology(
    /* [out] */ Int32* radioTechnology)
{
    VALIDATE_NOT_NULL(radioTechnology);
    GetRilDataRadioTechnology(radioTechnology);
    return NOERROR;
}

ECode CServiceState::GetNetworkType(
    /* [out] */ Int32* type)
{
    return RilRadioTechnologyToNetworkType(mRilVoiceRadioTechnology, type);
}

ECode CServiceState::GetDataNetworkType(
    /* [out] */ Int32* type)
{
    return RilRadioTechnologyToNetworkType(mRilDataRadioTechnology, type);
}

ECode CServiceState::GetVoiceNetworkType(
    /* [out] */ Int32* type)
{
    return RilRadioTechnologyToNetworkType(mRilVoiceRadioTechnology, type);
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

ECode CServiceState::RilRadioTechnologyToString(
    /* [in] */ Int32 rt,
    /* [out] */ String* str)
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
            Slogger::W(TAG, "Unexpected radioTechnology=%d", rt);
            break;
    }
    *str = rtString;
    return NOERROR;
}

ECode CServiceState::IsGsm(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_GPRS
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EDGE
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_UMTS
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSPA
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_LTE
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_GSM
            || radioTechnology == RIL_RADIO_TECHNOLOGY_TD_SCDMA;
    return NOERROR;
}

ECode CServiceState::IsCdma(
    /* [in] */ Int32  radioTechnology,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_IS95A
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_IS95B
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B
            || radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD;
    return NOERROR;
}

ECode CServiceState::CopyFrom(
    /* [in] */ IServiceState* s)
{
    Int32 voiceRegState, dataRegState;
    Int32 rilVoiceRadioTechnology, rilDataRadioTechnology;
    Int32 networkId, systemId;
    Int32 cdmaRoamingIndicator, cdmaDefaultRoamingIndicator, cdmaEriIconIndex, cdmaEriIconMode;
    Boolean roaming, isManualNetworkSelection, isEmergencyOnly, cssIndicator;
    String operatorAlphaLong, operatorAlphaShort, operatorNumeric;

    s->GetVoiceRegState(&voiceRegState);
    s->GetDataRegState(&dataRegState);
    s->GetRoaming(&roaming);
    s->GetIsManualSelection(&isManualNetworkSelection);
    s->GetOperatorAlphaLong(&operatorAlphaLong);
    s->GetOperatorAlphaShort(&operatorAlphaShort);
    s->GetOperatorNumeric(&operatorNumeric);
    s->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
    s->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    Int32 val;
    s->GetCssIndicator(&val);
    cssIndicator = val;
    s->GetNetworkId(&networkId);
    s->GetSystemId(&systemId);
    s->GetCdmaRoamingIndicator(&cdmaRoamingIndicator);
    s->GetCdmaDefaultRoamingIndicator(&cdmaDefaultRoamingIndicator);
    s->GetCdmaEriIconIndex(&cdmaEriIconIndex);
    s->GetCdmaEriIconMode(&cdmaEriIconMode);
    s->IsEmergencyOnly(&isEmergencyOnly);

    mVoiceRegState = voiceRegState;
    mDataRegState = dataRegState;
    mRoaming = roaming;
    mOperatorAlphaLong = operatorAlphaLong;
    mOperatorAlphaShort = operatorAlphaShort;
    mOperatorNumeric = operatorNumeric;
    mIsManualNetworkSelection = isManualNetworkSelection;
    mRilVoiceRadioTechnology = rilVoiceRadioTechnology;
    mRilDataRadioTechnology = rilDataRadioTechnology;
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

ECode CServiceState::SetNullState(
    /* [in] */ Int32 state)
{
    if (DBG) Slogger::D(TAG, "[ServiceState] setNullState=%d", state);

    mVoiceRegState = state;
    mDataRegState = state;
    mRoaming = FALSE;
    mOperatorAlphaLong = String(NULL);
    mOperatorAlphaShort = String(NULL);
    mOperatorNumeric = String(NULL);
    mIsManualNetworkSelection = FALSE;
    mRilVoiceRadioTechnology = 0;
    mRilDataRadioTechnology = 0;
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

Boolean CServiceState::EqualsHandlesNulls(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    return (a.IsNull()) ? (b.IsNull()) : a.Equals(b);
}

CARAPI CServiceState::SetFromNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->GetInt32(String("voiceRegState"), &mVoiceRegState);
    m->GetInt32(String("dataRegState"), &mDataRegState);
    m->GetBoolean(String("roaming"), &mRoaming);
    m->GetString(String("operator-alpha-long"), &mOperatorAlphaLong);
    m->GetString(String("operator-alpha-short"), &mOperatorAlphaShort);
    m->GetString(String("operator-numeric"), &mOperatorNumeric);
    m->GetBoolean(String("manual"), &mIsManualNetworkSelection);
    m->GetInt32(String("radioTechnology"), &mRilVoiceRadioTechnology);
    m->GetInt32(String("dataRadioTechnology"), &mRilDataRadioTechnology);
    m->GetBoolean(String("cssIndicator"), &mCssIndicator);
    m->GetInt32(String("networkId"), &mNetworkId);
    m->GetInt32(String("systemId"), &mSystemId);
    m->GetInt32(String("cdmaRoamingIndicator"), &mCdmaRoamingIndicator);
    m->GetInt32(String("cdmaDefaultRoamingIndicator"), &mCdmaDefaultRoamingIndicator);
    m->GetBoolean(String("emergencyOnly"), &mIsEmergencyOnly);
    return NOERROR;
}

ECode CServiceState::RilRadioTechnologyToNetworkType(
    /* [in] */ Int32 rt,
    /* [out] */ Int32* networkType)
{
    VALIDATE_NOT_NULL(networkType);
    switch(rt) {

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
        case IServiceState::RIL_RADIO_TECHNOLOGY_GSM:
            *networkType = ITelephonyManager::NETWORK_TYPE_GSM;
            return NOERROR;
        default:
            *networkType = ITelephonyManager::NETWORK_TYPE_UNKNOWN;
            return NOERROR;
    }
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

