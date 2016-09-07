#include "elastos/droid/internal/utility/cm/CLockscreenShortcutsHelper.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include "elastos/droid/graphics/CColorMatrixColorFilter.h"
#include "elastos/droid/provider/Settings.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

//==============================================================================
//  CLockscreenShortcutsHelper::Shortcuts
//==============================================================================

CAR_INTERFACE_IMPL(CLockscreenShortcutsHelper::Shortcuts,
        Object, ILockscreenShortcutsHelperShortcuts)

AutoPtr<CLockscreenShortcutsHelper::Shortcuts> CLockscreenShortcutsHelper::Shortcuts::LEFT_SHORTCUT;
AutoPtr<CLockscreenShortcutsHelper::Shortcuts> CLockscreenShortcutsHelper::Shortcuts::RIGHT_SHORTCUT;

Boolean CLockscreenShortcutsHelper::Shortcuts::sInit = CLockscreenShortcutsHelper::Shortcuts::InitStatic();

CLockscreenShortcutsHelper::Shortcuts::Shortcuts(
    /* [in] */ Int32 index)
    : mIndex(index)
{
}

ECode CLockscreenShortcutsHelper::Shortcuts::GetIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex;
    return NOERROR;
}

Boolean CLockscreenShortcutsHelper::Shortcuts::InitStatic()
{
    LEFT_SHORTCUT = new CLockscreenShortcutsHelper::Shortcuts(0);
    RIGHT_SHORTCUT = new CLockscreenShortcutsHelper::Shortcuts(1);
    return TRUE;
}

//==============================================================================
//  CLockscreenShortcutsHelper::TargetInfo
//==============================================================================

CLockscreenShortcutsHelper::TargetInfo::TargetInfo(
    /* [in] */ IDrawable* icon,
    /* [in] */ IColorFilter* colorFilter,
    /* [in] */ const String& uri)
    : mIcon(icon)
    , mColorFilter(colorFilter)
    , mUri(uri)
{
}

//==============================================================================
//  CLockscreenShortcutsHelper::MyContentObserver
//==============================================================================

ECode CLockscreenShortcutsHelper::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->FetchTargets();
    if (mHost->mListener != NULL) {
        mHost->mListener->OnChange();
    }
    return NOERROR;
}

//==============================================================================
//  CLockscreenShortcutsHelper
//==============================================================================

CAR_INTERFACE_IMPL(CLockscreenShortcutsHelper, Object, ILockscreenShortcutsHelper)

CAR_OBJECT_IMPL(CLockscreenShortcutsHelper)

const String CLockscreenShortcutsHelper::DELIMITER("|");
const String CLockscreenShortcutsHelper::SYSTEM_UI_PKGNAME("Elastos.Droid.SystemUI");
const String CLockscreenShortcutsHelper::PHONE_DEFAULT_ICON("ic_phone_24dp");
const String CLockscreenShortcutsHelper::CAMERA_DEFAULT_ICON("ic_camera_alt_24dp");

CLockscreenShortcutsHelper::CLockscreenShortcutsHelper()
{
}

ECode CLockscreenShortcutsHelper::constructor()
{
    mObserver = new MyContentObserver(this);
    mObserver->constructor();
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILockscreenShortcutsHelperOnChangeListener* listener)
{
    mObserver = new MyContentObserver(this);
    mObserver->constructor();

    mContext = context;
    if (listener != NULL) {
        mListener = listener;
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IUri> uri;
        Settings::System::GetUriFor(ISettingsSecure::LOCKSCREEN_TARGETS, (IUri**)&uri);
        cr->RegisterContentObserver(uri, FALSE, mObserver);
    }
    FetchTargets();
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::GetDrawablesForTargets(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    FetchTargets();
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);

    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetSaturation(0);
    AutoPtr<IColorMatrixColorFilter> filter;
    CColorMatrixColorFilter::New(cm, (IColorMatrixColorFilter**)&filter);

    for (Int32 i = 0; i < 2/*Shortcuts.values().length*/; i++) {
        AutoPtr<IInterface> obj;
        mTargetActivities->Get(i, (IInterface**)&obj);
        String activity;
        ICharSequence::Probe(obj)->ToString(&activity);
        AutoPtr<IDrawable> drawable;
        AutoPtr<IColorFilter> filerToSet;

        if (!activity.IsEmpty() && !activity.Equals(NONE)) {
            // No pre-defined action, try to resolve URI
            // try {
            AutoPtr<IIntent> intent;
            Intent::ParseUri(activity, 0, (IIntent**)&intent);
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IActivityInfo> info;
            intent->ResolveActivityInfo(pm, IPackageManager::GET_ACTIVITIES,
                    (IActivityInfo**)&info);

            if (info != NULL) {
                IPackageItemInfo::Probe(info)->LoadIcon(pm, (IDrawable**)&drawable);
                filerToSet = IColorFilter::Probe(filter);
            }
            // } catch (URISyntaxException e) {
            //     e.printStackTrace();
            //     // Treat as empty
            // }
        }

        if (drawable == NULL) {
            Int32 index;
            CLockscreenShortcutsHelper::Shortcuts::LEFT_SHORTCUT->GetIndex(&index);
            String name = i == index ? PHONE_DEFAULT_ICON : CAMERA_DEFAULT_ICON;
            GetDrawableFromSystemUI(name, (IDrawable**)&drawable);
            filerToSet = NULL;
        }

        AutoPtr<TargetInfo> ti = new TargetInfo(drawable, filerToSet, activity);
        list->Add((IInterface*)(IObject*)ti);
    }

    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::GetDrawableFromSystemUI(
    /* [in] */ const String& name,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IResources> res;
    String pkgName;
    mContext->GetPackageName(&pkgName);
    if (pkgName.Equals(SYSTEM_UI_PKGNAME)) {
        mContext->GetResources((IResources**)&res);
    }
    else {
        if (mSystemUiResources == NULL) {
            // try {
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            pm->GetResourcesForApplication(SYSTEM_UI_PKGNAME, (IResources**)&mSystemUiResources);
            // } catch (PackageManager.NameNotFoundException e) {
            //     // ignore
            // }
        }
        res = mSystemUiResources;
    }
    if (res == NULL) {
        *result = NULL;
    }
    Int32 id;
    res->GetIdentifier(name, String("drawable"), SYSTEM_UI_PKGNAME, &id);
    if (id > 0) {
        res->GetDrawable(id, result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::GetFriendlyNameForUri(
    /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> intent;
    GetIntent(shortcut, (IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_MAIN)) {
        *result = GetFriendlyActivityName(intent, FALSE);
        return NOERROR;
    }
    *result = GetFriendlyShortcutName(intent);
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::IsTargetCustom(
    /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if (mTargetActivities == NULL || (mTargetActivities->IsEmpty(&b), b)) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 index;
    shortcut->GetIndex(&index);
    AutoPtr<IInterface> obj;
    mTargetActivities->Get(index, (IInterface**)&obj);
    String action;
    ICharSequence::Probe(obj)->ToString(&action);
    if (action.Equals(DEFAULT)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    GetIntent(shortcut, (IIntent**)&intent);
    *result = action.Equals(NONE) || intent != NULL;
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::IsTargetEmpty(
    /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 index;
    shortcut->GetIndex(&index);
    if (mTargetActivities != NULL) {
        Boolean b;
        mTargetActivities->IsEmpty(&b);
        if (!b) {
            AutoPtr<IInterface> obj;
            mTargetActivities->Get(index, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            *result = str.Equals(NONE);
        }
    }
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::GetIntent(
    /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> intent;
    // try {
    Int32 index;
    shortcut->GetIndex(&index);
    AutoPtr<IInterface> obj;
    mTargetActivities->Get(index, (IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    Intent::ParseUri(str, 0, (IIntent**)&intent);
    // } catch (URISyntaxException e) {
    //     e.printStackTrace();
    // }
    *result = intent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CLockscreenShortcutsHelper::SaveTargets(
    /* [in] */ IArrayList* targets)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
// TODO: Need Settings::Secure::PutListAsDelimitedString
    // return Settings::Secure::PutListAsDelimitedString(cr,
    //         ISettingsSecure::LOCKSCREEN_TARGETS, DELIMITER, targets);
    return NOERROR;
}

void CLockscreenShortcutsHelper::FetchTargets()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
// TODO: Need Settings::Secure::GetDelimitedStringAsList
    // Settings::Secure::GetDelimitedStringAsList(cr,
    //         ISettingsSecure::LOCKSCREEN_TARGETS, DELIMITER, (IList**)&mTargetActivities);

    Int32 size;
    mTargetActivities->GetSize(&size);
    Int32 itemsToPad = 2/*Shortcuts.values().length*/ - size;
    if (itemsToPad > 0) {
        for (Int32 i = 0; i < itemsToPad; i++) {
            AutoPtr<ICharSequence> cs;
            CString::New(DEFAULT, (ICharSequence**)&cs);
            mTargetActivities->Add(cs);
        }
    }
}

String CLockscreenShortcutsHelper::GetFriendlyActivityName(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean labelOnly)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IActivityInfo> ai;
    intent->ResolveActivityInfo(pm, IPackageManager::GET_ACTIVITIES, (IActivityInfo**)&ai);
    String friendlyName;
    if (ai != NULL) {
        AutoPtr<ICharSequence> cs;
        IPackageItemInfo::Probe(ai)->LoadLabel(pm, (ICharSequence**)&cs);
        cs->ToString(&friendlyName);
        if (friendlyName.IsNull() && !labelOnly) {
            IPackageItemInfo::Probe(ai)->GetName(&friendlyName);
        }
    }
    if (!friendlyName.IsNull() || labelOnly) {
        return friendlyName;
    }
    else {
        String str;
        intent->ToUri(0, &str);
        return str;
    }
}

String CLockscreenShortcutsHelper::GetFriendlyShortcutName(
    /* [in] */ IIntent* intent)
{
    String activityName = GetFriendlyActivityName(intent, TRUE);
    String name;
    intent->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &name);

    if (activityName != NULL && name != NULL) {
        return activityName + ": " + name;
    }
    if (name != NULL) {
        return name;
    }
    else {
        String str;
        intent->ToUri(0, &str);
        return str;
    }
}

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
