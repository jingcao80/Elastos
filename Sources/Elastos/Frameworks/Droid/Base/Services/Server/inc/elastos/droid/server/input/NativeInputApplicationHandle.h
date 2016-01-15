
#ifndef __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTAPPLICATIONHANDLE_H__
#define __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTAPPLICATIONHANDLE_H__

#include <input/InputApplication.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class InputApplicationHandle;

class NativeInputApplicationHandle : public android::InputApplicationHandle
{
public:
    NativeInputApplicationHandle(
        /* [in] */ Input::InputApplicationHandle* obj);

    virtual ~NativeInputApplicationHandle();

    Input::InputApplicationHandle* getInputApplicationHandleObj();

    virtual bool updateInfo();

private:
//    jweak mObjWeak;
    Input::InputApplicationHandle* mObject;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTAPPLICATIONHANDLE_H__
