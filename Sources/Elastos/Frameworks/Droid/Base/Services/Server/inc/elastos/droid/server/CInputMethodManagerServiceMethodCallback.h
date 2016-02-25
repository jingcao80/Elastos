
#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__

#include <_Elastos_Droid_Server_CInputMethodManagerServiceMethodCallback.h>
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::View::IIInputSessionCallback;
using Elastos::Droid::Internal::View::IIInputMethodManager;

namespace Elastos {
namespace Droid {
namespace Server {

class CInputMethodManagerService;

CarClass(CInputMethodManagerServiceMethodCallback)
    , public Object
    , public IIInputSessionCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIInputMethodManager* imms,
        /* [in] */ IIInputMethod* method,
        /* [in] */ IInputChannel* channel);

    CARAPI SessionCreated(
        /* [in] */ IIInputMethodSession* session);

    CARAPI ToString(
            /* [out] */ String* description);

private:
    CInputMethodManagerService* mParentIMMS;
    AutoPtr<IIInputMethod> mMethod;
    AutoPtr<IInputChannel> mChannel;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__
