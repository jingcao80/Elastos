
#ifndef __ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__

#include "_Elastos_Droid_Server_CBasePermissionDialog.h"
#include <elastos/droid/server/BasePermissionDialog.h>

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBasePermissionDialog)
    , public BasePermissionDialog
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__
