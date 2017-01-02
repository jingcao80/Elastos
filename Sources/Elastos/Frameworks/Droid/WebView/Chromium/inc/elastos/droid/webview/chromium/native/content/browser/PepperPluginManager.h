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

// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * {@link PepperPluginManager} collects meta data about plugins from preloaded android apps
  * that reply to PEPPERPLUGIN intent query.
  */
class PepperPluginManager
{
public:
    /**
      * Collects information about installed plugins and returns a plugin description
      * string, which will be appended to for command line to load plugins.
      *
      * @param context Android context
      * @return        Description string for plugins
      */
    static CARAPI_(String) GetPlugins(
        /* [in] */ /*const*/ IContext* context);

private:
    static CARAPI_(String) GetPluginDescription(
        /* [in] */ IBundle* metaData);

public:
    /**
      * Service Action: A plugin wishes to be loaded in the ContentView must
      * provide {@link android.content.IntentFilter IntentFilter} that accepts
      * this action in its AndroidManifest.xml.
      */
    static const String PEPPER_PLUGIN_ACTION;
    static const String PEPPER_PLUGIN_ROOT;

private:
    static const String LOGTAG;
    // A plugin will specify the following fields in its AndroidManifest.xml.
    static const String FILENAME;
    static const String MIMETYPE;
    static const String NAME;
    static const String DESCRIPTION;
    static const String VERSION;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_
