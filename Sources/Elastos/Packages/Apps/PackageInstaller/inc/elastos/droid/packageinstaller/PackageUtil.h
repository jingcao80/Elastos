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

#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_PACKAGEUTIL_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_PACKAGEUTIL_H__

#include "elastos/droid/content/pm/PackageParser.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include "elastos/core/Object.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

/**
  * This is a utility class for defining some utility methods and constants
  * used in the package installer application.
  */
class PackageUtil
{
public:
    class AppSnippet
        : public Object
    {
    public:
        AppSnippet(
            /* [in] */ ICharSequence* label,
            /* [in] */ IDrawable* icon);

    public:
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<IDrawable> mIcon;
    };

private:
  PackageUtil();

public:
    /**
      * Utility method to get application information for a given {@link File}
      */
    static CARAPI_(AutoPtr<IApplicationInfo>) GetApplicationInfo(
        /* [in] */ IFile* sourcePath);

    /**
      * Utility method to get package information for a given {@link File}
      */
    static CARAPI_(AutoPtr<PackageParser::Package>) GetPackageInfo(
        /* [in] */ IFile* sourceFile);

    static CARAPI_(AutoPtr<IView>) InitSnippet(
        /* [in] */ IView* snippetView,
        /* [in] */ ICharSequence* label,
        /* [in] */ IDrawable* icon);

    /**
      * Utility method to display a snippet of an installed application.
      * The content view should have been set on context before invoking this method.
      * appSnippet view should include R.id.app_icon and R.id.app_name
      * defined on it.
      *
      * @param pContext context of package that can load the resources
      * @param componentInfo ComponentInfo object whose resources are to be loaded
      * @param snippetView the snippet view
      */
    static CARAPI_(AutoPtr<IView>) InitSnippetForInstalledApp(
        /* [in] */ IActivity* pContext,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IView* snippetView);

    /**
      * Utility method to display a snippet of an installed application.
      * The content view should have been set on context before invoking this method.
      * appSnippet view should include R.id.app_icon and R.id.app_name
      * defined on it.
      *
      * @param pContext context of package that can load the resources
      * @param componentInfo ComponentInfo object whose resources are to be loaded
      * @param snippetView the snippet view
      * @param UserHandle user that the app si installed for.
      */
    static CARAPI_(AutoPtr<IView>) InitSnippetForInstalledApp(
        /* [in] */ IActivity* pContext,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IView* snippetView,
        /* [in] */ IUserHandle* user);

    /**
      * Utility method to display application snippet of a new package.
      * The content view should have been set on context before invoking this method.
      * appSnippet view should include R.id.app_icon and R.id.app_name
      * defined on it.
      *
      * @param pContext context of package that can load the resources
      * @param appInfo ApplicationInfo object of package whose resources are to be loaded
      * @param snippetId view id of app snippet view
      */
    static CARAPI_(AutoPtr<IView>) InitSnippetForNewApp(
        /* [in] */ IActivity* pContext,
        /* [in] */ AppSnippet* as,
        /* [in] */ Int32 snippetId);

    static CARAPI_(Boolean) IsPackageAlreadyInstalled(
        /* [in] */ IActivity* context,
        /* [in] */ const String& pkgName);

    /**
      * Utility method to load application label
      *
      * @param pContext context of package that can load the resources
      * @param appInfo ApplicationInfo object of package whose resources are to be loaded
      * @param snippetId view id of app snippet view
      */
    static CARAPI_(AutoPtr<PackageUtil::AppSnippet>) GetAppSnippet(
        /* [in] */ IActivity* pContext,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IFile* sourceFile);

public:
    static const String PREFIX;
    static const String INTENT_ATTR_INSTALL_STATUS;
    static const String INTENT_ATTR_APPLICATION_INFO;
    static const String INTENT_ATTR_PERMISSIONS_LIST;
    //intent attribute strings related to uninstall
    static const String INTENT_ATTR_PACKAGE_NAME;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_PACKAGEUTIL_H__

