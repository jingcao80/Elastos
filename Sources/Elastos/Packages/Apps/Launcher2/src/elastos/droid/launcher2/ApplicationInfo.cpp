
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String ApplicationInfo::TAG("Launcher2.ApplicationInfo");

CAR_INTERFACE_IMPL(ApplicationInfo, ItemInfo, IApplicationInfo);

ApplicationInfo::ApplicationInfo()
    : mFirstInstallTime(0)
    , mFlags(0)
{
}

ECode ApplicationInfo::constructor()
{
    mItemType = ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT;
    return NOERROR;
}

ApplicationInfo::constructor(
    /* [in] */ ILauncherActivityInfo* info,
    /* [in] */ IUserHandle* user,
    /* [in] */ IconCache* iconCache,
    /* [in] */ IHashMap* labelCache)
{
    info->GetComponentName((IComponentName**)&mComponentName);
    mContainer = ItemInfo::NO_ID;
    SetActivity(mComponentName,
            IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

    AutoPtr<Elastos::Droid::Content::Pm::IApplicationInfo> ainpo;
    info->GetApplicationInfo((Elastos::Droid::Content::Pm::IApplicationInfo**)&ainpo);
    Int32 appFlags;
    ainpo->GetFlags(&appFlags);
    if ((appFlags & Elastos::Droid::Content::Pm::IApplicationInfo::FLAG_SYSTEM) == 0) {
        mFlags |= DOWNLOADED_FLAG;
    }
    if ((appFlags & Elastos::Droid::Content::Pm::IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0) {
        mFlags |= UPDATED_SYSTEM_APP_FLAG;
    }
    info->GetFirstInstallTime(&mFirstInstallTime);
    iconCache->GetTitleAndIcon(this, info, labelCache);
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&mIntent);
    mIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<IComponentName> name;
    info->GetComponentName((IComponentName**)&name);
    mIntent->SetComponent(name);
    mIntent->PutExtra(EXTRA_PROFILE, IParcelable::Probe(user));
    UpdateUser(mIntent);
    return NOERROR;
}

ApplicationInfo::constructor(
    /* [in] */ ApplicationInfo* info)
{
    ItemInfo::constructor(info);
    mComponentName = info->mComponentName;
    String tmp;
    info->mTitle->ToString(&tmp);
    mTitle = CoreUtils::Convert(tmp);
    CIntent::New(info->mIntent, (IIntent**)&mIntent);
    mFlags = info->mFlags;
    mFirstInstallTime = info->mFirstInstallTime;
    return NOERROR;
}

ECode ApplicationInfo::SetActivity(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 launchFlags)
{
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&mIntent);
    mIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    mIntent->SetComponent(className);
    mIntent->SetFlags(launchFlags);
    mItemType = ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_APPLICATION;
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
        IObject::Probe(info->mIconBitmap)->ToString(&tmp2);
        Slogger::D(tag, "   title=\"%s\" iconBitmap=%s firstInstallTime=%d",
                tmp.string(), tmp2.string(), info->mFirstInstallTime);
    }
    return;
}

ECode ApplicationInfo::MakeShortcut(
    /* [out] */ IShortcutInfo** info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<ShortcutInfo> _info = new ShortcutInfo();
    _info->constructor(this);
    *info = IShortcutInfo::Probe(_info);
    REFCOUNT_ADD(*info);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos