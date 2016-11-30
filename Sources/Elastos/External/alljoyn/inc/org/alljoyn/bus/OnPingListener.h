
#ifndef __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__
#define __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class OnPingListener
    : public Object
    , public IOnPingListener
{
public:
    CAR_INTERFACE_DECL()

    OnPingListener();

    virtual ~OnPingListener();

    CARAPI constructor();

    CARAPI OnPing(
        /* [in] */ Int32 status,
        /* [in] */ IInterface* context);

private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this OnPingListener object.
     */
    void Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this OnPingListener object.
     */
    void Destroy();

private:
    friend class CBusAttachment;

    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__
