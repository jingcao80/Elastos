#ifndef __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

class AwHashSet
{
public:
    static CARAPI_(void*) ElaHashSetCallback_Init();

private:
    static CARAPI_(Boolean) Add(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(void) Clear(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Clone(
            /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) Contains(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(Boolean) IsEmpty(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Iterator(
            /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) Remove(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(Int32) Size(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHS();

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSI(
            /* [in] */ Int32 p0);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSIF(
            /* [in] */ Int32 p0,
            /* [in] */ Float p1);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSJUC(
            /* [in] */ IInterface* extends);
};

} //Api
} //Webkit
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
