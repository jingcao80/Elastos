
#ifndef __NATIVEINPUTAPPLICATIONHANDLE_H__
#define __NATIVEINPUTAPPLICATIONHANDLE_H__

#include "elastos/droid/server/input/InputApplicationHandle.h"
#include <inputflinger/InputApplication.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class NativeInputApplicationHandle
    : public android::InputApplicationHandle
{
public:
    NativeInputApplicationHandle(
        /* [in] */ IWeakReference* obj);

    virtual ~NativeInputApplicationHandle();

    CARAPI_(AutoPtr<Elastos::Droid::Server::Input::InputApplicationHandle>) getInputApplicationHandleObj();

    virtual bool updateInfo();

private:
    AutoPtr<IWeakReference> mObjWeak;
};

extern android::sp<android::InputApplicationHandle> GetNativeInputApplicationHandle(
    /* [in] */ Elastos::Droid::Server::Input::InputApplicationHandle* inputApplicationHandleObj);

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__NATIVEINPUTAPPLICATIONHANDLE_H__
