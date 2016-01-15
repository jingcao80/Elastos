#ifndef __ELASTOS_DROID_SERVER_CLIPBOARD_CCLIPBOARDSERVICE_H__
#define __ELASTOS_DROID_SERVER_CLIPBOARD_CCLIPBOARDSERVICE_H__

#include "_Elastos_Droid_Server_Clipboard_CClipboardService.h"
#include "elastos/droid/server/clipboard/ClipboardService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Clipboard {

CarClass(CClipboardService)
    , public ClipboardService
{
public:
    CAR_OBJECT_DECL();
};

}//namespace Clipboard
}//namespace Server
}//namesapce Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CLIPBOARD_CCLIPBOARDSERVICE_H__
