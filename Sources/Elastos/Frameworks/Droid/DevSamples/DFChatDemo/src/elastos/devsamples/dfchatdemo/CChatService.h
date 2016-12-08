
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCHATSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCHATSERVICE_H__

#include "_Elastos_DevSamples_DFChatDemo_CChatService.h"
#include "CAllJoynService.h"

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

CarClass(CChatService)
    , public CAllJoynService::ChatService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCHATSERVICE_H__
