#include "elastos/droid/telephony/cdma/CCdmaCellLocation.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaCellLocation, CellLocation, ICdmaCellLocation)

CAR_OBJECT_IMPL(CCdmaCellLocation)

CCdmaCellLocation::CCdmaCellLocation()
    : mBaseStationId(-1)
    , mBaseStationLatitude(ICdmaCellLocation::INVALID_LAT_LONG)
    , mBaseStationLongitude(ICdmaCellLocation::INVALID_LAT_LONG)
    , mSystemId(-1)
    , mNetworkId(-1)
{
}

CCdmaCellLocation::~CCdmaCellLocation()
{
}

ECode CCdmaCellLocation::constructor()
{
    return NOERROR;
}

ECode CCdmaCellLocation::constructor(
    /* [in] */ IBundle* bundle)
{
    this->mBaseStationId = bundle->GetInt32(String("baseStationId"), &mBaseStationId);
    this->mBaseStationLatitude = bundle->GetInt32(String("baseStationLatitude"), &mBaseStationLatitude);
    this->mBaseStationLongitude = bundle->GetInt32(String("baseStationLongitude"), &mBaseStationLongitude);
    this->mSystemId = bundle->GetInt32(String("systemId"), &mSystemId);
    this->mNetworkId = bundle->GetInt32(String("networkId"), &mNetworkId);
    return NOERROR;
}

ECode CCdmaCellLocation::FillInNotifierBundle(
    /* [in] */ IBundle* bundle)
{
    bundle->PutInt32(String("baseStationId"), this->mBaseStationId);
    bundle->PutInt32(String("baseStationLatitude"), this->mBaseStationLatitude);
    bundle->PutInt32(String("baseStationLongitude"), this->mBaseStationLongitude);
    bundle->PutInt32(String("systemId"), this->mSystemId);
    bundle->PutInt32(String("networkId"), this->mNetworkId);
    return NOERROR;
}

ECode CCdmaCellLocation::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (this->mBaseStationId == -1 &&
            this->mBaseStationLatitude == INVALID_LAT_LONG &&
            this->mBaseStationLongitude == INVALID_LAT_LONG &&
            this->mSystemId == -1 &&
            this->mNetworkId == -1);
    return NOERROR;
}

ECode CCdmaCellLocation::GetBaseStationId(
    /* [out] */ Int32* baseStationId)
{
    VALIDATE_NOT_NULL(baseStationId);
    *baseStationId = this->mBaseStationId;
    return NOERROR;
}

ECode CCdmaCellLocation::GetBaseStationLatitude(
    /* [out] */ Int32* baseStationLatitude)
{
    VALIDATE_NOT_NULL(baseStationLatitude);
    *baseStationLatitude = mBaseStationLatitude;
    return NOERROR;
}

ECode CCdmaCellLocation::GetBaseStationLongitude(
    /* [out] */ Int32* baseStationLongitude)
{
    VALIDATE_NOT_NULL(baseStationLongitude);
    *baseStationLongitude = mBaseStationLongitude;
    return NOERROR;
}

ECode CCdmaCellLocation::GetSystemId(
    /* [out] */ Int32* systemid)
{
    VALIDATE_NOT_NULL(systemid);
    *systemid = mSystemId;
    return NOERROR;
}

ECode CCdmaCellLocation::GetNetworkId(
    /* [out] */ Int32* networkid)
{
    VALIDATE_NOT_NULL(networkid);
    *networkid = mNetworkId;
    return NOERROR;
}

ECode CCdmaCellLocation::SetStateInvalid()
{
    this->mBaseStationId = -1;
    this->mBaseStationLatitude = INVALID_LAT_LONG;
    this->mBaseStationLongitude = INVALID_LAT_LONG;
    this->mSystemId = -1;
    this->mNetworkId = -1;
    return NOERROR;
}

ECode CCdmaCellLocation::SetCellLocationData(
    /* [in] */ Int32 baseStationId,
    /* [in] */ Int32 baseStationLatitude,
    /* [in] */ Int32 baseStationLongitude)
{
    // The following values have to be written in the correct sequence
    this->mBaseStationId = baseStationId;
    this->mBaseStationLatitude = baseStationLatitude;   //values[2];
    this->mBaseStationLongitude = baseStationLongitude; //values[3];
    return NOERROR;
}

ECode CCdmaCellLocation::SetCellLocationData(
    /* [in] */ Int32 baseStationId,
    /* [in] */ Int32 baseStationLatitude,
    /* [in] */ Int32 baseStationLongitude,
    /* [in] */ Int32 systemId,
    /* [in] */ Int32 networkId)
{
    // The following values have to be written in the correct sequence
    this->mBaseStationId = baseStationId;
    this->mBaseStationLatitude = baseStationLatitude;   //values[2];
    this->mBaseStationLongitude = baseStationLongitude; //values[3];
    this->mSystemId = systemId;
    this->mNetworkId = networkId;
    return NOERROR;
}

ECode CCdmaCellLocation::ConvertQuartSecToDecDegrees(
    /* [in] */ Int32 quartSec,
    /* [out] */ Double* val)
{
    VALIDATE_NOT_NULL(val);
    AutoPtr<IDouble> adouble;
    CDouble::New(quartSec, (IDouble**)&adouble);
    Boolean isflag = FALSE;
    if((adouble->IsNaN(&isflag), isflag) || quartSec < -2592000 || quartSec > 2592000){
        // Invalid value
        //throw new IllegalArgumentException("Invalid coordiante value:" + quartSec);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //((double)quartSec) / (3600 * 4));
    *val = ((Double)quartSec) / (3600 * 4);

    return NOERROR;
}

ECode CCdmaCellLocation::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (this->mBaseStationId ^ this->mBaseStationLatitude ^ this->mBaseStationLongitude
        ^ this->mSystemId ^ this->mNetworkId);
    return NOERROR;
}

ECode CCdmaCellLocation::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaCellLocation> s;

    //try {
    s = ICdmaCellLocation::Probe(o);
    // } catch (ClassCastException ex) {
    //     return false;
    // }

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 baseStationId, baseStationLatitude, baseStationLongitude, systemId, networkId;
    s->GetBaseStationId(&baseStationId);
    s->GetBaseStationLatitude(&baseStationLatitude);
    s->GetBaseStationLongitude(&baseStationLongitude);
    s->GetSystemId(&systemId);
    s->GetNetworkId(&networkId);

    *result = (EqualsHandlesNulls(this->mBaseStationId, baseStationId) &&
            EqualsHandlesNulls(this->mBaseStationLatitude, baseStationLatitude) &&
            EqualsHandlesNulls(this->mBaseStationLongitude, baseStationLongitude) &&
            EqualsHandlesNulls(this->mSystemId, systemId) &&
            EqualsHandlesNulls(this->mNetworkId, networkId)
    );

    return NOERROR;
}

ECode CCdmaCellLocation::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("[");
    sb.Append(this->mBaseStationId);
    sb.Append(",");
    sb.Append(this->mBaseStationLatitude);
    sb.Append(",");
    sb.Append(this->mBaseStationLongitude);
    sb.Append(",");
    sb.Append(this->mSystemId);
    sb.Append(",");
    sb.Append(this->mNetworkId);
    sb.Append("]");
    *str = sb.ToString();
    return NOERROR;
}

Boolean CCdmaCellLocation::EqualsHandlesNulls(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b)
{
    return a == b;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

