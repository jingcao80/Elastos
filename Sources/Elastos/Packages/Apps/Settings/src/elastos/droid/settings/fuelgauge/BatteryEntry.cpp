
#include "Elastos.Droid.Os.h"
#include "elastos/droid/settings/fuelgauge/BatteryEntry.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_IDLE;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_CELL;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_PHONE;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_WIFI;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_BLUETOOTH;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_SCREEN;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_FLASHLIGHT;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_APP;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_USER;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_UNACCOUNTED;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_OVERCOUNTED;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

//===============================================================================
//                  BatteryEntry::NameAndIconLoader
//===============================================================================

BatteryEntry::NameAndIconLoader::NameAndIconLoader()
    : mAbort(FALSE)
{}

ECode BatteryEntry::NameAndIconLoader::constructor()
{
    return Thread::constructor(String("BatteryUsage Icon Loader"));
}

ECode BatteryEntry::NameAndIconLoader::Abort()
{
    mAbort = TRUE;
    return NOERROR;
}

ECode BatteryEntry::NameAndIconLoader::Run()
{
    while (TRUE) {
        BatteryEntry* be;
        {
            AutoLock syncLock(sRequestQueue);
            Boolean res;
            if ((sRequestQueue->IsEmpty(&res), res) || mAbort) {
                if (sHandler != NULL) {
                    sHandler->SendEmptyMessage(MSG_REPORT_FULLY_DRAWN, &res);
                }
                sRequestQueue->Clear();
                return NOERROR;
            }
            AutoPtr<IInterface> obj;
            sRequestQueue->Remove(0, (IInterface**)&obj);
            be = (BatteryEntry*)IBatteryEntry::Probe(obj);
        }
        be->LoadNameAndIcon();
    }
    return NOERROR;
}

//===============================================================================
//                  BatteryEntry
//===============================================================================

CAR_INTERFACE_IMPL(BatteryEntry, Object, IBatteryEntry)

const Int32 BatteryEntry::MSG_UPDATE_NAME_ICON;
const Int32 BatteryEntry::MSG_REPORT_FULLY_DRAWN;

static AutoPtr<IHashMap> InitsUidCache()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

const AutoPtr<IHashMap> BatteryEntry::sUidCache = InitsUidCache();

static AutoPtr<IArrayList> InitsRequestQueue()
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    return list;
}

const AutoPtr<IArrayList> BatteryEntry::sRequestQueue = InitsRequestQueue();
AutoPtr<IHandler> BatteryEntry::sHandler;

AutoPtr<BatteryEntry::NameAndIconLoader> BatteryEntry::sRequestThread;

void BatteryEntry::StartRequestQueue()
{
    if (sHandler != NULL) {
        {
            AutoLock syncLock(sRequestQueue);
            Boolean res;
            if (sRequestQueue->IsEmpty(&res), !res) {
                if (sRequestThread != NULL) {
                    sRequestThread->Abort();
                }
                sRequestThread = new NameAndIconLoader();
                sRequestThread->constructor();
                sRequestThread->SetPriority(IThread::MIN_PRIORITY);
                sRequestThread->Start();
               ((Object*)sRequestQueue.Get())->Notify();
            }
        }
    }
}

void BatteryEntry::StopRequestQueue()
{
    {
        AutoLock syncLock(sRequestQueue);
        if (sRequestThread != NULL) {
            sRequestThread->Abort();
            sRequestThread = NULL;
            sHandler = NULL;
        }
    }
}

void BatteryEntry::ClearUidCache()
{
    sUidCache->Clear();
}

BatteryEntry::BatteryEntry(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IUserManager* um,
    /* [in] */ IBatterySipper* sipper)
{
    sHandler = handler;
    mContext = context;
    mSipper = sipper;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    BatterySipperDrainType drainType;
    sipper->GetDrainType(&drainType);
    switch (drainType) {
        case BatterySipperDrainType_IDLE:
            resources->GetString(R::string::power_idle, &mName);
            mIconId = R::drawable::ic_settings_phone_idle;
            break;
        case BatterySipperDrainType_CELL:
            resources->GetString(R::string::power_cell, &mName);
            mIconId = R::drawable::ic_settings_cell_standby;
            break;
        case BatterySipperDrainType_PHONE:
            resources->GetString(R::string::power_phone, &mName);
            mIconId = R::drawable::ic_settings_voice_calls;
            break;
        case BatterySipperDrainType_WIFI:
            resources->GetString(R::string::power_wifi, &mName);
            mIconId = R::drawable::ic_settings_wifi;
            break;
        case BatterySipperDrainType_BLUETOOTH:
            resources->GetString(R::string::power_bluetooth, &mName);
            mIconId = R::drawable::ic_settings_bluetooth;
            break;
        case BatterySipperDrainType_SCREEN:
            resources->GetString(R::string::power_screen, &mName);
            mIconId = R::drawable::ic_settings_display;
            break;
        case BatterySipperDrainType_FLASHLIGHT:
            resources->GetString(R::string::power_flashlight, &mName);
            mIconId = R::drawable::ic_settings_display;
            break;
        case BatterySipperDrainType_APP:
            sipper->GetPackageWithHighestDrain(&mName);
            break;
        case BatterySipperDrainType_USER: {
            Int32 userId;
            sipper->GetUserId(&userId);
            AutoPtr<IUserInfo> info;
            um->GetUserInfo(userId, (IUserInfo**)&info);
            if (info != NULL) {
                mIcon = Utils::GetUserIcon(context, um, info);
                String name;
                mName = info != NULL ? (info->GetName(&name), name) : String(NULL);
                if (mName == NULL) {
                    Int32 id;
                    info->GetId(&id);
                    mName = StringUtils::ToString(id);
                }
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(mName));
                resources->GetString(R::string::running_process_item_user_label, args, &mName);
            }
            else {
                mIcon = NULL;
                resources->GetString(R::string::running_process_item_removed_user_label, &mName);
            }
        } break;
        case BatterySipperDrainType_UNACCOUNTED:
            resources->GetString(R::string::power_unaccounted, &mName);
            mIconId = R::drawable::ic_power_system;
            break;
        case BatterySipperDrainType_OVERCOUNTED:
            resources->GetString(R::string::power_overcounted, &mName);
            mIconId = R::drawable::ic_power_system;
            break;
    }
    if (mIconId > 0) {
        mIcon = NULL;
        resources->GetDrawable(mIconId, (IDrawable**)&mIcon);
    }
    AutoPtr<IBatteryStatsUid> uidObj;
    mSipper->GetUidObj((IBatteryStatsUid**)&uidObj);
    if ((mName == NULL || mIconId == 0) && uidObj != NULL) {
        GetQuickNameIconForUid(uidObj);
    }
}

AutoPtr<IDrawable> BatteryEntry::GetIcon()
{
    return mIcon;
}

String BatteryEntry::GetLabel()
{
    return mName;
}

void BatteryEntry::GetQuickNameIconForUid(
    /* [in] */ IBatteryStatsUid* uidObj)
{
    Int32 uid;
    uidObj->GetUid(&uid);
    String uidString = StringUtils::ToString(uid);
    Boolean res;
    if (sUidCache->ContainsKey(CoreUtils::Convert(uidString), &res), res) {
        AutoPtr<IInterface> obj;
        sUidCache->Get(CoreUtils::Convert(uidString), (IInterface**)&obj);
        UidToDetail* utd = (UidToDetail*)IObject::Probe(obj);
        mDefaultPackageName = utd->mPackageName;
        mName = utd->mName;
        mIcon = utd->mIcon;
        return;
    }

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr< ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    mIcon = NULL;
    pm->GetDefaultActivityIcon((IDrawable**)&mIcon);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    if (packages == NULL) {
        //name = Integer->ToString(uid);
        if (uid == 0) {
            resources->GetString(R::string::process_kernel_label, &mName);
        }
        else if (mName.Equals("mediaserver")) {
            resources->GetString(R::string::process_mediaserver_label, &mName);
        }
        mIconId = R::drawable::ic_power_system;
        resources->GetDrawable(mIconId, (IDrawable**)&mIcon);
        return;
    }
    else {
        //name = packages[0];
    }
    if (sHandler != NULL) {
        {
            AutoLock syncLock(sRequestQueue);
            sRequestQueue->Add((IBatteryEntry*)this);
        }
    }
}

ECode BatteryEntry::LoadNameAndIcon()
{
    // Bail out if the current sipper is not an App sipper.
    AutoPtr<IBatteryStatsUid> uidObj;
    mSipper->GetUidObj((IBatteryStatsUid**)&uidObj);
    if (uidObj == NULL) {
        return NOERROR;
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 uid;
    uidObj->GetUid(&uid);
    AutoPtr<IDrawable> defaultActivityIcon;
    pm->GetDefaultActivityIcon((IDrawable**)&defaultActivityIcon);
    AutoPtr< ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    mSipper->SetPackages(packages);
    if (packages == NULL) {
        mName = StringUtils::ToString(uid);
        return NOERROR;
    }

    Int32 length = packages->GetLength();
    AutoPtr< ArrayOf<String> > packageLabels = ArrayOf<String>::Alloc(length);
    packageLabels->Copy(0, packages, 0, length);

    // Convert package names to user-facing labels where possible
    AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
    Int32 userId = UserHandle::GetUserId(uid);
    for (Int32 i = 0; i < packageLabels->GetLength(); i++) {
        // try {
            AutoPtr<IApplicationInfo> ai;
            ipm->GetApplicationInfo((*packageLabels)[i], 0 /* no flags */, userId, (IApplicationInfo**)&ai);
            if (ai == NULL) {
                Logger::D("CPowerUsageSummary", "Retrieving NULL app info for package %s, user %d",
                        (*packageLabels)[i].string(), userId);
                continue;
            }
            IPackageItemInfo* _ai = IPackageItemInfo::Probe(ai);
            AutoPtr<ICharSequence> label;
            _ai->LoadLabel(pm, (ICharSequence**)&label);
            if (label != NULL) {
                label->ToString(&((*packageLabels)[i]));
            }
            Int32 icon;
            _ai->GetIcon(&icon);
            if (icon != 0) {
                mDefaultPackageName = (*packages)[i];
                mIcon = NULL;
                _ai->LoadIcon(pm, (IDrawable**)&mIcon);
                break;
            }
        // } catch (RemoteException e) {
        //     Logger::D(PowerUsageSummary.TAG, "Error while retrieving app info for package "
        //             + packageLabels[i] + ", user " + userId, e);
        // }
    }
    if (mIcon == NULL) {
        mIcon = defaultActivityIcon;
    }

    if (packageLabels->GetLength() == 1) {
        mName = (*packageLabels)[0];
    }
    else {
        // Look for an official name for this UID.
        for (Int32 i = 0; i < packages->GetLength(); i++) {
            String pkgName = (*packages)[i];
            // try {
                AutoPtr<IPackageInfo> pi;
                ipm->GetPackageInfo(pkgName, 0 /* no flags */, userId, (IPackageInfo**)&pi);
                if (pi == NULL) {
                    Logger::D("CPowerUsageSummary", "Retrieving NULL package info for package %s, user %d",
                            pkgName.string(), userId);
                    continue;
                }
                Int32 sharedUserLabel;
                pi->GetSharedUserLabel(&sharedUserLabel);
                if (sharedUserLabel != 0) {
                    AutoPtr<IApplicationInfo> applicationInfo;
                    pi->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
                    AutoPtr<ICharSequence> nm;
                    pm->GetText(pkgName, sharedUserLabel, applicationInfo, (ICharSequence**)&nm);
                    if (nm != NULL) {
                        nm->ToString(&mName);
                        Int32 icon;
                        IPackageItemInfo::Probe(applicationInfo)->GetIcon(&icon);
                        if (icon != 0) {
                            mDefaultPackageName = pkgName;
                            mIcon = NULL;
                            IPackageItemInfo::Probe(applicationInfo)->LoadIcon(pm, (IDrawable**)&mIcon);
                        }
                        break;
                    }
                }
            // } catch (RemoteException e) {
            //     Logger::D(PowerUsageSummary.TAG, "Error while retrieving package info for package "
            //             + pkgName + ", user " + userId, e);
            // }
        }
    }
    String uidString = StringUtils::ToString(uid);
    AutoPtr<UidToDetail> utd = new UidToDetail();
    utd->mName = mName;
    utd->mIcon = mIcon;
    utd->mPackageName = mDefaultPackageName;
    sUidCache->Put(CoreUtils::Convert(uidString), (IObject*)utd);
    if (sHandler != NULL) {
        AutoPtr<IMessage> message;
        sHandler->ObtainMessage(MSG_UPDATE_NAME_ICON, (IBatteryEntry*)this, (IMessage**)&message);
        Boolean res;
        sHandler->SendMessage(message, &res);
    }
    return NOERROR;
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos