
#ifndef __ELASTOS_TEXT_CDATEFORMATSYMBOLS_H__
#define __ELASTOS_TEXT_CDATEFORMATSYMBOLS_H__

#include "_Elastos_Text_CDateFormatSymbols.h"
#include "DateFormatSymbols.h"

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CDateFormatSymbols) , public DateFormatSymbols
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATSYMBOLS_H__
