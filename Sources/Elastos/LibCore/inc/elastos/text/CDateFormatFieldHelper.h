
#ifndef __ELASTOS_TEXT_CDATEFORMATFIELDHELPER_H__
#define __ELASTOS_TEXT_CDATEFORMATFIELDHELPER_H__

#include "_Elastos_Text_CDateFormatFieldHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Text {

CarClass(CDateFormatFieldHelper)
    , public Singleton
    , public IDateFormatFieldHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetDateFormatFieldByName(
        /* [in] */ const String& name,
        /* [out] */ IDateFormatField** field);

    CARAPI OfCalendarField(
        /* [in] */ Int32 calendarField,
        /* [out] */ IDateFormatField** field);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATFIELDHELPER_H__
