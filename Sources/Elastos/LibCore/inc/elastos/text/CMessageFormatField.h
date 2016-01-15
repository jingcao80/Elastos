
#ifndef __ELASTOS_TEXT_CMESSAGEFORMATFIELD_H__
#define __ELASTOS_TEXT_CMESSAGEFORMATFIELD_H__

#include "_Elastos_Text_CMessageFormatField.h"
#include "MessageFormat.h"

namespace Elastos {
namespace Text {

CarClass(CMessageFormatField)
    , public MessageFormat::MessageFormatField
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CMESSAGEFORMATFIELD_H__
