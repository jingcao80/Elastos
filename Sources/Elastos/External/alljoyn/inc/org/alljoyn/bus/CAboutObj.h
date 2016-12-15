
#ifndef __ORG_ALLJOYN_BUS_CABOUTOBJ_H__
#define __ORG_ALLJOYN_BUS_CABOUTOBJ_H__

#include "_Org_Alljoyn_Bus_CAboutObj.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutObj)
    , public Object
    , public IAboutObj
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAboutObj();

    ~CAboutObj();

    CARAPI constructor(
        /* [in] */ IBusAttachment* bus);

    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ Boolean isAboutObjectAnnounced);

    CARAPI Announce(
        /* [in] */ Int16 sessionPort,
        /* [in] */ IAboutDataListener* aboutData);

    /**
     * Cancel the last announce signal sent. If no signals have been sent this
     * method call will return.
     *
     * @return Status.OK on success
     */
    CARAPI Unannounce();

private:
    void Destory();

private:
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTOBJ_H__
