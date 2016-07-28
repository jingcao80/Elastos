#ifndef __ELASTOS_DROID_CONTENT_CPARCELFILEDESCRIPTORINNER_H__
#define __ELASTOS_DROID_CONTENT_CPARCELFILEDESCRIPTORINNER_H__

#include "_Elastos_Droid_Content_CParcelFileDescriptorInner.h"
#include "elastos/droid/content/ContentResolver.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CParcelFileDescriptorInner)
    , public ContentResolver::ParcelFileDescriptorInner
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_CPARCELFILEDESCRIPTORINNER_H__
