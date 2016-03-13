
#include "elastos/droid/webkit/webview/chromium/native/content_public/Referrer.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content_Public {

Referrer::Referrer(
    /* [in] */ const String& url,
    /* [in] */ Int32 policy)
    : mUrl(url)
    , mPolicy(policy)
{
}

String Referrer::GetUrl()
{
    return mUrl;
}

Int32 Referrer::GetPolicy()
{
    return mPolicy;
}

} // namespace Content_Public
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
