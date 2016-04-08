#include "elastos/droid/systemui/recents/model/RecentsPackageMonitor.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/os/Looper.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

CAR_INTERFACE_IMPL(RecentsPackageMonitor, PackageMonitor, IRecentsPackageMonitor)

RecentsPackageMonitor::RecentsPackageMonitor()
{}

ECode RecentsPackageMonitor::Register(
    /* [in] */ IContext* context,
    /* [in] */ IPackageCallbacks* cb)
{
    mSystemServicesProxy = new SystemServicesProxy(context);
    mCb = cb;
    AutoPtr<ILooper> looper = Looper::GetMainLooper();
    ECode ec = PackageMonitor::Register(context, looper, TRUE);
    if (FAILED(ec)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode RecentsPackageMonitor::Unregister()
{
    ECode ec = PackageMonitor::Unregister();
    if (FAILED(ec)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mSystemServicesProxy = NULL;
    mCb = NULL;
    mTasks->Clear();
    return NOERROR;
}

ECode RecentsPackageMonitor::SetTasks(
    /* [in] */ IList* tasks)
{
    mTasks = tasks;
    return NOERROR;
}

ECode RecentsPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    if (mCb == NULL) return E_NULL_POINTER_EXCEPTION;

    // Identify all the tasks that should be removed as a result of the package being removed.
    // Using a set to ensure that we callback once per unique component.
    // HashSet<ComponentName> componentsToRemove = new HashSet<ComponentName>();
    AutoPtr<IHashSet> componentsToRemove;
    CHashSet::New((IHashSet**)&componentsToRemove);
    AutoPtr<IIterator> it;
    IIterable::Probe(mTasks)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ITaskKey> t = ITaskKey::Probe(obj);
        AutoPtr<Task::TaskKey> _t = (Task::TaskKey*)t.Get();
        AutoPtr<IIntent> baseIntent = _t->mBaseIntent;
        AutoPtr<IComponentName> cn;
        baseIntent->GetComponent((IComponentName**)&cn);
        String pn;
        cn->GetPackageName(&pn);
        if (pn.Equals(packageName)) {
            componentsToRemove->Add(cn);
        }
    }
    // Notify our callbacks that the components no longer exist
    mCb->OnComponentRemoved(componentsToRemove);
    return NOERROR;
}

ECode RecentsPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    OnPackageModified(packageName);
    *result = TRUE;
    return NOERROR;
}

ECode RecentsPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    if (mCb == NULL) return E_NULL_POINTER_EXCEPTION;

    // Identify all the tasks that should be removed as a result of the package being removed.
    // Using a set to ensure that we callback once per unique component.
    AutoPtr<IHashSet> componentsKnownToExist;
    CHashSet::New((IHashSet**)&componentsKnownToExist);
    AutoPtr<IHashSet> componentsToRemove;
    CHashSet::New((IHashSet**)&componentsToRemove);

    AutoPtr<IIterator> it;
    IIterable::Probe(mTasks)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ITaskKey> t = ITaskKey::Probe(obj);
        AutoPtr<Task::TaskKey> _t = (Task::TaskKey*)t.Get();
        AutoPtr<IIntent> baseIntent = _t->mBaseIntent;
        AutoPtr<IComponentName> cn;
        baseIntent->GetComponent((IComponentName**)&cn);
        String pn;
        cn->GetPackageName(&pn);
        if (pn.Equals(packageName)) {
            Boolean isContained;
            componentsKnownToExist->Contains(cn, &isContained);
            if (isContained) {
                // If we know that the component still exists in the package, then skip
                continue;
            }
            AutoPtr<IActivityInfo> ai = mSystemServicesProxy->GetActivityInfo(cn);
            if (ai != NULL) {
                componentsKnownToExist->Add(cn);
            }
            else {
                componentsToRemove->Add(cn);
            }
        }
    }

    // Notify our callbacks that the components no longer exist
    mCb->OnComponentRemoved(componentsToRemove);
    return NOERROR;
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos