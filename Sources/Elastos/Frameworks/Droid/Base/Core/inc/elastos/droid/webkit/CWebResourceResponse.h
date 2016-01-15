
#ifndef __ELASTOS_DROID_WEBKIT_CWEBRESOURCERESPONSE_H__
#define __ELASTOS_DROID_WEBKIT_CWEBRESOURCERESPONSE_H__

#include "_Elastos_Droid_Webkit_CWebResourceResponse.h"
#include "elastos/droid/webkit/WebResourceResponse.h"

using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Webkit {

struct ElaWebResourceResponseCallback
{
    String (*getMimeType)(IInterface* obj);
    String (*getEncoding)(IInterface* obj);
    AutoPtr<IInterface> (*getData)(IInterface* obj);
};

CarClass(CWebResourceResponse), public WebResourceResponse
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBRESOURCERESPONSE_H__
