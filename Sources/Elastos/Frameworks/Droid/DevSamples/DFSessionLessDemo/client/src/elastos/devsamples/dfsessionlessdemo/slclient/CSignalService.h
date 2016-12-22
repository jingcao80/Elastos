
#ifndef __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__
#define __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__

#include "CClient.h"
#include "_Elastos_DevSamples_DFSessionLessDemo_SLClient_CSignalService.h"

namespace Elastos {
namespace DevSamples {
namespace DFSessionLessDemo {
namespace SLClient {

CarClass(CSignalService)
    , public CClient::SignalService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace SLClient
} // namespace DFSessionLessDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__
