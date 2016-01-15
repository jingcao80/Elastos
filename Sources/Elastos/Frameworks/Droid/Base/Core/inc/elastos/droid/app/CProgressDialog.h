#ifndef __ELASTOS_DROID_APP_CPROGRESSDIALOG_H__
#define __ELASTOS_DROID_APP_CPROGRESSDIALOG_H__

#include "_Elastos_Droid_App_CProgressDialog.h"
#include "elastos/droid/app/ProgressDialog.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProgressDialog)
    , public ProgressDialog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROGRESSDIALOG_H__
