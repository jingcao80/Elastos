
#include <Elastos.Droid.Net.h>
#include "elastos/apps/packageinstaller/RemoveReceiver.h"

using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                            RemoveReceiver
//=====================================================================
CAR_INTERFACE_IMPL(RemoveReceiver, Object, IBroadcastReceiver)

ECode RemoveReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // if (Intent.ACTION_PACKAGE_FULLY_REMOVED.equals(intent.getAction())) {
    //     Uri uri = intent.getData();
    //     String pkg = uri != null ? uri.getSchemeSpecificPart() : null;
    //     if (pkg != null) {
    //         SharedPreferences prefs = context.getSharedPreferences(
    //                 PackageInstallerActivity.PREFS_ALLOWED_SOURCES,
    //                 Context.MODE_PRIVATE);
    //         if (prefs.getBoolean(pkg, false)) {
    //             prefs.edit().remove(pkg).apply();
    //         }
    //     }
    // }

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PACKAGE_FULLY_REMOVED.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        String scheme;
        uri->GetSchemeSpecificPart(&scheme);
        String pkg = uri != NULL ? scheme : String("");
        if (pkg != NULL) {
            AutoPtr<ISharedPreferences> prefs;
            //context->GetSharedPreferences(PackageInstallerActivity::PREFS_ALLOWED_SOURCES, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
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
} // namespace Apps
} // namespace Elastos


