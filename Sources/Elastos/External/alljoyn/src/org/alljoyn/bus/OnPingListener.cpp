
#include "org/alljoyn/bus/OnPingListener.h"
#include "org/alljoyn/bus/NativeOnPingListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(OnPingListener, Object, IOnPingListener)

OnPingListener::OnPingListener()
    : mHandle(0)
{}

OnPingListener::~OnPingListener()
{
    Destroy();
}

ECode OnPingListener::constructor()
{
    Create();
    return NOERROR;
}

ECode OnPingListener::OnPing(
    /* [in] */ Int32 status,
    /* [in] */ IInterface* context)
{
    return NOERROR;
}

void OnPingListener::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeOnPingListener((IOnPingListener*)this));
}

void OnPingListener::Destroy()
{
    if (mHandle != 0) {
        NativeOnPingListener* t = reinterpret_cast<NativeOnPingListener*>(mHandle);
        delete t;
        mHandle = 0;
    }
}
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

