
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBICONDATABASEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBICONDATABASEADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Webkit::IWebIconDatabase;
using Elastos::Droid::Webkit::IWebIconDatabaseIconListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of WebIconDatabase -- big old no-op (base class is deprecated).
  */
class WebIconDatabaseAdapter
    : public Object
    , public IWebIconDatabase
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI Open(
        /* [in] */ const String& path);

    // @Override
    CARAPI Close();

    // @Override
    CARAPI RemoveAllIcons();

    // @Override
    CARAPI RequestIconForPageUrl(
        /* [in] */ const String& url,
        /* [in] */ IWebIconDatabaseIconListener* listener);

    // @Override
    CARAPI BulkRequestIconForPageUrl(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& where,
        /* [in] */ IWebIconDatabaseIconListener* listener);

    // @Override
    CARAPI RetainIconForPageUrl(
        /* [in] */ const String& url);

    // @Override
    CARAPI ReleaseIconForPageUrl(
        /* [in] */ const String& url);
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBICONDATABASEADAPTER_H_

