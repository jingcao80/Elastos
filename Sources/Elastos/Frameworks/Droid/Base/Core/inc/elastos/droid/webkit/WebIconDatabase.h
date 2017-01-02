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

#ifndef __ELASTOS_DROID_WEBKIT_WEBICONDATABASE_H__
#define __ELASTOS_DROID_WEBKIT_WEBICONDATABASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/WebViewFactory.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Functions for manipulating the icon database used by WebView.
 * These functions require that a WebView be constructed before being invoked
 * and WebView.getIconDatabase() will return a WebIconDatabase object. This
 * WebIconDatabase object is a single instance and all methods operate on that
 * single object.
 */
class WebIconDatabase
{
public:
    /**
     * Get the global instance of WebIconDatabase.
     * @return A single instance of WebIconDatabase. It will be the same
     *         instance for the current process each time this method is
     *         called.
     */
    static CARAPI_(AutoPtr<IWebIconDatabase>) GetInstance();
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBICONDATABASE_H__
