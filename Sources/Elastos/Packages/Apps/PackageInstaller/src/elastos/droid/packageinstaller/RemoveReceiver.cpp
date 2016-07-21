
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
