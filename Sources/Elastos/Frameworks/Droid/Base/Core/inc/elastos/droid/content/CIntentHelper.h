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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTHELPER_H__

#include "_Elastos_Droid_Content_CIntentHelper.h"
#include "elastos/droid/content/Intent.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentHelper)
    , public Singleton
    , public IIntentHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Convenience function for creating a {@link #ACTION_CHOOSER} Intent.
     *
     * <p>Builds a new {@link #ACTION_CHOOSER} Intent that wraps the given
     * target intent, also optionally supplying a title.  If the target
     * intent has specified {@link #FLAG_GRANT_READ_URI_PERMISSION} or
     * {@link #FLAG_GRANT_WRITE_URI_PERMISSION}, then these flags will also be
     * set in the returned chooser intent, with its ClipData set appropriately:
     * either a direct reflection of {@link #getClipData()} if that is non-null,
     * or a new ClipData build from {@link #getData()}.
     *
     * @param target The Intent that the user will be selecting an activity
     * to perform.
     * @param title Optional title that will be displayed in the chooser.
     * @return Return a new Intent object that you can hand to
     * {@link Context#startActivity(Intent) Context.startActivity()} and
     * related methods.
     */
    CARAPI CreateChooser(
        /* [in] */ IIntent* target,
        /* [in] */ ICharSequence* title,
        /* [out] */ IIntent** intent);

    /**
     * Create an intent to launch the main (root) activity of a task.  This
     * is the Intent that is started when the application's is launched from
     * Home.  For anything else that wants to launch an application in the
     * same way, it is important that they use an Intent structured the same
     * way, and can use this function to ensure this is the case.
     *
     * <p>The returned Intent has the given Activity component as its explicit
     * component, {@link #ACTION_MAIN} as its action, and includes the
     * category {@link #CATEGORY_LAUNCHER}.  This does <em>not</em> have
     * {@link #FLAG_ACTIVITY_NEW_TASK} set, though typically you will want
     * to do that through {@link #addFlags(int)} on the returned Intent.
     *
     * @param mainActivity The main activity component that this Intent will
     * launch.
     * @return Returns a newly created Intent that can be used to launch the
     * activity as a main application entry.
     *
     * @see #setClass
     * @see #setComponent
     */
    CARAPI MakeMainActivity(
        /* [in] */ IComponentName* mainActivity,
        /* [out] */ IIntent** intent);

    /**
     * Make an Intent for the main activity of an application, without
     * specifying a specific activity to run but giving a selector to find
     * the activity.  This results in a final Intent that is structured
     * the same as when the application is launched from
     * Home.  For anything else that wants to launch an application in the
     * same way, it is important that they use an Intent structured the same
     * way, and can use this function to ensure this is the case.
     *
     * <p>The returned Intent has {@link #ACTION_MAIN} as its action, and includes the
     * category {@link #CATEGORY_LAUNCHER}.  This does <em>not</em> have
     * {@link #FLAG_ACTIVITY_NEW_TASK} set, though typically you will want
     * to do that through {@link #addFlags(int)} on the returned Intent.
     *
     * @param selectorAction The action name of the Intent's selector.
     * @param selectorCategory The name of a category to add to the Intent's
     * selector.
     * @return Returns a newly created Intent that can be used to launch the
     * activity as a main application entry.
     *
     * @see #setSelector(Intent)
     */
    CARAPI MakeMainSelectorActivity(
        /* [in] */ const String& selectorAction,
        /* [in] */ const String& selectorCategory,
        /* [out] */ IIntent** intent);

    /**
     * Make an Intent that can be used to re-launch an application's task
     * in its base state.  This is like {@link #makeMainActivity(ComponentName)},
     * but also sets the flags {@link #FLAG_ACTIVITY_NEW_TASK} and
     * {@link #FLAG_ACTIVITY_CLEAR_TASK}.
     *
     * @param mainActivity The activity component that is the root of the
     * task; this is the activity that has been published in the application's
     * manifest as the main launcher icon.
     *
     * @return Returns a newly created Intent that can be used to relaunch the
     * activity's task in its root state.
     */
    CARAPI MakeRestartActivityTask(
        /* [in] */ IComponentName* mainActivity,
        /* [out] */ IIntent** intent);

    /**
     * Call {@link #parseUri} with 0 flags.
     * @deprecated Use {@link #parseUri} instead.
     */
    CARAPI GetIntent(
        /* [in] */ const String& uri,
        /* [out] */ IIntent** intent);

    /**
     * Create an intent from a URI.  This URI may encode the action,
     * category, and other intent fields, if it was returned by
     * {@link #toUri}.  If the Intent was not generate by toUri(), its data
     * will be the entire URI and its action will be ACTION_VIEW.
     *
     * <p>The URI given here must not be relative -- that is, it must include
     * the scheme and full path.
     *
     * @param uri The URI to turn into an Intent.
     * @param flags Additional processing flags.  Either 0 or
     * {@link #URI_INTENT_SCHEME}.
     *
     * @return Intent The newly created Intent object.
     *
     * @throws URISyntaxException Throws URISyntaxError if the basic URI syntax
     * it bad (as parsed by the Uri class) or the Intent data within the
     * URI is invalid.
     *
     * @see #toUri
     */
    CARAPI ParseUri(
        /* [in] */ const String& uri,
        /* [in] */ Int32 flags,
        /* [out] */ IIntent** intent);

    CARAPI GetIntentOld(
        /* [in] */ const String& uri,
        /* [out] */ IIntent** intent);

    /**
     * Parses the "intent" element (and its children) from XML and instantiates
     * an Intent object.  The given XML parser should be located at the tag
     * where parsing should start (often named "intent"), from which the
     * basic action, data, type, and package and class name will be
     * retrieved.  The function will then parse in to any child elements,
     * looking for <category android:name="xxx"> tags to add categories and
     * <extra android:name="xxx" android:value="yyy"> to attach extra data
     * to the intent.
     *
     * @param resources The Resources to use when inflating resources.
     * @param parser The XML parser pointing at an "intent" tag.
     * @param attrs The AttributeSet interface for retrieving extended
     * attribute data at the current <var>parser</var> location.
     * @return An Intent object matching the XML data.
     * @throws XmlPullParserException If there was an XML parsing error.
     * @throws IOException If there was an I/O error.
     */
    CARAPI ParseIntent(
        /* [in] */ IResources* resources,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IIntent** intent);

    CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [out] */ IIntent** intent);

    /**
     * Normalize a MIME data type.
     *
     * <p>A normalized MIME type has white-space trimmed,
     * content-type parameters removed, and is lower-case.
     * This aligns the type with Android best practices for
     * intent filtering.
     *
     * <p>For example, "text/plain; charset=utf-8" becomes "text/plain".
     * "text/x-vCard" becomes "text/x-vcard".
     *
     * <p>All MIME types received from outside Android (such as user input,
     * or external sources like Bluetooth, NFC, or the Internet) should
     * be normalized before they are used to create an Intent.
     *
     * @param type MIME data type to normalize
     * @return normalized MIME data type, or null if the input was null
     * @see {@link #setType}
     * @see {@link #setTypeAndNormalize}
     */
    CARAPI NormalizeMimeType(
        /* [in] */ const String& type,
        /* [out] */ String* mimeType);

    CARAPI IsAccessUriMode(
        /* [in] */ Int32 modeFlags,
        /* [out] */ Boolean* result);
private:
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTHELPER_H__
