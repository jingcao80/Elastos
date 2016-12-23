
#ifndef __Elastos_DevSamples_DFObserver_CDoorEventListener_H__
#define __Elastos_DevSamples_DFObserver_CDoorEventListener_H__

#include "_Elastos_DevSamples_DFObserver_CDoorEventListener.h"
#include "BusHandler.h"

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

CarClass(CDoorEventListener)
    , public BusHandler::DoorEventListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_CDoorEventListener_H__
