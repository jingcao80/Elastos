
#ifndef __ELASTOS_DROID_CONTENT_CRESTRICTIONENTRY_H__
#define __ELASTOS_DROID_CONTENT_CRESTRICTIONENTRY_H__

#include "_Elastos_Droid_Content_CRestrictionEntry.h"
#include "elastos/droid/content/RestrictionEntry.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CRestrictionEntry)
    , public RestrictionEntry
{
public:
    CAR_OBJECT_DECL()
};

} // Content
} // Droid
} // Elastos

#endif//__ELASTOS_DROID_CONTENT_CRESTRICTIONENTRY_H__