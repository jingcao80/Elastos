#include "elastos/droid/telephony/CCellIdentityCdma.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellIdentityCdma::TAG("CCellIdentityCdma");
const Boolean CCellIdentityCdma::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CCellIdentityCdma, Object, ICellIdentityCdma, IParcelable)

CAR_OBJECT_IMPL(CCellIdentityCdma)

CCellIdentityCdma::CCellIdentityCdma()
    : mNetworkId(Elastos::Core::Math::INT32_MAX_VALUE)
    , mSystemId(Elastos::Core::Math::INT32_MAX_VALUE)
    , mBasestationId(Elastos::Core::Math::INT32_MAX_VALUE)
    , mLongitude(Elastos::Core::Math::INT32_MAX_VALUE)
    , mLatitude(Elastos::Core::Math::INT32_MAX_VALUE)
{
}

CCellIdentityCdma::~CCellIdentityCdma()
{
}

ECode CCellIdentityCdma::constructor()
{
    return NOERROR;
}

ECode CCellIdentityCdma::constructor(
    /* [in] */ Int32 nid,
    /* [in] */ Int32 sid,
    /* [in] */ Int32 bid,
    /* [in] */ Int32 lon,
    /* [in] */ Int32 lat)
{
    mNetworkId = nid;
    mSystemId = sid;
    mBasestationId = bid;
    mLongitude = lon;
    mLatitude = lat;
    return NOERROR;
}

ECode CCellIdentityCdma::constructor(
    /* [in] */ ICellIdentityCdma* cid)
{
    Int32 nid, sid, bid, lon, lat;
    cid->GetNetworkId(&nid);
    cid->GetSystemId(&sid);
    cid->GetBasestationId(&bid);
    cid->GetLongitude(&lon);
    cid->GetLatitude(&lat);
    mNetworkId = nid;
    mSystemId = sid;
    mBasestationId = bid;
    mLongitude = lon;
    mLatitude = lat;
    return NOERROR;
}

ECode CCellIdentityCdma::Copy(
    /* [out] */ ICellIdentityCdma** cid)
{
    VALIDATE_NOT_NULL(cid);
    return CCellIdentityCdma::New((ICellIdentityCdma*)this, cid);
}

ECode CCellIdentityCdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNetworkId);
    source->ReadInt32(&mSystemId);
    source->ReadInt32(&mBasestationId);
    source->ReadInt32(&mLongitude);
    source->ReadInt32(&mLatitude);
    return NOERROR;
}

ECode CCellIdentityCdma::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (DBG) {
        String str;
        ToString(&str);
        Logger::I(TAG, String("writeToParcel(Parcel, int): ") + str);
    }
    dest->WriteInt32(mNetworkId);
    dest->WriteInt32(mSystemId);
    dest->WriteInt32(mBasestationId);
    dest->WriteInt32(mLongitude);
    dest->WriteInt32(mLatitude);
    return NOERROR;
}

ECode CCellIdentityCdma::GetNetworkId(
    /* [out] */ Int32* networkid)
{
    VALIDATE_NOT_NULL(networkid);
    *networkid = mNetworkId;
    return NOERROR;
}

ECode CCellIdentityCdma::GetSystemId(
    /* [out] */ Int32* systemid)
{
    VALIDATE_NOT_NULL(systemid);
    *systemid = mSystemId;
    return NOERROR;
}

ECode CCellIdentityCdma::GetBasestationId(
    /* [out] */ Int32* basestationid)
{
    VALIDATE_NOT_NULL(basestationid);
    *basestationid = mBasestationId;
    return NOERROR;
}

ECode CCellIdentityCdma::GetLongitude(
    /* [out] */ Int32* longitude)
{
    VALIDATE_NOT_NULL(longitude);
    *longitude = mLongitude;
    return NOERROR;
}

ECode CCellIdentityCdma::GetLatitude(
    /* [out] */ Int32* latitude)
{
    VALIDATE_NOT_NULL(latitude);
    *latitude = mLatitude;
    return NOERROR;
}

ECode CCellIdentityCdma::GetHashCode(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 primeNum = 31;
    *res = (mNetworkId * primeNum) + (mSystemId * primeNum) + (mBasestationId * primeNum) +
            (mLatitude * primeNum) + (mLongitude * primeNum);
    return NOERROR;
}

ECode CCellIdentityCdma::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    //try {
    AutoPtr<ICellIdentityCdma> o = ICellIdentityCdma::Probe(other);
    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 nid, sid, bid, lon, lat;
    o->GetNetworkId(&nid);
    o->GetSystemId(&sid);
    o->GetBasestationId(&bid);
    o->GetLongitude(&lon);
    o->GetLatitude(&lat);
    *res = (mNetworkId == nid &&
            mSystemId == sid &&
            mBasestationId == bid &&
            mLongitude == lon &&
            mLatitude == lat);

    return NOERROR;
    /*} catch (ClassCastException e) {
        *res = FALSE;
        return NOERROR;
    }*/

}

ECode CCellIdentityCdma::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellIdentityCdma:{");
    sb.Append(" mNetworkId="); sb.Append(mNetworkId);
    sb.Append(" mSystemId="); sb.Append(mSystemId);
    sb.Append(" mBasestationId="); sb.Append(mBasestationId);
    sb.Append(" mLongitude="); sb.Append(mLongitude);
    sb.Append(" mLatitude="); sb.Append(mLatitude);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
