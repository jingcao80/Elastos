
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApplicationStatus.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/LocalizationUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/LocalizationUtils_dec.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApiCompatibilityUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApplicationStatus;
using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//                          LocalizationUtils
//=====================================================================
const Int32 LocalizationUtils::UNKNOWN_DIRECTION;
const Int32 LocalizationUtils::RIGHT_TO_LEFT;
const Int32 LocalizationUtils::LEFT_TO_RIGHT;
Boolean LocalizationUtils::sIsLayoutRtl;

String LocalizationUtils::GetDefaultLocale()
{
    // ==================before translated======================
    // Locale locale = Locale.getDefault();
    // String language = locale.getLanguage();
    // String country = locale.getCountry();
    //
    // // Android uses deprecated lanuages codes for Hebrew and Indonesian but Chromium uses the
    // // updated codes. Also, Android uses "tl" while Chromium uses "fil" for Tagalog/Filipino.
    // // So apply a mapping.
    // // See http://developer.android.com/reference/java/util/Locale.html
    // if ("iw".equals(language)) {
    //     language = "he";
    // } else if ("in".equals(language)) {
    //     language = "id";
    // } else if ("tl".equals(language)) {
    //     language = "fil";
    // }
    // return country.isEmpty() ? language : language + "-" + country;

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String language;
    locale->GetLanguage(&language);
    String country;
    locale->GetCountry(&country);

    // Android uses deprecated lanuages codes for Hebrew and Indonesian but Chromium uses the
    // updated codes. Also, Android uses "tl" while Chromium uses "fil" for Tagalog/Filipino.
    // So apply a mapping.
    // See http://developer.android.com/reference/java/util/Locale.html
    if (String("iw") == language) {
        language = String("he");
    }
    else if (String("in") == language) {
        language = String("id");
    }
    else if (String("tl") == language) {
        language = String("fil");
    }
    return country.IsEmpty() ? language : language + String("-") + country;
}

Boolean LocalizationUtils::IsLayoutRtl()
{
    // ==================before translated======================
    // if (sIsLayoutRtl == null) {
    //     Configuration configuration =
    //             ApplicationStatus.getApplicationContext().getResources().getConfiguration();
    //     sIsLayoutRtl = Boolean.valueOf(
    //             ApiCompatibilityUtils.getLayoutDirection(configuration) ==
    //             View.LAYOUT_DIRECTION_RTL);
    // }
    //
    // return sIsLayoutRtl.booleanValue();

    if (FALSE == sIsLayoutRtl) {
        AutoPtr<IContext> context = ApplicationStatus::GetApplicationContext();
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);

        AutoPtr<IConfiguration> configuration;
        resources->GetConfiguration((IConfiguration**)&configuration);

        Int32 layoutDirect = ApiCompatibilityUtils::GetLayoutDirection(configuration);
        sIsLayoutRtl = (Boolean)(layoutDirect == IView::LAYOUT_DIRECTION_RTL);
    }

    return sIsLayoutRtl;
}

Int32 LocalizationUtils::GetFirstStrongCharacterDirection(
    /* [in] */ const String& string)
{
    // ==================before translated======================
    // return nativeGetFirstStrongCharacterDirection(string);

    return NativeGetFirstStrongCharacterDirection(string);
}

String LocalizationUtils::GetDurationString(
    /* [in] */ Int64 timeInMillis)
{
    // ==================before translated======================
    // return nativeGetDurationString(timeInMillis);

    return NativeGetDurationString(timeInMillis);
}

LocalizationUtils::LocalizationUtils()
{
    // ==================before translated======================
    // /* cannot be instantiated */
}

AutoPtr<IInterface> LocalizationUtils::GetJavaLocale(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    // ==================before translated======================
    // return new Locale(language, country, variant);

    AutoPtr<ILocale> locale;
    CLocale::New(language, country, variant, (ILocale**)&locale);
    return TO_IINTERFACE(locale);
}

String LocalizationUtils::GetDisplayNameForLocale(
    /* [in] */ ILocale* locale,
    /* [in] */ ILocale* displayLocale)
{
    // ==================before translated======================
    // return locale.getDisplayName(displayLocale);

    String result;
    locale->GetDisplayName(displayLocale, &result);
    return result;
}

Int32 LocalizationUtils::NativeGetFirstStrongCharacterDirection(
    /* [in] */ const String& string)
{
    return Elastos_LocalizationUtils_nativeGetFirstStrongCharacterDirection(string);
}

String LocalizationUtils::NativeGetDurationString(
    /* [in] */ Int64 timeInMillis)
{
    return Elastos_LocalizationUtils_nativeGetDurationString(timeInMillis);
}

String LocalizationUtils::GetDisplayNameForLocale(
    /* [in] */ IInterface* locale,
    /* [in] */ IInterface* displayLocale)
{
    ILocale* l = ILocale::Probe(locale);
    ILocale* dl = ILocale::Probe(displayLocale);
    return GetDisplayNameForLocale(l, dl);
}


} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


