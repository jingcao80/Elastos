
#include "org/alljoyn/bus/AboutProxy.h"
#include "org/alljoyn/bus/NativeApi.h"

using Org::Alljoyn::Bus::Ifaces::EIID_IAbout;

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(AboutProxy, Object, IAboutProxy)

ECode AboutProxy::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ const String& busName,
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IClassLoader> loader = GetModuleClassLoader();
    AutoPtr<IInterfaceInfo> itfcInfo;
    loader->LoadInterface(String("Org.Alljoyn.Bus.Ifaces.IAbout"), (IInterfaceInfo**)&itfcInfo);
    AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
    busInterfaces->Set(0, itfcInfo);

    AutoPtr<IProxyBusObject> aboutProxy;
    bus->GetProxyBusObject(busName, IAbout::OBJ_PATH, sessionId,
        busInterfaces, (IProxyBusObject**)&aboutProxy);
    return aboutProxy->GetInterface(EIID_IAbout, (IInterface**)&mProxy);
}

ECode AboutProxy::GetObjectDescription(
    /* [out, callee] */ ArrayOf<IAboutObjectDescription*>** array)
{
    return mProxy->GetObjectDescription(array);
}

ECode AboutProxy::GetAboutData(
    /* [in] */ const String& languageTag,
    /* [out] */ IMap** aboutData)
{
    return mProxy->GetAboutData(languageTag, aboutData);
}

ECode AboutProxy::GetVersion(
    /* [out] */ Int16* ver)
{
    return mProxy->GetVersion(ver);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
