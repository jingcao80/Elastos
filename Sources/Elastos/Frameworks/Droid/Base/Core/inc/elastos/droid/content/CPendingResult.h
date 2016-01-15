
#ifndef __ELASTOS_DROID_CONTENT_CBROADCASTRECEIVERPENDINGRESULT_H__
#define __ELASTOS_DROID_CONTENT_CBROADCASTRECEIVERPENDINGRESULT_H__

#include "_Elastos_Droid_Content_CPendingResult.h"
#include "elastos/droid/content/BroadcastReceiver.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CPendingResult)
    , public BroadcastReceiver::PendingResult
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CBROADCASTRECEIVERPENDINGRESULT_H__
