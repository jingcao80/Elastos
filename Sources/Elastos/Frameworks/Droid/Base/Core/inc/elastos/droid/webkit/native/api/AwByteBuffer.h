#ifndef __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

class AwByteBuffer
{
public:
    static CARAPI_(void*) ElaByteBufferCallback_Init();

private:
    static CARAPI_(void*) GetDirectBufferAddress(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int64) GetDirectBufferCapacity(
        /* [in] */ IInterface* obj);
};

} //Api
} //Webkit
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
