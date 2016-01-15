
#ifndef __ELASTOS_DROID_NET_CLINKADDRESS_H__
#define __ELASTOS_DROID_NET_CLINKADDRESS_H__

#include "_Elastos_Droid_Net_CLinkAddress.h"
#include "LinkAddress.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Identifies an IP address on a network link.
 *
 * A {@code LinkAddress} consists of:
 * <ul>
 * <li>An IP address and prefix length (e.g., {@code 2001:db8::1/64} or {@code 192.0.2.1/24}).
 * The address must be unicast, as multicast addresses cannot be assigned to interfaces.
 * <li>Address flags: A bitmask of {@code OsConstants.IFA_F_*} values representing properties
 * of the address (e.g., {@code android.system.OsConstants.IFA_F_OPTIMISTIC}).
 * <li>Address scope: One of the {@code OsConstants.IFA_F_*} values; defines the scope in which
 * the address is unique (e.g.,
 * {@code android.system.OsConstants.RT_SCOPE_LINK} or
 * {@code android.system.OsConstants.RT_SCOPE_UNIVERSE}).
 * </ul>
 */
CarClass(CLinkAddress)
    , public LinkAddress
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLINKADDRESS_H__
