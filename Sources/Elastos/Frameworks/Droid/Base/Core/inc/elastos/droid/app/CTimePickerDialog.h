
#ifndef __ELASTOS_DROID_APP_CTIMEPICKER_DIALOG_H__
#define __ELASTOS_DROID_APP_CTIMEPICKER_DIALOG_H__

#include "_Elastos_Droid_App_CTimePickerDialog.h"
#include "elastos/droid/app/TimePickerDialog.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CTimePickerDialog)
    , public TimePickerDialog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CTIMEPICKER_DIALOG_H__
