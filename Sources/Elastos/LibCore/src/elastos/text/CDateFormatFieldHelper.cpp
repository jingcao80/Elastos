
#include "CDateFormatFieldHelper.h"
#include "DateFormat.h"

namespace Elastos {
namespace Text {

CAR_SINGLETON_IMPL(CDateFormatFieldHelper)

CAR_INTERFACE_IMPL(CDateFormatFieldHelper, Singleton, IDateFormatFieldHelper)

ECode CDateFormatFieldHelper::GetDateFormatFieldByName(
    /* [in] */ const String& name,
    /* [out] */ IDateFormatField** field)
{
    return NOERROR;
}

ECode CDateFormatFieldHelper::OfCalendarField(
    /* [in] */ Int32 calendarField,
    /* [out] */ IDateFormatField** field)
{
    return DateFormat::Field::OfCalendarField(calendarField,field);
}


} // namespace Text
} // namespace Elastos