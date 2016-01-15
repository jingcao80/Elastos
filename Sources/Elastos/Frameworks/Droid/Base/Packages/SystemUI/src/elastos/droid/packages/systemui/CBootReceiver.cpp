
#include "elastos/droid/packages/systemui/CBootReceiver.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

//===============================================================
// CBootReceiver::
//===============================================================
String CBootReceiver::TAG = String("SystemUIBootReceiver");
ECode CBootReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Start the load average overlay, if activated
    AutoPtr<IContentResolver> res;
    context->GetContentResolver((IContentResolver**)&res);

    Int32 value = 0;
    ECode ec = Elastos::Droid::Provider::Settings::Global::GetInt32(res, ISettingsGlobal::SHOW_PROCESSES, 0, &value);
    if (FAILED(ec)) {
        Logger::E(TAG, "Can't start load average service");
    }

    if (value != 0) {
        AutoPtr<IIntent> loadavg;
        assert(0 && "TODO");
        // CIntent::New(context, com.android.systemui.LoadAverageService.class);

        AutoPtr<IComponentName> name;
        if (FAILED(context->StartService(loadavg, (IComponentName**)&name))) {
            Logger::E(TAG, "Can't start load average service");
        }
    }

    return NOERROR;
}

} // namespace SystemUI
} // namespace Packages
} // namepsace Droid
} // namespace Elastos