
#ifndef __ELASTOS_DROID_NET_CURIBUILDER_H__
#define __ELASTOS_DROID_NET_CURIBUILDER_H__

#include "_Elastos_Droid_Net_CUriBuilder.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Helper class for building or manipulating URI references. Not safe for
 * concurrent use.
 *
 * <p>An absolute hierarchical URI reference follows the pattern:
 * {@code <scheme>://<authority><absolute path>?<query>#<fragment>}
 *
 * <p>Relative URI references (which are always hierarchical) follow one
 * of two patterns: {@code <relative or absolute path>?<query>#<fragment>}
 * or {@code //<authority><absolute path>?<query>#<fragment>}
 *
 * <p>An opaque URI follows this pattern:
 * {@code <scheme>:<opaque part>#<fragment>}
 *
 * <p>Use {@link Uri#buildUpon()} to obtain a builder representing an existing URI.
 */
CarClass(CUriBuilder)
    , public UriBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CURIBUILDER_H__
