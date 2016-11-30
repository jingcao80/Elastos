
#include "org/alljoyn/bus/OnJoinSessionListener.h"
#include "org/alljoyn/bus/NativeOnJoinSessionListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(OnJoinSessionListener, Object, IOnJoinSessionListener)

OnJoinSessionListener::OnJoinSessionListener()
    : mHandle(0)
{}

OnJoinSessionListener::~OnJoinSessionListener()
{
    Destroy();
}

ECode OnJoinSessionListener::constructor()
{
    Create();
    return NOERROR;
}

void OnJoinSessionListener::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeOnJoinSessionListener((IOnJoinSessionListener*)this));
}

void OnJoinSessionListener::Destroy()
{
    if (mHandle != 0) {
        NativeOnJoinSessionListener* t = reinterpret_cast<NativeOnJoinSessionListener*>(mHandle);
        delete t;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org