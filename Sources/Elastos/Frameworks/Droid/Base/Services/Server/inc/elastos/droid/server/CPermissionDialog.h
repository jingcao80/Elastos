
#ifndef __ELASTOS_DROID_SERVER_CPERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_CPERMISSIONDIALOG_H__

#include "_Elastos_Droid_Server_CPermissionDialog.h"
#include <elastos/droid/server/PermissionDialog.h>

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CPermissionDialog)
    , public PermissionDialog
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CPERMISSIONDIALOG_H__
