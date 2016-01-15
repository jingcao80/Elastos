
#ifndef __ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENTFILECHOOSERPARAMSHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENTFILECHOOSERPARAMSHELPER_H__

#include "_Elastos_Droid_Webkit_CWebChromeClientFileChooserParamsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebChromeClientFileChooserParamsHelper)
    , public Singleton
    , public IWebChromeClientFileChooserParamsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ParseResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data,
        /* [out, callee] */ ArrayOf<IUri*>** uriArrayOut);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENTFILECHOOSERPARAMSHELPER_H__
