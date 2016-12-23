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
    // changed       Property values that changed as an array of dictionary entries, signature "a{sv}".
    // invalidated   Properties whose values have been invalidated, signature "as".
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    NativePropertiesChangedListener* npcl = new NativePropertiesChangedListener(wr, String("a{sv}"), String("as"));
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