#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTPARSER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTPARSER_H__

#include "_Elastos_Droid_Media_CWebVttParser.h"
#include "elastos/droid/media/WebVttParser.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttParser)
    , public WebVttParser
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTPARSER_H__