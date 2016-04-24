#ifndef __ELASTOS_DROID_OS_HANDLERRUNNABLE_H__
#define __ELASTOS_DROID_OS_HANDLERRUNNABLE_H__

#include <elastos/droid/os/Handler.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Base class for HandlerRunnable
 */
class ECO_PUBLIC HandlerRunnable
    : public Handler
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("HandlerRunnable")

    HandlerRunnable(
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ ILooper* looper,
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ IHandlerCallback* callback,
        /* [in] */ Boolean takeStrongRefOfCallback,
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ ILooper* looper,
        /* [in] */ IHandlerCallback* callback,
        /* [in] */ Boolean takeStrongRefOfCallback,
        /* [in] */ Boolean async = FALSE);

    virtual ~HandlerRunnable();

    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);
};


} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_HANDLERRUNNABLE_H__
