
#ifndef __ELASTOS_DROID_NET_CHIERARCHICALURI_H__
#define __ELASTOS_DROID_NET_CHIERARCHICALURI_H__

#include "_Elastos_Droid_Net_CHierarchicalUri.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Hierarchical Uri.
 */
CarClass(CHierarchicalUri)
    , public HierarchicalUri
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CHIERARCHICALURI_H__
