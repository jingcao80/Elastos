
#ifndef __ELASTOS_TEXT_CATTRIBUTEDSTRING_H__
#define __ELASTOS_TEXT_CATTRIBUTEDSTRING_H__

#include "_Elastos_Text_CAttributedString.h"
#include "AttributedString.h"

using Elastos::Utility::IMap;

namespace Elastos {
namespace Text {

CarClass(CAttributedString) , public AttributedString
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CATTRIBUTEDSTRING_H__
