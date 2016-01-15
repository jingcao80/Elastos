
#ifndef __ELASTOS_TEXT_CSIMPLEDATEFORMAT_H__
#define __ELASTOS_TEXT_CSIMPLEDATEFORMAT_H__

#include "_Elastos_Text_CSimpleDateFormat.h"
#include "SimpleDateFormat.h"

using Elastos::Utility::ILocale;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;
using Elastos::Utility::ICurrency;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Text {

CarClass(CSimpleDateFormat) , public SimpleDateFormat
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CSIMPLEDATEFORMAT_H__
