#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERDATATOKENIZER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERDATATOKENIZER_H__

#include "_Elastos_Droid_Media_CWebVttRendererTokenizerDataTokenizer.h"
#include "elastos/droid/media/Tokenizer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRendererTokenizerDataTokenizer)
    , public Tokenizer::DataTokenizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERDATATOKENIZER_H__