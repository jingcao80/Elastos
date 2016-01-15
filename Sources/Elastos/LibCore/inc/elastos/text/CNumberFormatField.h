
#ifndef __ELASTOS_TEXT_CNUMBERFORMATFIELD_H__
#define __ELASTOS_TEXT_CNUMBERFORMATFIELD_H__

#include "_Elastos_Text_CNumberFormatField.h"
#include "NumberFormat.h"

namespace Elastos {
namespace Text {

CarClass(CNumberFormatField)
    , public NumberFormat::Field
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CNUMBERFORMATFIELD_H__
