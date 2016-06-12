
#include "elastos/droid/internal/telephony/dataconnection/ApnSetting.h"

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        this.id = id;
        this.numeric = numeric;
        this.carrier = carrier;
        this.apn = apn;
        this.proxy = proxy;
        this.port = port;
        this.mmsc = mmsc;
        this.mmsProxy = mmsProxy;
        this.mmsPort = mmsPort;
        this.user = user;
        this.password = password;
        this.authType = authType;
        this.types = new String[types.length];
        for (int i = 0; i < types.length; i++) {
            this.types[i] = types[i].toLowerCase(Locale.ROOT);
        }
        this.protocol = protocol;
        this.roamingProtocol = roamingProtocol;
        this.carrierEnabled = carrierEnabled;
        this.bearer = bearer;
        this.profileId = profileId;
        this.modemCognitive = modemCognitive;
        this.maxConns = maxConns;
        this.waitTime = waitTime;
        this.maxConnsTime = maxConnsTime;
        this.mtu = mtu;
        this.mvnoType = mvnoType;
        this.mvnoMatchData = mvnoMatchData;

#endif
}

ECode ApnSetting::FromString(
    /* [in] */ const String& data,
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (data == null) return null;
        int version;
        // matches() operates on the whole string, so append .* to the regex.
        if (data.matches(V3_FORMAT_REGEX + ".*")) {
            version = 3;
            data = data.replaceFirst(V3_FORMAT_REGEX, "");
        } else if (data.matches(V2_FORMAT_REGEX + ".*")) {
            version = 2;
            data = data.replaceFirst(V2_FORMAT_REGEX, "");
        } else {
            version = 1;
        }
        String[] a = data.split("\\s*,\\s*");
        if (a.length < 14) {
            return null;
        }
        int authType;
        try {
            authType = Integer.parseInt(a[12]);
        } catch (NumberFormatException e) {
            authType = 0;
        }
        String[] typeArray;
        String protocol, roamingProtocol;
        boolean carrierEnabled;
        int bearer = 0;
        int profileId = 0;
        boolean modemCognitive = false;
        int maxConns = 0;
        int waitTime = 0;
        int maxConnsTime = 0;
        int mtu = PhoneConstants.UNSET_MTU;
        String mvnoType = "";
        String mvnoMatchData = "";
        if (version == 1) {
            typeArray = new String[a.length - 13];
            System.arraycopy(a, 13, typeArray, 0, a.length - 13);
            protocol = RILConstants.SETUP_DATA_PROTOCOL_IP;
            roamingProtocol = RILConstants.SETUP_DATA_PROTOCOL_IP;
            carrierEnabled = true;
            bearer = 0;
        } else {
            if (a.length < 18) {
                return null;
            }
            typeArray = a[13].split("\\s*\\|\\s*");
            protocol = a[14];
            roamingProtocol = a[15];
            carrierEnabled = Boolean.parseBoolean(a[16]);
            try {
                bearer = Integer.parseInt(a[17]);
            } catch (NumberFormatException ex) {
            }
            if (a.length > 22) {
                modemCognitive = Boolean.parseBoolean(a[19]);
                try {
                    profileId = Integer.parseInt(a[18]);
                    maxConns = Integer.parseInt(a[20]);
                    waitTime = Integer.parseInt(a[21]);
                    maxConnsTime = Integer.parseInt(a[22]);
                } catch (NumberFormatException e) {
                }
            }
            if (a.length > 23) {
                try {
                    mtu = Integer.parseInt(a[23]);
                } catch (NumberFormatException e) {
                }
            }
            if (a.length > 25) {
                mvnoType = a[24];
                mvnoMatchData = a[25];
            }
        }
        return new ApnSetting(-1,a[10]+a[11],a[0],a[1],a[2],a[3],a[7],a[8],
                a[9],a[4],a[5],authType,typeArray,protocol,roamingProtocol,carrierEnabled,bearer,
                profileId, modemCognitive, maxConns, waitTime, maxConnsTime, mtu,
                mvnoType, mvnoMatchData);

#endif
}

ECode ApnSetting::ArrayFromString(
    /* [in] */ const String& data,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<ApnSetting> retVal = new ArrayList<ApnSetting>();
        if (TextUtils.isEmpty(data)) {
            return retVal;
        }
        String[] apnStrings = data.split("\\s*;\\s*");
        for (String apnString : apnStrings) {
            ApnSetting apn = fromString(apnString);
            if (apn != null) {
                retVal.add(apn);
            }
        }
        return retVal;

#endif
}

ECode ApnSetting::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        StringBuilder sb = new StringBuilder();
        sb.append("[ApnSettingV3] ")
        .append(carrier)
        .append(", ").append(id)
        .append(", ").append(numeric)
        .append(", ").append(apn)
        .append(", ").append(proxy)
        .append(", ").append(mmsc)
        .append(", ").append(mmsProxy)
        .append(", ").append(mmsPort)
        .append(", ").append(port)
        .append(", ").append(authType).append(", ");
        for (int i = 0; i < types.length; i++) {
            sb.append(types[i]);
            if (i < types.length - 1) {
                sb.append(" | ");
            }
        }
        sb.append(", ").append(protocol);
        sb.append(", ").append(roamingProtocol);
        sb.append(", ").append(carrierEnabled);
        sb.append(", ").append(bearer);
        sb.append(", ").append(profileId);
        sb.append(", ").append(modemCognitive);
        sb.append(", ").append(maxConns);
        sb.append(", ").append(waitTime);
        sb.append(", ").append(maxConnsTime);
        sb.append(", ").append(mtu);
        sb.append(", ").append(mvnoType);
        sb.append(", ").append(mvnoMatchData);
        return sb.toString();

#endif
}

ECode ApnSetting::HasMvnoParams(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return !TextUtils.isEmpty(mvnoType) && !TextUtils.isEmpty(mvnoMatchData);

#endif
}

ECode ApnSetting::CanHandleType(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!carrierEnabled) return false;
        for (String t : types) {
            // DEFAULT handles all, and HIPRI is handled by DEFAULT
            if (t.equalsIgnoreCase(type) ||
                    t.equalsIgnoreCase(PhoneConstants.APN_TYPE_ALL) ||
                    (t.equalsIgnoreCase(PhoneConstants.APN_TYPE_DEFAULT) &&
                    type.equalsIgnoreCase(PhoneConstants.APN_TYPE_HIPRI))) {
                return true;
            }
        }
        return false;

#endif
}

ECode ApnSetting::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (o instanceof ApnSetting == false) return false;
        return (toString().equals(o.toString()));

#endif
}

ECode ApnSetting::GetApnProfileType(
    /* [out] */ ApnProfileType* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ApnProfileType.PROFILE_TYPE_APN;

#endif
}

ECode ApnSetting::GetProfileId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return profileId;

#endif
}

ECode ApnSetting::ToShortString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return "ApnSetting";

#endif
}

ECode ApnSetting::ToHash(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return this.toString();

#endif
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
    /* [out] */ ArrayOf<String>** result)
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
