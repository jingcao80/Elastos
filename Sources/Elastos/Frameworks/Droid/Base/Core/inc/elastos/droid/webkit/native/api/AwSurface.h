#ifndef __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

class AwSurface
{
public:
    static CARAPI_(void*) ElaSurfaceCallback_Init();

private:
    static CARAPI_(void) Finalize(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ReleaseResources(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) IsValid(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) LockCanvas(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0);

    static CARAPI_(void) UnlockCanvasAndPost(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0);

    static CARAPI_(void) UnlockCanvas(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0);

    static CARAPI_(Int32) DescribeContents(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ReadFromParcel(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0);

    static CARAPI_(void) WriteToParcel(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* p0,
        /* [in] */ Int32 p1);

    static CARAPI_(String) ToString(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Constructor(
        /* [in] */ IInterface* p0);
};

} //Api
} //Webkit
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
