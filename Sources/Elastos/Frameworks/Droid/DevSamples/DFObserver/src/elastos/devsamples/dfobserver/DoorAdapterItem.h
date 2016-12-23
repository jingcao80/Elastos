
#ifndef __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__
#define __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__

#include "_Elastos.DevSamples.DFObserver.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

class DoorAdapterItem
    : public Object
    , public IDoorAdapterItem
{
public:
    CAR_INTERFACE_DECL()

    DoorAdapterItem();

    DoorAdapterItem(
        /* [in] */ const String& name,
        /* [in] */ Boolean isOpen);

    CARAPI GetIsOpen(
        /* [out] */ Boolean* value);

    CARAPI SetIsOpen(
        /* [in] */ Boolean value);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    String mName;
    Boolean mIsOpen;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__
