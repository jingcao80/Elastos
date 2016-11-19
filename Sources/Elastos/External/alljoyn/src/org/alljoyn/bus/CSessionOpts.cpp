
#include "org/alljoyn/bus/CSessionOpts.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CSessionOpts, Object, ISessionOpts)

CAR_OBJECT_IMPL(CSessionOpts)

ECode CSessionOpts::constructor()
{
    mTraffic = TRAFFIC_MESSAGES;
    mIsMultipoint = FALSE;
    mProximity = PROXIMITY_ANY;
    mTransports = TRANSPORT_ANY;
    return NOERROR;
}

ECode CSessionOpts::constructor(
    /* [in] */ Byte traffic,
    /* [in] */ Boolean isMultipoint,
    /* [in] */ Byte proximity,
    /* [in] */ Int16 transports)
{
    mTraffic = traffic;
    mIsMultipoint = isMultipoint;
    mProximity = proximity;
    mTransports = transports;
    return NOERROR;
}

ECode CSessionOpts::GetTraffic(
    /* [out] */ Byte* traffic)
{
    VALIDATE_NOT_NULL(traffic);
    *traffic = mTraffic;
    return NOERROR;
}

ECode CSessionOpts::SetTraffic(
    /* [in] */ Byte traffic)
{
    mTraffic = traffic;
    return NOERROR;
}

ECode CSessionOpts::IsMultipoint(
    /* [out] */ Boolean* isMultipoint)
{
    VALIDATE_NOT_NULL(isMultipoint);
    *isMultipoint = mIsMultipoint;
    return NOERROR;
}

ECode CSessionOpts::SetMultipoint(
    /* [in] */ Boolean multipoint)
{
    mIsMultipoint = multipoint;
    return NOERROR;
}

ECode CSessionOpts::GetProximity(
    /* [out] */ Byte* proximity)
{
    VALIDATE_NOT_NULL(proximity);
    *proximity = mProximity;
    return NOERROR;
}

ECode CSessionOpts::SetProximity(
    /* [in] */ Byte proximity)
{
    mProximity = proximity;
    return NOERROR;
}

ECode CSessionOpts::GetTransports(
    /* [out] */ Int16* transports)
{
    VALIDATE_NOT_NULL(transports);
    *transports = mTransports;
    return NOERROR;
}

ECode CSessionOpts::SetTransports(
    /* [in] */ Int16 transports)
{
    mTransports = transports;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
