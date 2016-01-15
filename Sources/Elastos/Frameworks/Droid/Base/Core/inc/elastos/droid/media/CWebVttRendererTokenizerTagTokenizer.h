#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERTAGTOKENIZER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERTAGTOKENIZER_H__

#include "_Elastos_Droid_Media_CWebVttRendererTokenizerTagTokenizer.h"
#include "elastos/droid/media/Tokenizer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRendererTokenizerTagTokenizer)
    , public Tokenizer::TagTokenizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERERTOKENIZERTAGTOKENIZER_H__