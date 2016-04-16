// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class FileDescriptorInfo : public Object
{
public:
    FileDescriptorInfo(
        /* [in] */ Int32 id,
        /* [in] */ Int32 fd,
        /* [in] */ Boolean autoClose);

public:
    Int32 mId;
    Int32 mFd;
    Boolean mAutoClose;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::FileDescriptorInfo, IInterface)

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_

