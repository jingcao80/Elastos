
#ifndef __ELASTOS_TEXT_CMESSAGEFORMAT_H__
#define __ELASTOS_TEXT_CMESSAGEFORMAT_H__

#include "_Elastos_Text_CMessageFormat.h"
#include "MessageFormat.h"

using Elastos::Utility::ILocale;
using Elastos::Text::IAttributedCharacterIterator;;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;

namespace Elastos {
namespace Text {

CarClass(CMessageFormat) , public MessageFormat
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CMESSAGEFORMAT_H__
