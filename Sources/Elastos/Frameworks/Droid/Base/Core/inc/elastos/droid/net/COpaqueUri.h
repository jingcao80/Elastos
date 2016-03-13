
#ifndef __ELASTOS_DROID_NET_COPAQUEURI_H__
#define __ELASTOS_DROID_NET_COPAQUEURI_H__

#include "_Elastos_Droid_Net_COpaqueUri.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Opaque URI.
 */
CarClass(COpaqueUri)
    , public OpaqueUri
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_COPAQUEURI_H__
