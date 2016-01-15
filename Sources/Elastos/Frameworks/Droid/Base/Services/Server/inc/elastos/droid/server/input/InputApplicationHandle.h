
#ifndef __ELASTOS_DROID_SERVER_INPUT_INPUTAPPLICATIONHANDLE_H__
#define __ELASTOS_DROID_SERVER_INPUT_INPUTAPPLICATIONHANDLE_H__

#include <elastos.h>
#include "NativeInputApplicationHandle.h"
#include <input/InputApplication.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

/**
 * Functions as a handle for an application that can receive input.
 * Enables the native input dispatcher to refer indirectly to the window manager's
 * application window token.
 * @hide
 */
class InputApplicationHandle : public ElRefBase
{
public:
    InputApplicationHandle(
        /* [in] */ void* appWindowToken);

    ~InputApplicationHandle();

    static CARAPI_(android::sp<android::InputApplicationHandle>) GetHandle(
        /* [in] */ InputApplicationHandle* inputApplicationHandleObj);

public:
    // Application name.
    String mName;

    // Dispatching timeout.
    Int64 mDispatchingTimeoutNanos;

    // The window manager's application window token.
    // CWindowManagerService::AppWindowToken* mAppWindowToken
    void* mAppWindowToken;

private:
    // Pointer to the native input application handle.
    // This field is lazily initialized via JNI.
    NativeInputApplicationHandle* mNative;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_INPUTAPPLICATIONHANDLE_H__
