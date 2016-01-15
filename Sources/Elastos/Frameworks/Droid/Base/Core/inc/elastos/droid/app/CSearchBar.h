
#ifndef __ELASTOS_DROID_APP_CSERACHBAR_H__
#define __ELASTOS_DROID_APP_CSERACHBAR_H__

#include "_Elastos_Droid_App_CSearchBar.h"
#include "elastos/droid/app/SearchDialog.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CSearchBar)
    , public SearchDialog::SearchBar
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CSERACHBAR_H__