
#ifndef __ELASTOS_TEXT_CSTRINGCHARACTERIERATOR_H__
#define __ELASTOS_TEXT_CSTRINGCHARACTERIERATOR_H__

#include "_Elastos_Text_CStringCharacterIterator.h"
#include "StringCharacterIterator.h"

namespace Elastos {
namespace Text {

CarClass(CStringCharacterIterator)
    , public StringCharacterIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CSTRINGCHARACTERIERATOR_H__
