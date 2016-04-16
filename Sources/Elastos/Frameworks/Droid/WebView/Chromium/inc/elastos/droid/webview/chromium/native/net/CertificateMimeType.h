
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTIFICATEMIMETYPE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTIFICATEMIMETYPE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

class CertificateMimeType
    : public Object
{
public:
    static const Int32 UNKNOWN = 0;
    static const Int32 X509_USER_CERT = 1;
    static const Int32 X509_CA_CERT = 2;
    static const Int32 PKCS12_ARCHIVE = 3;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTIFICATEMIMETYPE_H_

