
#ifndef __ELASTOS_DROID_TEXT_CANNOTATION_H__
#define __ELASTOS_DROID_TEXT_CANNOTATION_H__

#include "_Elastos_Droid_Text_CAnnotation.h"
#include "elastos/droid/text/Annotation.h"

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CAnnotation)
    , public Annotation
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CANNOTATION_H__
