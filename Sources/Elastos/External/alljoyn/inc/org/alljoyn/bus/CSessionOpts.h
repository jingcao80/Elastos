
#ifndef __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__
#define __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__

#include "_Org_Alljoyn_Bus_CSessionOpts.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CSessionOpts)
    , public Object
    , public ISessionOpts
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Byte traffic,
        /* [in] */ Boolean isMultipoint,
        /* [in] */ Byte proximity,
        /* [in] */ Int16 transports);

    CARAPI GetTraffic(
        /* [out] */ Byte* traffic);

    CARAPI SetTraffic(
        /* [in] */ Byte traffic);

    CARAPI IsMultipoint(
        /* [out] */ Boolean* isMultipoint);

    CARAPI SetMultipoint(
        /* [in] */ Boolean multipoint);

    CARAPI GetProximity(
        /* [out] */ Byte* proximity);

    CARAPI SetProximity(
        /* [in] */ Byte proximity);

    CARAPI GetTransports(
        /* [out] */ Int16* transports);

    CARAPI SetTransports(
        /* [in] */ Int16 transports);

private:
    /**
     * Holds the traffic type for this SessionOpt
     */
    Byte mTraffic;

    /**
     * Multi-point session capable.
     *
     * A session is multi-point if it can be joined multiple times to form a single
     * session with multi (greater than 2) endpoints. When false, each join attempt
     * creates a new point-to-point session.
     */
    Boolean mIsMultipoint;

    /**
     * Holds the proximity for this SessionOpt
     */
    Byte mProximity;

    /**
     * Holds the allowed transports for this SessionOpt
     */
    Int16 mTransports;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__
