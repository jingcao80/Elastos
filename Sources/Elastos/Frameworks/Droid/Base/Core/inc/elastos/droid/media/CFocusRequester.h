
#ifndef __ELASTOS_DROID_MEDIA_CFOCUSREQUESTER_H__
#define __ELASTOS_DROID_MEDIA_CFOCUSREQUESTER_H__

#include "_Elastos_Droid_Media_CFocusRequester.h"
#include "elastos/droid/media/FocusRequester.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CFocusRequester)
    , public FocusRequester
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CFOCUSREQUESTER_H__
