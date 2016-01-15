
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/HandlerRunnable.h"

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(HandlerRunnable, Handler, IRunnable)

HandlerRunnable::HandlerRunnable(
    /* [in] */ Boolean async)
{
    Handler::constructor(async);
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean async)
    : Handler(looper, async)
{
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : Handler(callback, takeStrongRefOfCallback, async)
{
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ ILooper* looper,
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : Handler(looper, callback, takeStrongRefOfCallback, async)
{
}

HandlerRunnable::~HandlerRunnable()
{
}

ECode HandlerRunnable::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
