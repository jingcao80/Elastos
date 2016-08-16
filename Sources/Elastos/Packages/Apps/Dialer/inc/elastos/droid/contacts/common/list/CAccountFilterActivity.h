
#ifndef __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
#define __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CarClass(CAccountFilterActivity)
    , public Activity
    , public IAccountFilterActivity
    , public IAdapterViewOnItemClickListener
{

};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
