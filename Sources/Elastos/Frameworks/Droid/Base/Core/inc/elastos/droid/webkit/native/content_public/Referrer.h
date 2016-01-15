
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_REFERRER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_REFERRER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content_Public {

/**
 * Container that holds together a referrer URL along with the referrer policy set on the
 * originating frame. This corresponds to native content/public/common/referrer.h.
 */
class Referrer : public Object
{
public:
    Referrer(
        /* [in] */ const String& url,
        /* [in] */ Int32 policy);

    CARAPI_(String) GetUrl();

    CARAPI_(Int32) GetPolicy();

private:
    const String mUrl;
    const Int32 mPolicy;
};

} // namespace Content_Public
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_REFERRER_H__
