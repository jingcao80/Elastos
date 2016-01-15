
#ifndef __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTWINDOWHANDLE_H__
#define __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTWINDOWHANDLE_H__

#include <input/InputWindow.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class InputWindowHandle;

class NativeInputWindowHandle : public android::InputWindowHandle
{
public:
    NativeInputWindowHandle(
        /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
        /* [in] */ Input::InputWindowHandle* obj);

    virtual ~NativeInputWindowHandle();

    Input::InputWindowHandle* getInputWindowHandleObj();

    virtual bool updateInfo();

private:
    //jweak mObjWeak;
    Input::InputWindowHandle* mObject;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTWINDOWHANDLE_H__
