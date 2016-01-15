#ifndef __ELASTOS_DROID_MEDIA_CUNSTYLEDTEXTEXTRACTOR_H__
#define __ELASTOS_DROID_MEDIA_CUNSTYLEDTEXTEXTRACTOR_H__

#include "_Elastos_Droid_Media_CUnstyledTextExtractor.h"
#include "elastos/droid/media/UnstyledTextExtractor.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CUnstyledTextExtractor)
    , public UnstyledTextExtractor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CUNSTYLEDTEXTEXTRACTOR_H__