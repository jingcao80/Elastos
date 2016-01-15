// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class FileDescriptorInfo : public ElRefBase
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_FILEDESCRIPTORINFO_H_

