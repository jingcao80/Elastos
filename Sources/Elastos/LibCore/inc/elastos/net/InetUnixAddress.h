#ifndef __ELASTOS_NET_INETUNIXADDRESS_H__
#define __ELASTOS_NET_INETUNIXADDRESS_H__

#include "InetAddress.h"

namespace Elastos {
namespace Net {

/**
 * An AF_UNIX address. See {@link InetAddress}.
 * @hide
 */
class InetUnixAddress
    : public InetAddress
{
public:
    /**
     * Constructs an AF_UNIX InetAddress for the given path.
     */
    CARAPI constructor(
        /* [in] */ const String& path);

    /**
     * Constructs an AF_UNIX InetAddress for the given path.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * path);

    /**
     * Returns a string form of this InetAddress.
     */
    CARAPI ToString(
        /* [out] */ String* str);

};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_INETUNIXADDRESS_H__
