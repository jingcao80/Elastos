
#ifndef __ELASTOS_DROID_NET_CSTRINGURI_H__
#define __ELASTOS_DROID_NET_CSTRINGURI_H__

#include "_Elastos_Droid_Net_CStringUri.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * An implementation which wraps a String URI. This URI can be opaque or
 * hierarchical, but we extend AbstractHierarchicalUri in case we need
 * the hierarchical functionality.
 */
CarClass(CStringUri)
    , public StringUri
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CSTRINGURI_H__
