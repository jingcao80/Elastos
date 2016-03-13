
#include "elastos/droid/launcher2/AllAppsList.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int32 AllAppsList::DEFAULT_APPLICATIONS_NUMBER = 42;

AllAppsList::AllAppsList(
    /* [in] */ IconCache* iconCache)
{
    CArrayList::New(DEFAULT_APPLICATIONS_NUMBER, (IArrayList**)&mData);
    CArrayList::New(DEFAULT_APPLICATIONS_NUMBER, (IArrayList**)&mAdded);
    CArrayList::New((IArrayList**)&mRemoved);
    CArrayList::New((IArrayList**)&mModified);

    mIconCache = iconCache;
}

ECode AllAppsList::Add(
    /* [in] */ ApplicationInfo* info)
{

    if (FindActivity(mData, info->mComponentName, info->mUser)) {
        return NOERROR;
    }
    mData->Add(info);
    mAdded->Add(info);
    return NOERROR;
}

ECode AllAppsList::Clear()
{
    mData->Clear();
    // TODO: do we clear these too?
    mAdded->Clear();
    mRemoved->Clear();
    mModified->Clear();
    return NOERROR;
}

ECode AllAppsList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    return mData->GetSize(size);
}

ECode AllAppsList::Get(
    /* [in] */ Int32 index,
    /* [out] */ ApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<IInterface> obj;
    data->Get(index, (IInterface**)&obj);
    *info = (ApplicationInfo*)IObject::Probe(obj);
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode AllAppsList::AddPackage(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    AutoPtr<ILauncherApps> launcherApps = ILauncherApps::Probe(obj);
    AutoPtr<IList> matches;
    launcherApps->GetActivityList(packageName, user, (IList**)&matches);

    Int32 size;
    matches->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        matches->Get(i, (IInterface**)&tmp);
        AutoPtr<ILauncherActivityInfo> info = ILauncherActivityInfo::Probe(tmp);
        AutoPtr<ApplicationInfo> ainfo = new ApplicationInfo(info, user, mIconCache, NULL);
        Add(ainfo);
    }
    return NOERROR;
}

ECode AllAppsList::RemovePackage(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IList> data = mData;
    Int32 size;
    data->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        data->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IObject::Probe(obj);

        AutoPtr<IComponentName> component;
        info->mIntent->GetComponent((IComponentName**)&component);

        Boolean res;
        info->mUser->Equals(user, &res);
        if (res) {
            String name;
            component->GetPackageName(&name);
            Boolean tmp;
            packageName.Equals(name, &tmp);
            if (tmp) {
                removed->Add(info);
                data->Remove(i);
            }
        }
    }
    // This is more aggressive than it needs to be.
    return mIconCache->Flush();
}

ECode AllAppsList::UpdatePackage(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    AutoPtr<ILauncherApps> launcherApps = ILauncherApps::Probe(obj);
    AutoPtr<IList> matches;
    launcherApps->GetActivityList(packageName, user, (IList**)&matches);

    Int32 size;
    matches->Gize(size);
    if (size > 0) {
        // Find disabled/removed activities and remove them from data and add them
        // to the removed list.
        Int32 _size;
        mData->GetSize(&_size);
        for (Int32 i = _size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mData->Get(i, (IInterface**)&obj);
            AutoPtr<ApplicationInfo> applicationInfo = (ApplicationInfo*)IObject::Probe(obj);
            AutoPtr<IComponentName> component;
            applicationInfo->mIntent->GetComponent((IComponentName**)&component);

            Boolean res;
            user->Equals(applicationInfo->mUser, &res);
            if (res) {
                String pname;
                component->GetPackageName(&pname);
                Boolean tmp;
                packageName.Equals(pname, &tmp);
                if (tmp)) {
                    if (!FindActivity(matches, component, user)) {
                        mRemoved->Add(applicationInfo);
                        mIconCache->Remove(component);
                        mData->Remove(i);
                    }
                }
            }
        }

        // Find enabled activities and add them to the adapter
        // Also updates existing activities with new labels/icons
        Int32 count;
        matches->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            mMatches->Get(i, (IInterface**)&obj);
            AutoPtr<ILauncherActivityInfo> info = ILauncherActivityInfo::Preobe(obj);

            AutoPtr<IComponentName> name;
            info->GetComponentName((IComponentName**)&name);
            String pname;
            name->GetPackageName(&pname);
            String cname;
            name->GetShortClassName(&cname);
            AutoPtr<ApplicationInfo> applicationInfo = FindApplicationInfoLocked(
                    pname,
                    cname,
                    user);
            if (applicationInfo == NULL) {
                AutoPtr<ApplicationInfo> tmp = new ApplicationInfo(info, user, mIconCache, NULL);
                Add(tmp);
            }
            else {
                mIconCache->Remove(applicationInfo->mComponentName);
                mIconCache->GetTitleAndIcon(applicationInfo, info, NULL);
                modified->Add(applicationInfo);
            }
        }
    }
    else {
        // Remove all data for this package.
        Int32 size;
        mData->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mData->Get(i, (IInterface**)&obj);
            AutoPtr<ApplicationInfo> applicationInfo = (ApplicationInfo*)IObject::Probe(obj);
            AutoPtr<IComponentName> component;
            applicationInfo->mIntent->GetComponent((IComponentName**)&component);

            Boolean res;
            user->Equals(applicationInfo->mUser, &res);
            if (res) {
                String pname;
                component->GetPackageName(&pname);
                Boolean tmp;
                packageName.Equals(pname, &tmp);
                if (tmp) {
                    mRemoved->Add(applicationInfo);
                    mIconCache->Remove(component);
                    mData->Remove(i);
                }
            }
        }
    }
    return NOERROR;
}

Boolean AllAppsList::FindActivity(
    /* [in] */ IList* apps,
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user)
{
    Int32 size;
    apps->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        apps->Get(i, (IInterface**)&obj);
        AutoPtr<ILauncherActivityInfo> info = ILauncherActivityInfo::Probe(obj);

        AutoPtr<IUserHandle> handle;
        info->GetUser((IUserHandle**)&handle);
        Boolean res;
        handle->Equals(user, &res);
        if (res) {
            AutoPtr<IComponentName> name;
            info->GetComponentName((IComponentName**)&name);
            Boolean tmp;
            name->Equals(component, &tmp);
            if (tmp) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean AllAppsList::FindActivity(
    /* [in] */ IArrayList* apps,
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user)
{
    Int32 N;
    apps->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        apps->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IObject::Probe(obj);

        Boolean res;
        info->mUser->Equals(user, &res);
        if (res) {
            AutoPtr<IComponentName> name;
            info->GetComponentName((IComponentName**)&name);
            Boolean tmp;
            name->Equals(component, &tmp);
            if (tmp) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

AutoPtr<ApplicationInfo> AllAppsList::FindApplicationInfoLocked(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ IUserHandle* user)
{
    Int32 size;
    mData->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mData->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IObject::Probe(obj);

        AutoPtr<IComponentName> component;
        info->mIntent->GetComponent((IComponentName**)&component);

        Boolean res;
        user->Equals(info->mUser, &res);
        if (res) {
            String pname;
            component->GetPackageName(&pname);
            Boolean tmp;
            packageName.Equals(pname, &tmp);
            if (tmp) {
                String cname;
                component->GetClassName(&cname);
                Boolean result;
                className.Equals(cname, &result);
                if (result) {
                    return info;
                }
            }
        }
    }
    return NULL;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos