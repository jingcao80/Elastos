
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

ShortcutInfo::ShortcutInfo()
    : mCustomIcon(FALSE)
    , mUsingFallbackIcon(FALSE)
{
    mItemType = LauncherSettings::BaseLauncherColumns::ITEM_TYPE_SHORTCUT;
}

ShortcutInfo::ShortcutInfo(
    /* [in] */ ShortcutInfo* info)
    : ItemInfo(info)
    , mUsingFallbackIcon(FALSE)
{
    String _title;
    info->mTitle->ToString(&_title);
    mTitle = CoreUtils::Convert(_title);
    CIntent::New(info->mIntent, (IIntent**)&mIntent);
    if (info->mIconResource != NULL) {
        CIntentShortcutIconResource::New((IIntentShortcutIconResource**)&mIconResource);

        String pname;
        info->mIconResource->GetPackageName(&pname);
        mIconResource->SetPackageName(pname);
        String resourceName;
        info->mIconResource->GetResourceName(&resourceName);
        mIconResource->SetResourceName(resourceName);
    }
    mIcon = info.mIcon; // TODO: should make a copy here.  maybe we don't need this ctor at all
    mCustomIcon = info->mCustomIcon;
}

ShortcutInfo::ShortcutInfo(
    /* [in] */ IApplicationInfo* info)
    : ItemInfo(info)
    , mUsingFallbackIcon(FALSE)
{
    String _title;
    info->mTitle->ToString(&_title);
    mTitle = CoreUtils::Convert(_title);
    CIntent::New(info->mIntent, (IIntent**)&mIntent);
    mCustomIcon = FALSE;
}

void ShortcutInfo::SetIcon(
    /* [in] */ IBitmap* b)
{
    mIcon = b;
    return;
}

AutoPtr<IBitmap> ShortcutInfo::GetIcon(
    /* [in] */ IconCache* iconCache)
{
    if (mIcon == NULL) {
        UpdateIcon(iconCache);
    }
    return mIcon;
}

void ShortcutInfo::UpdateIcon(
    /* [in] */ IconCache* iconCache)
{
    mIcon = iconCache->GetIcon(mIntent, mUser);
    mUsingFallbackIcon = iconCache->IsDefaultIcon(mIcon);
    return;
}

void ShortcutInfo::SetActivity(
    /* [in] */ IIntent* intent)
{
    Int32 launchFlags = IIntent::FLAG_ACTIVITY_NEW_TASK |
            IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED;
    CIntent::New((IIntent**)&mIntent);
    mIntent->SetFlags(launchFlags);
    mIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    mIntent->SetComponent(componentName);
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    mIntent->PutExtras(extras);
    mItemType = LauncherSettings::BaseLauncherColumns::ITEM_TYPE_APPLICATION;
    UpdateUser(mIntent);
    return;
}

void ShortcutInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    ItemInfo::OnAddToDatabase(context, values);
    String titleStr;
    if (mTitle != NULL) {
        title->ToString(&titleStr);
    }
    else {
        titleStr = NULL;
    }
    values->Put(LauncherSettings::BaseLauncherColumns:TITLE, titleStr);

    String uri;
    if (mIntent != NULL) {
        mIntent->ToUri(0, &uri);
    }
    else {
        uri = NULL;
    }
    values->Put(LauncherSettings::BaseLauncherColumns::INTENT, uri);

    if (mCustomIcon) {
        values->Put(LauncherSettings::BaseLauncherColumns::ICON_TYPE,
                LauncherSettings::BaseLauncherColumns::ICON_TYPE_BITMAP);
        WriteBitmap(values, mIcon);
    }
    else {
        if (!mUsingFallbackIcon) {
            WriteBitmap(values, mIcon);
        }
        values->Put(LauncherSettings::BaseLauncherColumns::ICON_TYPE,
                LauncherSettings::BaseLauncherColumns::ICON_TYPE_RESOURCE);
        if (mIconResource != NULL) {
            String pname;
            mIconResource->GetPackageName(&pname);
            values->Put(LauncherSettings::BaseLauncherColumns::ICON_PACKAGE,
                    pname);
            String resourceName;
            mIconResource->GetResourceName(&resourceName);
            values->Put(LauncherSettings::BaseLauncherColumns::ICON_RESOURCE,
                    resourceName);
        }
    }
    return;
}

ECode ShortcutInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb +="ShortcutInfo(title=";
    String title;
    mTitle->ToString(&title);
    sb += title;
    sb += "intent=";
    String str;
    mIntent->ToString(&str);
    sb += str;
    sb += "id=";
    sb += mId;
    sb += " type=";
    sb += mItemType;
    sb += " container=";
    sb += mContainer;
    sb += " screen=";
    sb += mScreen;
    sb += " cellX=";
    sb += mCellX;
    sb += " cellY=";
    sb += mCellY;
    sb += " spanX=";
    sb += mSpanX;
    sb += " spanY="
    sb += mSpanY;
    sb += " dropPos=";
    sb += mDropPos;
    sb += ")";

    return sb.ToString(str);
}

void ShortcutInfo::DumpShortcutInfoList(
    /* [in] */ const String& tag,
    /* [in] */ const String& label,
    /* [in] */ IArrayList* list)
{
    StringBuilder sb;
    sb += label;
    sb += " size=";
    Int32 size;
    list->GetSize(&size);
    sb += size;
    Slogger::D(tag, sb.ToString());

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        ShortcutInfo* info = (ShortcutInfo*)IObject::Probe(obj);

        StringBuilder sb;
        sb += "   title=\"";
        sb += info->mTitle;
        sb += " icon=";
        sb += info->mIcon;
        sb += " customIcon=";
        sb += info->mCustomIcon;
        Slogger::D(tag, sb.ToString());
    }
    return;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos