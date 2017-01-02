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

#ifndef __ELASTOS_DROID_SETTINGS_HELPUTILS_H__
#define __ELASTOS_DROID_SETTINGS_HELPUTILS_H__

#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Functions to easily prepare contextual help menu option items with an intent that opens up the
 * browser to a particular URL, while taking into account the preferred language and app version.
 */
class HelpUtils
{
public:
    /**
     * Prepares the help menu item by doing the following.
     * - If the string corresponding to the helpUrlResourceId is empty or NULL, then the help menu
     *   item is made invisible.
     * - Otherwise, this makes the help menu item visible and sets the intent for the help menu
     *   item to view the URL.
     *
     * @return returns whether the help menu item has been made visible.
     */
    static CARAPI_(Boolean) PrepareHelpMenuItem(
        /* [in] */ IContext* context,
        /* [in] */ IMenuItem* helpMenuItem,
        /* [in] */ Int32 helpUrlResourceId);

    /**
     * Prepares the help menu item by doing the following.
     * - If the helpUrlString is empty or NULL, the help menu item is made invisible.
     * - Otherwise, this makes the help menu item visible and sets the intent for the help menu
     *   item to view the URL.
     *
     * @return returns whether the help menu item has been made visible.
     */
    static CARAPI_(Boolean) PrepareHelpMenuItem(
        /* [in] */ IContext* context,
        /* [in] */ IMenuItem* helpMenuItem,
        /* [in] */ const String& helpUrlString);

    /**
     * Adds two query parameters into the Uri, namely the language code and the version code
     * of the app's package as gotten via the context.
     * @return the uri with added query parameters
     */
    static CARAPI_(AutoPtr<IUri>) UriWithAddedParameters(
        /* [in] */ IContext* context,
        /* [in] */ IUri* baseUri);

private:
    /** Static helper that is not instantiable*/
    HelpUtils() { }

private:
    static const String TAG;

    /**
     * Help URL query parameter key for the preferred language.
     */
    static const String PARAM_LANGUAGE_CODE;

    /**
     * Help URL query parameter key for the app version.
     */
    static const String PARAM_VERSION;

    /**
     * Cached version code to prevent repeated calls to the package manager.
     */
    static String sCachedVersionCode;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_HELPUTILS_H__
