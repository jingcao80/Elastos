
#include "elastos/droid/internal/telephony/dataconnection/ApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSetting.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(ApnSetting, Object, IApnSetting)

const String ApnSetting::V2_FORMAT_REGEX("^\\[ApnSettingV2\\]\\s*");
const String ApnSetting::V3_FORMAT_REGEX("^\\[ApnSettingV3\\]\\s*");

ApnSetting::ApnSetting()
    : mAuthType(0)
    , mId(0)
    , mMtu(0)
    , mCarrierEnabled(FALSE)
    , mBearer(0)
    , mProfileId(0)
    , mModemCognitive(FALSE)
    , mMaxConns(0)
    , mWaitTime(0)
    , mMaxConnsTime(0)
{}

ECode ApnSetting::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& numeric,
    /* [in] */ const String& carrier,
    /* [in] */ const String& apn,
    /* [in] */ const String& proxy,
    /* [in] */ const String& port,
    /* [in] */ const String& mmsc,
    /* [in] */ const String& mmsProxy,
    /* [in] */ const String& mmsPort,
    /* [in] */ const String& user,
    /* [in] */ const String& password,
    /* [in] */ Int32 authType,
    /* [in] */ ArrayOf<String>* types,
    /* [in] */ const String& protocol,
    /* [in] */ const String& roamingProtocol,
    /* [in] */ Boolean carrierEnabled,
    /* [in] */ Int32 bearer,
    /* [in] */ Int32 profileId,
    /* [in] */ Boolean modemCognitive,
    /* [in] */ Int32 maxConns,
    /* [in] */ Int32 waitTime,
    /* [in] */ Int32 maxConnsTime,
    /* [in] */ Int32 mtu,
    /* [in] */ const String& mvnoType,
    /* [in] */ const String& mvnoMatchData)
{
    mId = id;
    mNumeric = numeric;
    mCarrier = carrier;
    mApn = apn;
    mProxy = proxy;
    mPort = port;
    mMmsc = mmsc;
    mMmsProxy = mmsProxy;
    mMmsPort = mmsPort;
    mUser = user;
    mPassword = password;
    mAuthType = authType;
    mTypes = ArrayOf<String>::Alloc(types->GetLength());
    for (Int32 i = 0; i < types->GetLength(); i++) {
        (*mTypes)[i] = (*types)[i].ToLowerCase();
    }
    mProtocol = protocol;
    mRoamingProtocol = roamingProtocol;
    mCarrierEnabled = carrierEnabled;
    mBearer = bearer;
    mProfileId = profileId;
    mModemCognitive = modemCognitive;
    mMaxConns = maxConns;
    mWaitTime = waitTime;
    mMaxConnsTime = maxConnsTime;
    mMtu = mtu;
    mMvnoType = mvnoType;
    mMvnoMatchData = mvnoMatchData;
    return NOERROR;
}

ECode ApnSetting::FromString(
    /* [in] */ const String& _data,
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    String data = _data;
    if (data.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    Int32 version;
    // matches() operates on the whole string, so append .* to the regex.
    Boolean isMatchesV3;
    StringUtils::Matches(data, V3_FORMAT_REGEX + ".*", &isMatchesV3);
    Boolean isMatchesV2;
    StringUtils::Matches(data, V2_FORMAT_REGEX + ".*", &isMatchesV2);
    if (isMatchesV3) {
        version = 3;
        String s;
        StringUtils::ReplaceFirst(data, V3_FORMAT_REGEX, "", &s);
        data = s;
    } else if (isMatchesV2) {
        version = 2;
        String s;
        StringUtils::ReplaceFirst(data, V2_FORMAT_REGEX, "", &s);
        data = s;
    } else {
        version = 1;
    }
    AutoPtr<ArrayOf<String> > a;
    StringUtils::Split(data, "\\s*,\\s*", (ArrayOf<String>**)&a);
    if (a->GetLength() < 14) {
        *result = NULL;
        return NOERROR;
    }
    Int32 authType;
    // try {
    ECode ec = StringUtils::Parse((*a)[12], &authType);
    // } catch (NumberFormatException e) {
    if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
        authType = 0;
    }
    // }
    AutoPtr<ArrayOf<String> > typeArray;
    String protocol, roamingProtocol;
    Boolean carrierEnabled;
    Int32 bearer = 0;
    Int32 profileId = 0;
    Boolean modemCognitive = FALSE;
    Int32 maxConns = 0;
    Int32 waitTime = 0;
    Int32 maxConnsTime = 0;
    Int32 mtu = IPhoneConstants::UNSET_MTU;
    String mvnoType("");
    String mvnoMatchData("");
    if (version == 1) {
        typeArray = ArrayOf<String>::Alloc(a->GetLength() - 13);
        typeArray->Copy(0, a, 13, a->GetLength() - 13);
        protocol = IRILConstants::SETUP_DATA_PROTOCOL_IP;
        roamingProtocol = IRILConstants::SETUP_DATA_PROTOCOL_IP;
        carrierEnabled = TRUE;
        bearer = 0;
    } else {
        if (a->GetLength() < 18) {
            *result = NULL;
            return NOERROR;
        }
        StringUtils::Split((*a)[13], "\\s*\\|\\s*", (ArrayOf<String>**)&typeArray);
        protocol = (*a)[14];
        roamingProtocol = (*a)[15];
        carrierEnabled = StringUtils::ParseBoolean((*a)[16]);
        // try {
        StringUtils::Parse((*a)[17], &bearer);
        // } catch (NumberFormatException ex) {
        // }
        if (a->GetLength() > 22) {
            modemCognitive = StringUtils::ParseBoolean((*a)[19]);
            // try {
            StringUtils::Parse((*a)[18], &profileId);
            StringUtils::Parse((*a)[20], &maxConns);
            StringUtils::Parse((*a)[21], &waitTime);
            StringUtils::Parse((*a)[22], &maxConnsTime);
            // } catch (NumberFormatException e) {
            // }
        }
        if (a->GetLength() > 23) {
            // try {
            StringUtils::Parse((*a)[23], &mtu);
            // } catch (NumberFormatException e) {
            // }
        }
        if (a->GetLength() > 25) {
            mvnoType = (*a)[24];
            mvnoMatchData = (*a)[25];
        }
    }
    return CApnSetting::New(-1,(*a)[10]+(*a)[11],(*a)[0],(*a)[1],(*a)[2],(*a)[3],(*a)[7],(*a)[8],
            (*a)[9],(*a)[4],(*a)[5],authType,typeArray,protocol,roamingProtocol,carrierEnabled,bearer,
            profileId, modemCognitive, maxConns, waitTime, maxConnsTime, mtu,
            mvnoType, mvnoMatchData, result);
}

ECode ApnSetting::ArrayFromString(
    /* [in] */ const String& data,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> retVal;
    CArrayList::New((IList**)&retVal);
    if (TextUtils::IsEmpty(data)) {
        *result = retVal;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > apnStrings;
    StringUtils::Split(data, "\\s*;\\s*", (ArrayOf<String>**)&apnStrings);
    for (Int32 i = 0; i < apnStrings->GetLength(); ++i) {
        String apnString = (*apnStrings)[i];
        AutoPtr<IApnSetting> apn;
        FromString(apnString, (IApnSetting**)&apn);
        if (apn != NULL) {
            retVal->Add(apn);
        }
    }
    *result = retVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnSetting::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("[ApnSettingV3] ");
    sb.Append(mCarrier);
    sb.Append(", ");
    sb.Append(mId);
    sb.Append(", ");
    sb.Append(mNumeric);
    sb.Append(", ");
    sb.Append(mApn);
    sb.Append(", ");
    sb.Append(mProxy);
    sb.Append(", ");
    sb.Append(mMmsc);
    sb.Append(", ");
    sb.Append(mMmsProxy);
    sb.Append(", ");
    sb.Append(mMmsPort);
    sb.Append(", ");
    sb.Append(mPort);
    sb.Append(", ");
    sb.Append(mAuthType);
    sb.Append(", ");
    for (Int32 i = 0; i < mTypes->GetLength(); i++) {
        sb.Append((*mTypes)[i]);
        if (i < mTypes->GetLength() - 1) {
            sb.Append(" | ");
        }
    }
    sb.Append(", ");
    sb.Append(mProtocol);
    sb.Append(", ");
    sb.Append(mRoamingProtocol);
    sb.Append(", ");
    sb.Append(mCarrierEnabled);
    sb.Append(", ");
    sb.Append(mBearer);
    sb.Append(", ");
    sb.Append(mProfileId);
    sb.Append(", ");
    sb.Append(mModemCognitive);
    sb.Append(", ");
    sb.Append(mMaxConns);
    sb.Append(", ");
    sb.Append(mWaitTime);
    sb.Append(", ");
    sb.Append(mMaxConnsTime);
    sb.Append(", ");
    sb.Append(mMtu);
    sb.Append(", ");
    sb.Append(mMvnoType);
    sb.Append(", ");
    sb.Append(mMvnoMatchData);
    return sb.ToString(result);
}

ECode ApnSetting::HasMvnoParams(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = !TextUtils::IsEmpty(mMvnoType) && !TextUtils::IsEmpty(mMvnoMatchData);
    return NOERROR;
}

ECode ApnSetting::CanHandleType(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mCarrierEnabled) {
        *result = FALSE;
        return NOERROR;
    }
    for (Int32 i = 0; i < mTypes->GetLength(); ++i) {
        String t = (*mTypes)[i];
        // DEFAULT handles all, and HIPRI is handled by DEFAULT
        if (t.EqualsIgnoreCase(type) ||
                t.EqualsIgnoreCase(IPhoneConstants::APN_TYPE_ALL) ||
                (t.EqualsIgnoreCase(IPhoneConstants::APN_TYPE_DEFAULT) &&
                type.EqualsIgnoreCase(IPhoneConstants::APN_TYPE_HIPRI))) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ApnSetting::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IApnSetting::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    String thisStr;
    ToString(&thisStr);
    *result = (thisStr.Equals(TO_STR(o)));
    return NOERROR;
}

ECode ApnSetting::GetApnProfileType(
    /* [out] */ ApnProfileType* result)
{
    VALIDATE_NOT_NULL(result)

    *result = PROFILE_TYPE_APN;
    return NOERROR;
}

ECode ApnSetting::GetProfileId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProfileId;
    return NOERROR;
}

ECode ApnSetting::ToShortString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("ApnSetting");
    return NOERROR;
}

ECode ApnSetting::ToHash(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return ToString(result);
}

ECode ApnSetting::GetCarrier(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrier;
    return NOERROR;
}

ECode ApnSetting::GetApn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mApn;
    return NOERROR;
}

ECode ApnSetting::GetProxy(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProxy;
    return NOERROR;
}

ECode ApnSetting::GetPort(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPort;
    return NOERROR;
}

ECode ApnSetting::GetMmsc(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMmsc;
    return NOERROR;
}

ECode ApnSetting::GetMmsProxy(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMmsProxy;
    return NOERROR;
}

ECode ApnSetting::GetMmsPort(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMmsPort;
    return NOERROR;
}

ECode ApnSetting::GetUser(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUser;
    return NOERROR;
}

ECode ApnSetting::GetPassword(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPassword;
    return NOERROR;
}

ECode ApnSetting::GetAuthType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAuthType;
    return NOERROR;
}

ECode ApnSetting::GetTypes(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTypes;
    return NOERROR;
}

ECode ApnSetting::SetTypes(
    /* [in] */ ArrayOf<String>* types)
{
    mTypes = types;
    return NOERROR;
}

ECode ApnSetting::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode ApnSetting::GetNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumeric;
    return NOERROR;
}

ECode ApnSetting::GetProtocol(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProtocol;
    return NOERROR;
}

ECode ApnSetting::GetRoamingProtocol(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRoamingProtocol;
    return NOERROR;
}

ECode ApnSetting::GetMtu(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMtu;
    return NOERROR;
}

ECode ApnSetting::GetCarrierEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrierEnabled;
    return NOERROR;
}

ECode ApnSetting::GetBearer(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBearer;
    return NOERROR;
}

ECode ApnSetting::SetProfileId(
    /* [in] */ Int32 profileId)
{
    mProfileId = profileId;
    return NOERROR;
}

ECode ApnSetting::GetModemCognitive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mModemCognitive;
    return NOERROR;
}

ECode ApnSetting::GetMaxConns(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxConns;
    return NOERROR;
}

ECode ApnSetting::GetWaitTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWaitTime;
    return NOERROR;
}

ECode ApnSetting::GetMaxConnsTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxConnsTime;
    return NOERROR;
}

ECode ApnSetting::GetMvnoType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMvnoType;
    return NOERROR;
}

ECode ApnSetting::GetMvnoMatchData(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMvnoMatchData;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
