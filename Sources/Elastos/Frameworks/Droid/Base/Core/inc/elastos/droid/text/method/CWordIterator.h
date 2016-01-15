#ifndef __ELASTOS_DROID_TEXT_METHOD_CWORDITERATOR_H__
#define __ELASTOS_DROID_TEXT_METHOD_CWORDITERATOR_H__

#include "_Elastos_Droid_Text_Method_CWordIterator.h"
#include "elastos/droid/text/method/WordIterator.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CWordIterator)
    , public WordIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CWORDITERATOR_H__
