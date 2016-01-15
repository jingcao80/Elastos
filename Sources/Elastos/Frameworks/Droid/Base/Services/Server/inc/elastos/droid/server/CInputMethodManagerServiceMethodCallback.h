
#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__

#include <_CInputMethodManagerServiceMethodCallback.h>
#include <CInputMethodManagerService.h>

using Elastos::AutoPtr;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputMethodSession;


namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CInputMethodManagerServiceMethodCallback)
{
public:
    CARAPI constructor(
        /* [in] */ IIInputMethod* method,
        /* [in] */ Handle32 host);

    CARAPI FinishedEvent(
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled);

    CARAPI SessionCreated(
        /* [in] */ IIInputMethodSession* session);

    CARAPI GetDescription(
            /* [out] */ String* description);

private:
    AutoPtr<IIInputMethod> mMethod;
    CInputMethodManagerService* mHost;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEMETHODCALLBACK_H__
