
#ifndef __ORG_ALLJOYN_BUS_ABOUTPROXY_H__
#define __ORG_ALLJOYN_BUS_ABOUTPROXY_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Org::Alljoyn::Bus::Ifaces::IAbout;

namespace Org {
namespace Alljoyn {
namespace Bus {

class AboutProxy
    : public Object
    , public IAboutProxy
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
     * Get a list of object paths and the interfaces that are being announced
     *
     * @return
     * An array of AboutObjectDescriptions. Each AboutObjectDescription contains
     * the path of a remote BusObject and a list of interfaces contained at that
     * path.
     * @throws BusException
     *   throws an exception indicating failure to make the remote method call
     */
    CARAPI GetObjectDescription(
        /* [out, callee] */ ArrayOf<IAboutObjectDescription*>** array);

    /**
     * Get all of the AboutData from the remote BusAttachment. Not all AboutData
     * is contained in the Announced AboutData. This method can be used to get
     * all of the AboutData for a specified language.
     *
     * @param languageTag a String formated as an IETF language tag specified by RFC 5646
     * @return
     * A map containing the fields specified in the About Interface
     *
     * @throws BusException
     *   throws an exception indicating failure to make the remote method call
     */
    CARAPI GetAboutData(
        /* [in] */ const String& languageTag,
        /* [out] */ IMap** aboutData);

    /**
     * Get the version of the remote About interface
     * @return the version of the remote About interface
     * @throws BusException
     *   throws an exception indicating failure to make the remote method call
     */
    CARAPI GetVersion(
        /* [out] */ Int16* ver);

private:
    /**
     * An org.alljoyn.Icon ProxyBusObject
     */
    AutoPtr<IAbout> mProxy;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ABOUTPROXY_H__
