#ifndef __ELASTOS_DROID_MEDIA_CTOKENIZERDATATOKENIZER_H__
#define __ELASTOS_DROID_MEDIA_CTOKENIZERDATATOKENIZER_H__

#include "elastos/droid/media/Tokenizer.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CTokenizerDataTokenizer)
    , public Tokenizer::DataTokenizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTOKENIZERDATATOKENIZER_H__