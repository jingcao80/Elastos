
#include "elastos/droid/internal/telephony/dataconnection/DataProfile.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DataProfile, Object, IDataProfile)

const Int32 DataProfile::TYPE_COMMON = 0;
const Int32 DataProfile::TYPE_3GPP = 1;
const Int32 DataProfile::TYPE_3GPP2 = 2;

ECode DataProfile::constructor(
    /* [in] */ Int32 profileId,
    /* [in] */ const String& apn,
    /* [in] */ const String& protocol,
    /* [in] */ Int32 authType,
    /* [in] */ const String& user,
    /* [in] */ const String& password,
    /* [in] */ Int32 type,
    /* [in] */ Int32 maxConnsTime,
    /* [in] */ Int32 maxConns,
    /* [in] */ Int32 waitTime,
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        this.profileId = profileId;
        this.apn = apn;
        this.protocol = protocol;
        this.authType = authType;
        this.user = user;
        this.password = password;
        this.type = type;
        this.maxConnsTime = maxConnsTime;
        this.maxConns = maxConns;
        this.waitTime = waitTime;
        this.enabled = enabled;

#endif
}

ECode DataProfile::constructor(
    /* [in] */ IApnSetting* apn,
    /* [in] */ Boolean isRoaming)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        this(apn.profileId, apn.apn, isRoaming? apn.protocol : apn.roamingProtocol,
                apn.authType, apn.user, apn.password, apn.bearer == 0 ? TYPE_COMMON :
                (ServiceState.isCdma(apn.bearer) ? TYPE_3GPP2 : TYPE_3GPP), apn.maxConnsTime,
                apn.maxConns, apn.waitTime, apn.carrierEnabled);

#endif
}

ECode DataProfile::ToParcel(
    /* [in] */ IParcel* pc,
    /* [in] */ ArrayOf<IDataProfile*>* dps,
    /* [out] */ IParcel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(pc == null) {
            return null;
        }
        pc.writeInt(dps.length);
        for(int i = 0; i < dps.length; i++) {
            pc.writeInt(dps[i].profileId);
            pc.writeString(dps[i].apn);
            pc.writeString(dps[i].protocol);
            pc.writeInt(dps[i].authType);
            pc.writeString(dps[i].user);
            pc.writeString(dps[i].password);
            pc.writeInt(dps[i].type);
            pc.writeInt(dps[i].maxConnsTime);
            pc.writeInt(dps[i].maxConns);
            pc.writeInt(dps[i].waitTime);
            pc.writeInt(dps[i].enabled ? 1 : 0);
        }
        return pc;

#endif
}

ECode DataProfile::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return "DataProfile " + profileId + "/" + apn + "/" + protocol + "/" + authType
                + "/" + user + "/" + password + "/" + type + "/" + maxConnsTime
                + "/" + maxConns + "/" + waitTime + "/" + enabled;

#endif
}

ECode DataProfile::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (o instanceof DataProfile == false) return false;
        return (toString().equals(o.toString()));

#endif
}

ECode DataProfile::GetProfileId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProfileId;
    return NOERROR;
}

ECode DataProfile::GetApn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mApn;
    return NOERROR;
}

ECode DataProfile::GetProtocol(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProtocol;
    return NOERROR;
}

ECode DataProfile::GetAuthType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAuthType;
    return NOERROR;
}

ECode DataProfile::GetUser(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUser;
    return NOERROR;
}

ECode DataProfile::GetPassword(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPassword;
    return NOERROR;
}

ECode DataProfile::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode DataProfile::GetMaxConnsTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxConnsTime;
    return NOERROR;
}

ECode DataProfile::GetMaxConns(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxConns;
    return NOERROR;
}

ECode DataProfile::GetWaitTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWaitTime;
    return NOERROR;
}

ECode DataProfile::GetEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEnabled;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
