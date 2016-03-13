
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

static const Int32 ApplicationInfo::DOWNLOADED_FLAG = 1;
static const Int32 ApplicationInfo::UPDATED_SYSTEM_APP_FLAG = 2;

static const String ApplicationInfo::TAG("Launcher2.ApplicationInfo");

ApplicationInfo::ApplicationInfo()
    : mFirstInstallTime(0)
    , mFlags(0)
{
    mItemType = LauncherSettings::BaseLauncherColumns::ITEM_TYPE_SHORTCUT;
}

ApplicationInfo::ApplicationInfo(
    /* [in] */ ILauncherActivityInfo* info,
    /* [in] */ IUserHandle* user,
    /* [in] */ IconCache* iconCache,
    /* [in] */ IHashMap* labelCache)
    : mFirstInstallTime(0)
    , mFlags(0)
{
    info->GetComponentName((IComponentName**)&mComponentName);
    mContainer = ItemInfo::NO_ID;
    SetActivity(componentName,
            IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

    AutoPtr<IApplicationInfo> ainpo;
    GetApplicationInfo((IApplicationInfo**)&ainpo);
    Int32 appFlags = ainpo.flags;
    ainpo->GetFlags(&appFlags);
    if ((appFlags & IApplicationInfo::FLAG_SYSTEM) == 0) {
        flags |= DOWNLOADED_FLAG;
    }
    if ((appFlags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0) {
        flags |= UPDATED_SYSTEM_APP_FLAG;
    }
    mFirstInstallTime;
    info->GetFirstInstallTime(&mFirstInstallTime);
    iconCache->GetTitleAndIcon(this, info, labelCache);
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&mIntent);
    mIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<IComponentName> name;
    info->GetComponentName((IComponentName**)&name);
    mIntent->SetComponent(name);
    mIntent->PutExtra(EXTRA_PROFILE, user);
    UpdateUser(mIntent);
}

ApplicationInfo::ApplicationInfo(
    /* [in] */ ApplicationInfo* info)
    : mFirstInstallTime(0)
    , mFlags(0)
    , ItemInfo(info);
{
    AutoPtr<IComponentName> name;
    info->GetComponentName((IComponentName**)&name);
    mComponentName = name;
    info->mTitle->ToString(&mTitle);
    CIntent::New(info->mIntent, (Intent**)&mIntent);
    mFlags = info->mFlags;
    mFirstInstallTime = info->mFirstInstallTime;
}

ECode ApplicationInfo::SetActivity(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 launchFlags)
{
    CIntent::New(IIntent::ACTION_MAIN, (Intent**)&mIntent);
    mIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    mIntent->SetComponent(className);
    mIntent->SetFlags(launchFlags);
    mItemType = LauncherSettings::BaseLauncherColumns::ITEM_TYPE_APPLICATION;
    return NOERROR;
}

ECode ApplicationInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String tmp;
    mTitle->ToString(&tmp);
    String tmp2;
    mUser->ToString(&tmp2);

    StringBuilder sb;
    sb += "ApplicationInfo(title=";
    sb += tmp;
    sb += " P=";
    sb += tmp2;
    sb += ")";
    return sb.ToString(str);
}

void ApplicationInfo::DumpApplicationInfoList(
    /* [in] */ const String& tag,
    /* [in] */ const String& label,
    /* [in] */ IArrayList* list)
{
    Int32 size;
    list->GetSize(&size);
    Slogger::D(tag, "%s size= %d", label.string(), size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IObject::Probe(obj);

        String tmp;
        info->mTitle->ToString(&tmp);
        String tmp2;
        info->mIconBitmap->ToString(&tmp2);
        Slogger::D(tag, "   title=\"%s\" iconBitmap=%s firstInstallTime=%d",
                tmp.string(), tmp2.string(), info->mFirstInstallTime);
    }
    return;
}

AutoPtr<ShortcutInfo> ApplicationInfo::MakeShortcut()
{
    AutoPtr<ShortcutInfo> info = new ShortcutInfo(this);
    return info;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos