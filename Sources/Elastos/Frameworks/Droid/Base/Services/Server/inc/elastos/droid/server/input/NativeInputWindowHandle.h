
#ifndef __NATIVEINPUTWINDOWHANDLE_H__
#define __NATIVEINPUTWINDOWHANDLE_H__

#include "elastos/droid/server/input/InputWindowHandle.h"
#include <inputflinger/InputWindow.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class NativeInputWindowHandle
    : public android::InputWindowHandle
{
public:
    NativeInputWindowHandle(
        /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
        /* [in] */ IWeakReference* obj);

    virtual ~NativeInputWindowHandle();

    CARAPI_(AutoPtr<Elastos::Droid::Server::Input::InputWindowHandle>) getInputWindowHandleObj();

    virtual bool updateInfo();

private:
    AutoPtr<IWeakReference> mObjWeak;
};

extern android::sp<NativeInputWindowHandle> GetNativeInputWindowHandle(
    /* [in] */ Elastos::Droid::Server::Input::InputWindowHandle* inputWindowHandleObj);

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__NATIVEINPUTWINDOWHANDLE_H__
