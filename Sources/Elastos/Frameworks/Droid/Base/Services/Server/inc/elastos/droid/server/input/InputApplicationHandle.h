#ifndef __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__
#define __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class InputApplicationHandle
    : public Object
    , public IInputApplicationHandle
{
public:
    CAR_INTERFACE_DECL()

    InputApplicationHandle(
        /* [in] */ IWeakReference* appWindowToken);

    virtual ~InputApplicationHandle();

private:
    CARAPI_(void) NativeDispose();

public:
    // The window manager's application window token.
    AutoPtr<IWeakReference> mWeakAppWindowToken;

    // Application name.
    String mName;

    // Dispatching timeout.
    Int64 mDispatchingTimeoutNanos;

public:
    // Pointer to the native input application handle.
    // This field is lazily initialized via JNI.
    Int64 mPtr;
};

} // Input
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__
