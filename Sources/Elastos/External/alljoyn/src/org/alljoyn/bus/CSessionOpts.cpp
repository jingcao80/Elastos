
#include "org/alljoyn/bus/CSessionOpts.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

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

ECode CSessionOpts::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder result;
    result.Append("CSessionOpts {");

    result.Append("traffic = ");
    String value = String::Format("(0x%02x)", mTraffic);
    result.Append(value);
    if ((mTraffic & TRAFFIC_MESSAGES) != 0) result.Append(" TRAFFIC_MESSAGES");
    if ((mTraffic & TRAFFIC_RAW_UNRELIABLE) != 0) result.Append(" TRAFFIC_RAW_UNRELIABLE");
    if ((mTraffic & TRAFFIC_RAW_RELIABLE) != 0) result.Append(" TRAFFIC_RAW_RELIABLE");

    result.Append(", isMultipoint = ");
    value = String::Format("%d", mIsMultipoint);
    result.Append(value);

    result.Append(", proximity =");
    value = String::Format("(0x%02x)", mProximity);
    result.Append(value);
    if ((mProximity & PROXIMITY_PHYSICAL) != 0) result.Append(" PROXIMITY_PHYSICAL");
    if ((mProximity & PROXIMITY_NETWORK) != 0) result.Append(" PROXIMITY_NETWORK");

    result.Append(", transports =");
    value = String::Format("(0x%04x)", mTransports);
    result.Append(value);
    if ((mTransports & TRANSPORT_LOCAL) != 0) result.Append(" TRANSPORT_LOCAL");
    if ((mTransports & TRANSPORT_TCP) != 0) result.Append(" TRANSPORT_TCP");
    if ((mTransports & TRANSPORT_UDP) != 0) result.Append(" TRANSPORT_UDP");

    result.Append("}");
    *str = result.ToString();
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
