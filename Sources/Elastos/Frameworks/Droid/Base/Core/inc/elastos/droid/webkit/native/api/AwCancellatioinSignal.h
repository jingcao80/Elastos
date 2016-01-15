#ifndef __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

class AwCancellationSignal
{
public:
    static CARAPI_(void*) ElaCancellationSignalCallback_Init();

private:
    static CARAPI_(Boolean) IsCanceled(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ThrowIfCanceled(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) Cancel(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SetOnCancelListener(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0);

    static CARAPI_(AutoPtr<IInterface>) Constructor();
};

} //Api
} //Webkit
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
