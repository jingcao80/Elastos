
#ifndef __ELASTOS_DROID_NET_CSTATICIPCONFIGURATION_H__
#define __ELASTOS_DROID_NET_CSTATICIPCONFIGURATION_H__

#include "_Elastos_Droid_Net_CStaticIpConfiguration.h"
#include "elastos/droid/net/StaticIpConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that describes static IP configuration.
 *
 * This class is different from LinkProperties because it represents
 * configuration intent. The general contract is that if we can represent
 * a configuration here, then we should be able to configure it on a network.
 * The intent is that it closely match the UI we have for configuring networks.
 *
 * In contrast, LinkProperties represents current state. It is much more
 * expressive. For example, it supports multiple IP addresses, multiple routes,
 * stacked interfaces, and so on. Because LinkProperties is so expressive,
 * using it to represent configuration intent as well as current state causes
 * problems. For example, we could unknowingly save a configuration that we are
 * not in fact capable of applying, or we could save a configuration that the
 * UI cannot display, which has the potential for malicious code to hide
 * hostile or unexpected configuration from the user: see, for example,
 * http://b/12663469 and http://b/16893413 .
 *
 * @hide
 */
CarClass(CStaticIpConfiguration)
    , public StaticIpConfiguration
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CSTATICIPCONFIGURATION_H__
