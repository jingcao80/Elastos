
#ifndef __ELASTOS_TEXT_CDECIMALFORMAT_H__
#define __ELASTOS_TEXT_CDECIMALFORMAT_H__

#include "_Elastos_Text_CDecimalFormat.h"
#include "DecimalFormat.h"

using Elastos::Utility::ILocale;
using Elastos::Text::IFieldPosition;
using Elastos::Utility::ICurrency;
using Elastos::Text::IDecimalFormatSymbols;
using Elastos::Text::IParsePosition;

namespace Elastos {
namespace Text {

CarClass(CDecimalFormat) , public DecimalFormat
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDECIMALFORMAT_H__
