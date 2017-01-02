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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBICONDATABASEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBICONDATABASEADAPTER_H_

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
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

