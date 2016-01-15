
#ifndef __ELASTOS_DROID_CONTENT_CLOADER_H__
#define __ELASTOS_DROID_CONTENT_CLOADER_H__

#include "_Elastos_Droid_Content_CLoader.h"
#include "elastos/droid/content/Loader.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CLoader)
    , public Loader
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CLOADER_H__
