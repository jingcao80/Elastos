
#ifndef __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__
#define __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A UNIX-domain (AF_LOCAL) socket address. For use with
 * android.net.LocalSocket and android.net.LocalServerSocket.
 *
 * On the Android system, these names refer to names in the Linux
 * abstract (non-filesystem) UNIX domain namespace.
 */
class LocalSocketAddress
    : public Object
    , public ILocalSocketAddress
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an instance with a given name.
     *
     * @param name non-null name
     * @param namespace namespace the name should be created in.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ LocalSocketAddressNamespace ns);

    /**
     * Creates an instance with a given name in the {@link Namespace#ABSTRACT}
     * namespace
     *
     * @param name non-null name
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Retrieves the string name of this address
     * @return string name
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Returns the namespace used by this address.
     *
     * @return non-null a namespace
     */
    CARAPI GetNamespace(
        /* [out] */ LocalSocketAddressNamespace* result);

private:
    String mName;

    LocalSocketAddressNamespace mNamespace;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__
