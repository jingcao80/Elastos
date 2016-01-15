// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_

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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PEPPERPLUGINMANAGER_H_
