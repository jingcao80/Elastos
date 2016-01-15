#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZER_H__

#include "_Elastos_Droid_Media_CWebVttRendererTokenizer.h"
#include "elastos/droid/media/Tokenizer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRendererTokenizer)
    , public Tokenizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZER_H__