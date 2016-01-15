
#ifndef __ELASTOS_DROID_APP_CDATEPICKER_DIALOG_H__
#define __ELASTOS_DROID_APP_CDATEPICKER_DIALOG_H__

#include "_Elastos_Droid_App_CDatePickerDialog.h"
#include "elastos/droid/app/DatePickerDialog.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CDatePickerDialog)
    , public DatePickerDialog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CDATEPICKER_DIALOG_H__
