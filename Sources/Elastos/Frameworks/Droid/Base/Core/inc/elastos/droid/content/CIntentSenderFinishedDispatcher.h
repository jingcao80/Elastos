
#ifndef __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__

#include "_Elastos_Droid_Content_CIntentSenderFinishedDispatcher.h"
#include "elastos/droid/content/CIntentSender.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentSenderFinishedDispatcher)
    , public CIntentSender::FinishedDispatcher
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__
