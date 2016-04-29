
#include "elastos/droid/systemui/CDessertCase.h"
#include "elastos/droid/systemui/CDessertCaseView.h"
#include "elastos/droid/systemui/Utils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::IView;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_OBJECT_IMPL(CDessertCase);
CAR_INTERFACE_IMPL(CDessertCase, Activity, IDessertCase);
ECode CDessertCase::OnStart()
{
    Activity::OnStart();

    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;

    AutoPtr<IClassInfo> classInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.CDessertCaseDream"));
    CComponentName::New(this, classInfo, (IComponentName**)&cn);
    Int32 value = 0;
    pm->GetComponentEnabledSetting(cn, &value);
    if (value != IPackageManager::COMPONENT_ENABLED_STATE_ENABLED) {
        Slogger::V("DessertCase", "ACHIEVEMENT UNLOCKED");
        pm->SetComponentEnabledSetting(cn,
                IPackageManager::COMPONENT_ENABLED_STATE_ENABLED,
                IPackageManager::DONT_KILL_APP);
    }

    CDessertCaseView::New(this, (IDessertCaseView**)&mView);
    AutoPtr<CDessertCaseView::RescalingContainer> container =
        new CDessertCaseView::RescalingContainer(this);

    container->SetView(mView);
    SetContentView(IView::Probe(container));
    return NOERROR;
}

ECode CDessertCase::OnResume()
{
    Activity::OnResume();
    AutoPtr<Runnable_1> r = new Runnable_1(this);
    Boolean res = FALSE;
    IView::Probe(mView)->PostDelayed(r, 1000, &res);
    return NOERROR;
}

ECode CDessertCase::OnPause()
{
    Activity::OnPause();
    mView->Stop();
    return NOERROR;
}

//===============================================================
// CDessertCase::Runnable_1
//===============================================================
CDessertCase::Runnable_1::Runnable_1(
    /* [in] */ CDessertCase* owner)
    : mOwner(owner)
{
}

ECode CDessertCase::Runnable_1::Run()
{
    mOwner->mView->Start();
    return NOERROR;
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos
