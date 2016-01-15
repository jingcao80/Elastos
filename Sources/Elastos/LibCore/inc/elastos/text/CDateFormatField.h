
#ifndef __ELASTOS_TEXT_CDATEFORMATFIELD_H__
#define __ELASTOS_TEXT_CDATEFORMATFIELD_H__

#include "_Elastos_Text_CDateFormatField.h"
#include "DateFormat.h"

namespace Elastos {
namespace Text {

CarClass(CDateFormatField)
    , public DateFormat::Field
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATFIELD_H__