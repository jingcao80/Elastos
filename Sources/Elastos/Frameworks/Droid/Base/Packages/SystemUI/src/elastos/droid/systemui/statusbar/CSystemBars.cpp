
#include "elastos/droid/systemui/statusbar/CSystemBars.h"
#include "../R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const String CSystemBars::TAG("SystemBars");
const Boolean CSystemBars::DEBUG = FALSE;
const Int32 CSystemBars::WAIT_FOR_BARS_TO_DIE = 500;

CAR_OBJECT_IMPL(CSystemBars);
CAR_INTERFACE_IMPL(CSystemBars, SystemUI, IServiceMonitorCallbacks);
ECode CSystemBars::Start()
{
    if (DEBUG) Logger::D(TAG, "start");
    mServiceMonitor = new ServiceMonitor(TAG, DEBUG,
            mContext, ISettingsSecure::BAR_SERVICE_COMPONENT, this);
    mServiceMonitor->Start();  // will call onNoService if no remote service is found
    return NOERROR;
}

ECode CSystemBars::OnNoService()
{
    if (DEBUG) Logger::D(TAG, "onNoService");
    CreateStatusBarFromConfig();  // fallback to using an in-process implementation
    return NOERROR;
}

ECode CSystemBars::OnServiceStartAttempt(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (DEBUG) Logger::D(TAG, "onServiceStartAttempt mStatusBar=%p", mStatusBar.Get());
    if (mStatusBar != NULL) {
        // tear down the in-process version, we'll recreate it again if needed
        mStatusBar->Destroy();
        mStatusBar = NULL;
        *result = WAIT_FOR_BARS_TO_DIE;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CSystemBars::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (mStatusBar != NULL) {
        ((SystemUI*)ISystemUI::Probe(mStatusBar))->OnConfigurationChanged(newConfig);
    }
    return NOERROR;
}

ECode CSystemBars::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    if (mStatusBar != NULL) {
        ISystemUI::Probe(mStatusBar)->Dump(fd, pw, args);
    }
    return NOERROR;
}

void CSystemBars::CreateStatusBarFromConfig()
{
    if (DEBUG) Logger::D(TAG, "createStatusBarFromConfig");
    String clsName;
    mContext->GetString(R::string::config_statusBarComponent, &clsName);
    if (clsName == NULL || clsName.GetLength() == 0) {
        Logger::D(TAG, "No status bar component configured");
    }
    AutoPtr<IClassInfo> cls = Utils::GetClassInfo(clsName);
    if (cls == NULL) {
        Logger::D(TAG, "Error loading status bar component: %s", clsName.string());
        return;
    }

    AutoPtr<IInterface> object;
    if (FAILED(cls->CreateObject((IInterface**)&object))) {
        Logger::D(TAG, "Error creating status bar component: %s", clsName.string());
        return;
    }
    mStatusBar = IBaseStatusBar::Probe(object);
    assert(0 && "TODO");
    // mStatusBar->mContext = mContext;
    // mStatusBar->mComponents = mComponents;
    mStatusBar->Start();
    // if (DEBUG) Logger::D(TAG, "started " + mStatusBar.getClass().GetSimpleName());
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
