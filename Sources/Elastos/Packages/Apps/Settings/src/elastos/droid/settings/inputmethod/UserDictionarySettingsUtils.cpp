
#include "elastos/droid/settings/inputmethod/UserDictionarySettingsUtils.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Settings::Utils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

String UserDictionarySettingsUtils::GetLocaleDisplayName(
    /* [in] */ IContext* context,
    /* [in] */ const String& localeStr)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String result;
    if (TextUtils::IsEmpty(localeStr)) {
        // CAVEAT: localeStr should not be NULL because a NULL locale stands for the system
        // locale in UserDictionary.Words.addWord.
        resources->GetString(R::string::user_dict_settings_all_languages, &result);
        return result;
    }
    AutoPtr<ILocale> locale = Utils::CreateLocaleFromString(localeStr);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> systemLocale;
    config->GetLocale((ILocale**)&systemLocale);
    locale->GetDisplayName(systemLocale, &result);
    return result;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos