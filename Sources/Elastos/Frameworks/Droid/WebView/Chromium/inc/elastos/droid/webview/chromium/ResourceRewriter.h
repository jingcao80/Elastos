//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

/*
  * Copyright (C) 2014 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_RESOURCEREWRITER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_RESOURCEREWRITER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package com.android.webview.chromium;
// import android.content.Context;
// import android.util.SparseArray;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Helper class used to fix up resource ids.
  * This is mostly a copy of the code in frameworks/base/core/java/android/app/LoadedApk.java.
  * TODO: Remove if a cleaner mechanism is provided (either public API or AAPT is changed to generate
  * this code).
  */
class ResourceRewriter
    : public Object
{
public:
    static CARAPI RewriteRValues(
        /* [in] */ IContext* ctx);
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_RESOURCEREWRITER_H_

