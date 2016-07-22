
#include "elastos/droid/internal/telephony/dataconnection/DataProfile.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/telephony/CServiceState.h"

using Elastos::Droid::Telephony::CServiceState;

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
    mProfileId = profileId;
    mApn = apn;
    mProtocol = protocol;
    mAuthType = authType;
    mUser = user;
    mPassword = password;
    mType = type;
    mMaxConnsTime = maxConnsTime;
    mMaxConns = maxConns;
    mWaitTime = waitTime;
    mEnabled = enabled;
    return NOERROR;
}

ECode DataProfile::constructor(
    /* [in] */ IApnSetting* apn,
    /* [in] */ Boolean isRoaming)
{
    Int32 bearer;
    apn->GetBearer(&bearer);
    Boolean isCdma;
    CServiceState::IsCdma(bearer, &isCdma);
    Int32 profileId;
    apn->GetProfileId(&profileId);
    String apnName;
    apn->GetApn(&apnName);
    String protocol;
    apn->GetProtocol(&protocol);
    String roamingProtocol;
    apn->GetRoamingProtocol(&roamingProtocol);
    Int32 authType;
    apn->GetAuthType(&authType);
    String user;
    apn->GetUser(&user);
    String password;
    apn->GetPassword(&password);
    Int32 maxConnsTime;
    apn->GetMaxConnsTime(&maxConnsTime);
    Int32 maxConns;
    apn->GetMaxConns(&maxConns);
    Int32 waitTime;
    apn->GetWaitTime(&waitTime);
    Boolean carrierEnabled;
    apn->GetCarrierEnabled(&carrierEnabled);
    return constructor(profileId, apnName, isRoaming? protocol : roamingProtocol,
            authType, user, password, bearer == 0 ? TYPE_COMMON :
            (isCdma ? TYPE_3GPP2 : TYPE_3GPP), maxConnsTime,
            maxConns, waitTime, carrierEnabled);
}

ECode DataProfile::ToParcel(
    /* [in] */ IParcel* pc,
    /* [in] */ ArrayOf<IDataProfile*>* dps,
    /* [out] */ IParcel** result)
{
    VALIDATE_NOT_NULL(result)

    if (pc == NULL) {
        *result = NULL;
        return NOERROR;
    }
    pc->WriteInt32(dps->GetLength());
    for (Int32 i = 0; i < dps->GetLength(); i++) {
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mProfileId);
        pc->WriteString(((DataProfile*) (*dps)[i])->mApn);
        pc->WriteString(((DataProfile*) (*dps)[i])->mProtocol);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mAuthType);
        pc->WriteString(((DataProfile*) (*dps)[i])->mUser);
        pc->WriteString(((DataProfile*) (*dps)[i])->mPassword);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mType);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mMaxConnsTime);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mMaxConns);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mWaitTime);
        pc->WriteInt32(((DataProfile*) (*dps)[i])->mEnabled ? 1 : 0);
    }
    *result = pc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataProfile::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("DataProfile %d//%s/%s/%d/%s/%s/%d/%d/%d/%d/%d",
            mProfileId, mApn.string(), mProtocol.string(), mAuthType, mUser.string(),
            mPassword.string(), mType, mMaxConnsTime, mMaxConns, mWaitTime, mEnabled);
    *result = rev;
    return NOERROR;
}

ECode DataProfile::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    IDataProfile* dp = IDataProfile::Probe(o);
    if (dp == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (dp == (IDataProfile*)this) {
        *result = TRUE;
        return NOERROR;
    }

    String str;
    ToString(&str);
    String ostr = Object::ToString(dp);
    *result = str.Equals(ostr);
    return NOERROR;
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
