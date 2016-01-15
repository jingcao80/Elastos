
#ifndef __ELASTOS_DROID_CONTENT_CINTENT_H__
#define __ELASTOS_DROID_CONTENT_CINTENT_H__

#include "_Elastos_Droid_Content_CIntent.h"
#include "elastos/droid/content/Intent.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntent)
    , public Intent
{
public:
    CAR_OBJECT_DECL()
};

} // Content
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_CONTENT_CINTENT_H__
