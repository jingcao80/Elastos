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