
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/HelpUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Settings {

const String HelpUtils::TAG("Elastos.Droid.Settings.HelpUtils");
const String HelpUtils::PARAM_LANGUAGE_CODE("hl");
const String HelpUtils::PARAM_VERSION("version");
String HelpUtils::sCachedVersionCode = String(NULL);

Boolean HelpUtils::PrepareHelpMenuItem(
    /* [in] */ IContext* context,
    /* [in] */ IMenuItem* helpMenuItem,
    /* [in] */ Int32 helpUrlResourceId)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String helpUrlString;
    resources->GetString(helpUrlResourceId, &helpUrlString);
    return PrepareHelpMenuItem(context, helpMenuItem, helpUrlString);
}

Boolean HelpUtils::PrepareHelpMenuItem(
    /* [in] */ IContext* context,
    /* [in] */ IMenuItem* helpMenuItem,
    /* [in] */ const String& helpUrlString)
{
    if (TextUtils::IsEmpty(helpUrlString)) {
        // The help url string is empty or NULL, so set the help menu item to be invisible.
        helpMenuItem->SetVisible(FALSE);

        // return that the help menu item is not visible (i.e. FALSE)
        return FALSE;
    }
    else {
        // The help url string exists, so first add in some extra query parameters.
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> baseUri;
        helper->Parse(helpUrlString, (IUri**)&baseUri);
        AutoPtr<IUri> fullUri = UriWithAddedParameters(context, baseUri);

        // Then, create an intent that will be fired when the user
        // selects this help menu item.
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_VIEW, fullUri, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

        // Set the intent to the help menu item, show the help menu item in the overflow
        // menu, and make it visible.
        AutoPtr<IPackageManager> manager;
        context->GetPackageManager((IPackageManager**)&manager);
        AutoPtr<IComponentName> component;
        intent->ResolveActivity(manager, (IComponentName**)&component);
        if (component != NULL) {
            helpMenuItem->SetIntent(intent);
            helpMenuItem->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
            helpMenuItem->SetVisible(TRUE);
        }
        else {
            helpMenuItem->SetVisible(FALSE);
            return FALSE;
        }

        // return that the help menu item is visible (i.e., TRUE)
        return TRUE;
    }
}

AutoPtr<IUri> HelpUtils::UriWithAddedParameters(
    /* [in] */ IContext* context,
    /* [in] */ IUri* baseUri)
{
    AutoPtr<IUriBuilder> builder;
    baseUri->BuildUpon((IUriBuilder**)&builder);

    // Add in the preferred language
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String str;
    locale->ToString(&str);
    builder->AppendQueryParameter(PARAM_LANGUAGE_CODE, str);

    // Add in the package version code
    if (sCachedVersionCode.IsNull()) {
        // There is no cached version code, so try to get it from the package manager.
        // try {
        // cache the version code
        String packageName;
        context->GetPackageName(&packageName);
        AutoPtr<IPackageManager> manager;
        context->GetPackageManager((IPackageManager**)&manager);
        AutoPtr<IPackageInfo> info;
        manager->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
        Int32 versionCode;
        info->GetVersionCode(&versionCode);
        sCachedVersionCode = StringUtils::ToString(versionCode);

        // append the version code to the uri
        builder->AppendQueryParameter(PARAM_VERSION, sCachedVersionCode);
        // } catch (NameNotFoundException e) {
        //     // Cannot find the package name, so don't add in the version parameter
        //     // This shouldn't happen.
        //     Log->Wtf(TAG, "Invalid package name for context", e);
        // }
    }
    else {
        builder->AppendQueryParameter(PARAM_VERSION, sCachedVersionCode);
    }

    // Build the full uri and return it
    AutoPtr<IUri> result;
    builder->Build((IUri**)&result);
    return result;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
