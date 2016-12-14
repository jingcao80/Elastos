#include "org/alljoyn/bus/PropertiesChangedListener.h"
#include "org/alljoyn/bus/NativePropertiesChangedListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(PropertiesChangedListener, Object, IPropertiesChangedListener)

PropertiesChangedListener::PropertiesChangedListener()
    : mHandle(0)
{}

PropertiesChangedListener::~PropertiesChangedListener()
{
    Destroy();
}

ECode PropertiesChangedListener::constructor()
{
    Create();
    return NOERROR;
}

void PropertiesChangedListener::Create()
{
    // try {
    //     Method m = getClass().getMethod("propertiesChanged", new Class<?>[] {ProxyBusObject.class, String.class, Map.class, String[].class});
    //     Type p[] = m.getGenericParameterTypes();
    //     Type changedType = p[2];
    //     Type invalidatedType = p[3];
    //     create(changedType, invalidatedType);
    // } catch (NoSuchMethodException ex) {
    //     System.err.println("failed to get propertiesChanged method");  // Should never happen.
    // }
    assert(0 && "TODO");
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    NativePropertiesChangedListener* npcl = new NativePropertiesChangedListener(wr, NULL, NULL);
    mHandle = reinterpret_cast<Int64>(npcl);
}

/** Release native resources. */
void PropertiesChangedListener::Destroy()
{
    if (mHandle != 0) {
        NativePropertiesChangedListener* npcl = reinterpret_cast<NativePropertiesChangedListener*>(mHandle);
        delete npcl;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org