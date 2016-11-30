
#include "org/alljoyn/bus/ProxyBusObject.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(ProxyBusObject, Object, IProxyBusObject)

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces)
{
    return NOERROR;
}

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ Boolean secure)
{
    return NOERROR;
}

ECode ProxyBusObject::EnablePropertyCaching()
{
    return NOERROR;
}

ECode ProxyBusObject::ReleaseResources()
{
    return NOERROR;
}

ECode ProxyBusObject::GetBusName(
    /* [out] */ String* busName)
{
    return NOERROR;
}

ECode ProxyBusObject::GetObjPath(
    /* [out] */ String* objPath)
{
    return NOERROR;
}

ECode ProxyBusObject::GetInterface(
    /* [in] */ IInterfaceInfo* intfId,
    /* [out] */ IInterface** intf)
{
    return NOERROR;
}

ECode ProxyBusObject::SetReplyTimeout(
    /* [in] */ Int32 timeoutMsecs)
{
    return NOERROR;
}

ECode ProxyBusObject::SetAutoStart(
    /* [in] */ Boolean autoStart)
{
    return NOERROR;
}

ECode ProxyBusObject::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
