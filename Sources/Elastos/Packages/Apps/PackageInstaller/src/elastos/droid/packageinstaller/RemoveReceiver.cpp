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

#include "elastos/droid/packageinstaller/RemoveReceiver.h"
#include "elastos/droid/packageinstaller/CPackageInstallerActivity.h"
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

ECode RemoveReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PACKAGE_FULLY_REMOVED.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        String scheme;
        uri->GetSchemeSpecificPart(&scheme);
        String pkg = uri != NULL ? scheme : String(NULL);
        if (pkg != NULL) {
            AutoPtr<ISharedPreferences> prefs;
            context->GetSharedPreferences(
                CPackageInstallerActivity::PREFS_ALLOWED_SOURCES, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
            Boolean resTmp = FALSE;
            prefs->GetBoolean(pkg, FALSE, &resTmp);
            if (resTmp) {
                AutoPtr<ISharedPreferencesEditor> edit;
                prefs->Edit((ISharedPreferencesEditor**)&edit);
                edit->Remove(pkg);
                edit->Apply();
            }
        }
    }
    return NOERROR;
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos
