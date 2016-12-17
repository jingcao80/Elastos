
#ifndef __ORG_ALLJOYN_BUS_ABOUTICONPROXY_H__
#define __ORG_ALLJOYN_BUS_ABOUTICONPROXY_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::Alljoyn::Bus::Ifaces::IIcon;

namespace Org {
namespace Alljoyn {
namespace Bus {

class AboutIconProxy
    : public Object
    , public IAboutIconProxy
    , public IIcon
{
public:
    CAR_INTERFACE_DECL();

    /**
     * AboutProxy constructor
     *
     * @param bus the BusAttachment that owns this ProxyBusObject
     * @param busName the unique or well-known name of the remote AllJoyn BusAttachment
     * @param sessionId the session ID this ProxyBusObject will use
     */
    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ const String& busName,
        /* [in] */ Int32 sessionId);

    /**
     * fill in an AboutIcon by making remote method calls to obtain the
     * Information from a remote AboutIcon BusObject
     *
     * @return An AboutIcon
     *
     * @throws BusException indicating there was a failure trying to obtain the
     * data needed to create an AboutIcon
     */
    CARAPI GetAboutIcon(
        /* [out] */ IAboutIcon** aboutIcon);

    CARAPI GetVersion(
        /* [out] */ Int16* ver);

    CARAPI GetMimeType(
        /* [out] */ String* mimeType);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetUrl(
        /* [out] */ String* url);

    CARAPI GetContent(
        /* [out, callee] */ ArrayOf<Byte>** content);

private:
    /**
     * An org.alljoyn.Icon ProxyBusObject
     */
    AutoPtr<IIcon> mProxy;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ABOUTICONPROXY_H__
