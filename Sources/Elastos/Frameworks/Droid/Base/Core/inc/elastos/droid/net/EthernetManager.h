
#ifndef __ELASTOS_DROID_NET_ETHERNETMANAGER_H__
#define __ELASTOS_DROID_NET_ETHERNETMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing the IP configuration of the Ethernet network.
 *
 * @hide
 */
class EthernetManager
    : public Object
    , public IEthernetManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new EthernetManager instance.
     * Applications will almost always want to use
     * {@link android.content.Context#getSystemService Context.getSystemService()} to retrieve
     * the standard {@link android.content.Context#ETHERNET_SERVICE Context.ETHERNET_SERVICE}.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIEthernetManager* service);

    /**
     * Get Ethernet configuration.
     * @return the Ethernet Configuration, contained in {@link IpConfiguration}.
     */
    CARAPI GetConfiguration(
        /* [out] */ IIpConfiguration** result);

    /**
     * Set Ethernet configuration.
     */
    CARAPI SetConfiguration(
        /* [in] */ IIpConfiguration* config);

private:
    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IIEthernetManager> mService;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_ETHERNETMANAGER_H__
