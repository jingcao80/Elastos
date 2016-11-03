
#ifndef __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__
#define __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__

#include "_Elastos_Droid_Content_CCursorLoader.h"
#include "elastos/droid/content/CursorLoader.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::ICancellationSignal;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CCursorLoader)
    , public CursorLoader
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__
