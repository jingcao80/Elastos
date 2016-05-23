
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/preference/CPreferenceInflater.h"
#include "elastos/droid/preference/CPreferenceManager.h"
#include "elastos/droid/preference/CPreferenceScreen.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::CPreferenceInflater;
using Elastos::Droid::Preference::CPreferenceManager;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

const String PreferenceManager::TAG("PreferenceManager");

CAR_INTERFACE_IMPL(PreferenceManager, Object, IPreferenceManager)

PreferenceManager::PreferenceManager()
    : mNextId(0)
    , mNextRequestCode(0)
    , mNoCommit(FALSE)
    , mSharedPreferencesMode(0)
{}

ECode PreferenceManager::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 firstRequestCode)
{
    mActivity = activity;
    mNextRequestCode = firstRequestCode;

    return constructor(IContext::Probe(activity));
}

ECode PreferenceManager::constructor(
    /* [in] */ IContext* context)
{
    return Init(context);
}

ECode PreferenceManager::Init(
    /* [in] */ IContext* context)
{
    mContext = context;
    return SetSharedPreferencesName(GetDefaultSharedPreferencesName(context));
}

ECode PreferenceManager::SetFragment(
    /* [in] */ IPreferenceFragment* fragment)
{
    mFragment = fragment;
    return NOERROR;
}

ECode PreferenceManager::GetFragment(
    /* [out] */ IPreferenceFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment)
    *fragment = mFragment;
    REFCOUNT_ADD(*fragment)
    return NOERROR;
}

AutoPtr<IList> PreferenceManager::QueryIntentActivities(
    /* [in] */ IIntent* queryIntent)
{
    AutoPtr<IPackageManager> manager;
    mContext->GetPackageManager((IPackageManager**)&manager);
    AutoPtr<IList> resolves;
    manager->QueryIntentActivities(queryIntent, IPackageManager::GET_META_DATA, (IList**)&resolves);
    return resolves;
}

ECode PreferenceManager::InflateFromIntent(
    /* [in] */ IIntent* queryIntent,
    /* [in] */ IPreferenceScreen* rootPreferences,
    /* [out] */ IPreferenceScreen** screen)
{
    VALIDATE_NOT_NULL(screen)
    const AutoPtr<IList> activities = QueryIntentActivities(queryIntent);
    const AutoPtr<IHashSet> inflatedRes;
    CHashSet::New((IHashSet**)&inflatedRes);

    Int32 size;
    activities->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> _resolveInfo;
        activities->Get(i, (IInterface**)&_resolveInfo);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(_resolveInfo);
        const AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);

        const AutoPtr<IBundle> metaData;
        IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metaData);

        Boolean contains;
        if ((metaData == NULL) || (metaData->ContainsKey(METADATA_KEY_PREFERENCES, &contains), !contains)) {
            continue;
        }

        //Need to concat the package with res ID since the same res ID
        //can be re-used across contexts
        String packageName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        Int32 keyPreferences;
        metaData->GetInt32(METADATA_KEY_PREFERENCES, &keyPreferences);
        StringBuilder sb(packageName);
        sb += ":";
        sb += keyPreferences;
        const String uniqueResId = sb.ToString();

        AutoPtr<IString> strObj;
        CString::New(uniqueResId, (IString**)&strObj);
        Boolean result;
        inflatedRes->Contains(strObj, &result);
        if (!result) {
            inflatedRes->Add(strObj);

            const AutoPtr<IContext> context;
            ECode ec = mContext->CreatePackageContext(packageName, 0, (IContext**)&context);
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                Slogger::W(TAG, "Could not create context for %s: 0x%08x", packageName.string(), ec);
                continue;
            }

            AutoPtr<IPreferenceInflater> inflater;
            CPreferenceInflater::New(context, this, (IPreferenceInflater**)&inflater);
            AutoPtr<IPackageManager> pm;
            context->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IXmlResourceParser> parser;
            IPackageItemInfo::Probe(activityInfo)->LoadXmlMetaData(pm, METADATA_KEY_PREFERENCES, (IXmlResourceParser**)&parser);
            AutoPtr<IInterface> temp;
            IGenericInflater::Probe(inflater)->Inflate(IXmlPullParser::Probe(parser), rootPreferences, TRUE, (IInterface**)&temp);
            rootPreferences = IPreferenceScreen::Probe(temp);
            parser->Close();
        }
    }
    IPreference::Probe(rootPreferences)->OnAttachedToHierarchy(this);

    *screen = IPreferenceScreen::Probe(rootPreferences);
    REFCOUNT_ADD(*screen)
    return NOERROR;
}

ECode PreferenceManager::InflateFromResource(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ IPreferenceScreen* rootPreferences,
    /* [out] */ IPreferenceScreen** screen)
{
    VALIDATE_NOT_NULL(screen)
    // Block commits
    SetNoCommit(TRUE);

    AutoPtr<IGenericInflater> inflater;
    CPreferenceInflater::New(context, this, (IGenericInflater**)&inflater);
    AutoPtr<IInterface> pfObj;
    inflater->Inflate(resId, rootPreferences, TRUE, (IInterface**)&pfObj);
    AutoPtr<IPreference> pf = IPreference::Probe(pfObj);
    if (pf != NULL) pf->OnAttachedToHierarchy(this);

    // Unblock commits
    SetNoCommit(FALSE);
    *screen = IPreferenceScreen::Probe(pf);
    REFCOUNT_ADD(*screen)
    return NOERROR;
}

ECode PreferenceManager::CreatePreferenceScreen(
    /* [in] */ IContext* context,
    /* [out] */ IPreferenceScreen** screen)
{
    VALIDATE_NOT_NULL(screen)
    AutoPtr<IPreferenceScreen> preferenceScreen;
    CPreferenceScreen::New(context, NULL, (IPreferenceScreen**)&preferenceScreen);
    AutoPtr<IPreference> preference = IPreference::Probe(preferenceScreen);
    preference->OnAttachedToHierarchy(this);
    *screen = preferenceScreen.Get();
    REFCOUNT_ADD(*screen)
    return NOERROR;
}

ECode PreferenceManager::GetNextId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    {    AutoLock syncLock(this);
        *id = mNextId++;
    }
    return NOERROR;
}

ECode PreferenceManager::GetSharedPreferencesName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mSharedPreferencesName;
    return NOERROR;
}

ECode PreferenceManager::SetSharedPreferencesName(
    /* [in] */ const String& sharedPreferencesName)
{
    mSharedPreferencesName = sharedPreferencesName;
    mSharedPreferences = NULL;
    return NOERROR;
}

ECode PreferenceManager::GetSharedPreferencesMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mSharedPreferencesMode;
    return NOERROR;
}

ECode PreferenceManager::SetSharedPreferencesMode(
    /* [in] */ Int32 sharedPreferencesMode)
{
    mSharedPreferencesMode = sharedPreferencesMode;
    mSharedPreferences = NULL;
    return NOERROR;
}

ECode PreferenceManager::GetSharedPreferences(
    /* [out] */ ISharedPreferences** sp)
{
    VALIDATE_NOT_NULL(sp)

    if (mSharedPreferences == NULL) {
        mContext->GetSharedPreferences(mSharedPreferencesName,
                mSharedPreferencesMode, (ISharedPreferences**)&mSharedPreferences);
    }

    *sp = mSharedPreferences;
    REFCOUNT_ADD(*sp)
    return NOERROR;
}

ECode PreferenceManager::GetDefaultSharedPreferences(
    /* [in] */ IContext* context,
    /* [out] */ ISharedPreferences** sp)
{
    VALIDATE_NOT_NULL(sp)
    return context->GetSharedPreferences(GetDefaultSharedPreferencesName(context),
            GetDefaultSharedPreferencesMode(), sp);
}

String PreferenceManager::GetDefaultSharedPreferencesName(
        /* [in] */ IContext* context)
{
    String packageName;
    context->GetPackageName(&packageName);
    return packageName + String("_preferences");
}

Int32 PreferenceManager::GetDefaultSharedPreferencesMode()
{
    return IContext::MODE_PRIVATE;
}

ECode PreferenceManager::GetPreferenceScreen(
    /* [out] */ IPreferenceScreen** screen)
{
    VALIDATE_NOT_NULL(screen)
    *screen = mPreferenceScreen;
    REFCOUNT_ADD(*screen)
    return NOERROR;
}

ECode PreferenceManager::SetPreferences(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (preferenceScreen != mPreferenceScreen) {
        mPreferenceScreen = preferenceScreen;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PreferenceManager::FindPreference(
    /* [in] */ ICharSequence* key,
    /* [out] */ IPreference** preference)
{
    VALIDATE_NOT_NULL(preference)
    if (mPreferenceScreen == NULL) {
        *preference = NULL;
        return NOERROR;
    }

    return IPreferenceGroup::Probe(mPreferenceScreen)->FindPreference(key, preference);
}

ECode PreferenceManager::SetDefaultValues(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Boolean readAgain)
{
    // Use the default shared preferences name and mode
    return SetDefaultValues(context, GetDefaultSharedPreferencesName(context),
            GetDefaultSharedPreferencesMode(), resId, readAgain);
}

ECode PreferenceManager::SetDefaultValues(
    /* [in] */ IContext* context,
    /* [in] */ const String& sharedPreferencesName,
    /* [in] */ Int32 sharedPreferencesMode,
    /* [in] */ Int32 resId,
    /* [in] */ Boolean readAgain)
{
    AutoPtr<ISharedPreferences> defaultValueSp;
    context->GetSharedPreferences(KEY_HAS_SET_DEFAULT_VALUES, IContext::MODE_PRIVATE, (ISharedPreferences**)&defaultValueSp);

    Boolean value;
    if (readAgain || (defaultValueSp->GetBoolean(KEY_HAS_SET_DEFAULT_VALUES, FALSE, &value), !value)) {
        AutoPtr<IPreferenceManager> pm;
        CPreferenceManager::New(context, (IPreferenceManager**)&pm);
        pm->SetSharedPreferencesName(sharedPreferencesName);
        pm->SetSharedPreferencesMode(sharedPreferencesMode);
        AutoPtr<IPreferenceScreen> screen;
        pm->InflateFromResource(context, resId, NULL, (IPreferenceScreen**)&screen);

        AutoPtr<ISharedPreferencesEditor> editor;
        defaultValueSp->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutBoolean(KEY_HAS_SET_DEFAULT_VALUES, TRUE);
        ECode ec = editor->Apply();
        if (FAILED(ec)) {
            // The app injected its own pre-Gingerbread
            // SharedPreferences.Editor implementation without
            // an apply method.
            Boolean result = FALSE;
            editor->Commit(&result);
            return E_ABSTRACT_METHOD_ERROR;
        }
    }
    return NOERROR;
}

ECode PreferenceManager::GetEditor(
    /* [out] */ ISharedPreferencesEditor** editor)
{
    VALIDATE_NOT_NULL(editor)
    if (mNoCommit) {
        if (mEditor == NULL) {
            AutoPtr<ISharedPreferences> sp;
            GetSharedPreferences((ISharedPreferences**)&sp);
            sp->Edit((ISharedPreferencesEditor**)&mEditor);
        }
        *editor = mEditor;
        REFCOUNT_ADD(*editor)
        return NOERROR;
    }
    else {
        mEditor = NULL;
        AutoPtr<ISharedPreferences> share;
        GetSharedPreferences((ISharedPreferences**)&share);
        share->Edit((ISharedPreferencesEditor**)&mEditor);
        *editor = mEditor;
        REFCOUNT_ADD(*editor)
        return NOERROR;
    }
}

ECode PreferenceManager::ShouldCommit(
    /* [out] */ Boolean* shouldCommit)
{
    VALIDATE_NOT_NULL(shouldCommit)
    *shouldCommit = !mNoCommit;
    return NOERROR;
}

ECode PreferenceManager::SetNoCommit(
    /* [in] */ Boolean noCommit)
{
    if (!noCommit && mEditor != NULL) {
        ECode ec = mEditor->Apply();
        if (FAILED(ec)) {
            // The app injected its own pre-Gingerbread
            // SharedPreferences.Editor implementation without
            // an apply method.
            Boolean result = FALSE;
            mEditor->Commit(&result);
            return E_ABSTRACT_METHOD_ERROR;
        }
    }
    mNoCommit = noCommit;
    return NOERROR;
}

ECode PreferenceManager::GetActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = mActivity;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode PreferenceManager::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode PreferenceManager::RegisterOnActivityResultListener(
    /* [in] */ IPreferenceManagerOnActivityResultListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityResultListeners == NULL) {
            mActivityResultListeners = new List<AutoPtr<IPreferenceManagerOnActivityResultListener> >();
        }

        AutoPtr<IPreferenceManagerOnActivityResultListener> temp = listener;
        List<AutoPtr<IPreferenceManagerOnActivityResultListener> >::Iterator it
                = Find(mActivityResultListeners->Begin(), mActivityResultListeners->End(), temp);
        if (it == mActivityResultListeners->End()) {
            mActivityResultListeners->PushBack(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::UnregisterOnActivityResultListener(
    /* [in] */ IPreferenceManagerOnActivityResultListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityResultListeners != NULL) {
            mActivityResultListeners->Remove(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::DispatchActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    AutoPtr<List< AutoPtr<IPreferenceManagerOnActivityResultListener> > > list;

    {    AutoLock syncLock(this);
        if (mActivityResultListeners == NULL) {
            return NOERROR;
        }
        list = new List< AutoPtr<IPreferenceManagerOnActivityResultListener> >(*mActivityResultListeners);
    }

    List<AutoPtr<IPreferenceManagerOnActivityResultListener> >::Iterator it;
    for (it = list->Begin(); it != list->End(); ++it) {
        Boolean result;
        (*it)->OnActivityResult(requestCode, resultCode, data, &result);
        if (result) {
            break;
        }
    }
    return NOERROR;
}

ECode PreferenceManager::RegisterOnActivityStopListener(
    /* [in] */ IPreferenceManagerOnActivityStopListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityStopListeners == NULL) {
            mActivityStopListeners = new List<AutoPtr<IPreferenceManagerOnActivityStopListener> >();
        }

        AutoPtr<IPreferenceManagerOnActivityStopListener> temp = listener;
        List<AutoPtr<IPreferenceManagerOnActivityStopListener> >::Iterator it
                = Find(mActivityStopListeners->Begin(), mActivityStopListeners->End(), temp);
        if (it == mActivityStopListeners->End()) {
            mActivityStopListeners->PushBack(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::UnregisterOnActivityStopListener(
    /* [in] */ IPreferenceManagerOnActivityStopListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityStopListeners != NULL) {
            mActivityStopListeners->Remove(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::DispatchActivityStop()
{
    AutoPtr<List<AutoPtr<IPreferenceManagerOnActivityStopListener> > > list;

    {    AutoLock syncLock(this);
        if (mActivityStopListeners == NULL) return NOERROR;
        list = new List<AutoPtr<IPreferenceManagerOnActivityStopListener> >(*mActivityStopListeners);
    }

    List<AutoPtr<IPreferenceManagerOnActivityStopListener> >::Iterator it;
    for (it = list->Begin(); it != list->End(); ++it) {
        (*it)->OnActivityStop();
    }
    return NOERROR;
}

ECode PreferenceManager::RegisterOnActivityDestroyListener(
    /* [in] */ IPreferenceManagerOnActivityDestroyListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityDestroyListeners == NULL) {
            mActivityDestroyListeners = new List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> >();
        }

        AutoPtr<IPreferenceManagerOnActivityDestroyListener> temp = listener;
        List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> >::Iterator it
                = Find(mActivityDestroyListeners->Begin(), mActivityDestroyListeners->End(), temp);
        if (it == mActivityDestroyListeners->End()) {
            mActivityDestroyListeners->PushBack(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::UnregisterOnActivityDestroyListener(
    /* [in] */ IPreferenceManagerOnActivityDestroyListener* listener)
{
    {    AutoLock syncLock(this);
        if (mActivityDestroyListeners != NULL) {
            mActivityDestroyListeners->Remove(listener);
        }
    }
    return NOERROR;
}

ECode PreferenceManager::DispatchActivityDestroy()
{
    AutoPtr<List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> > > list;

    {    AutoLock syncLock(this);
        if (mActivityDestroyListeners != NULL) {
            list = new List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> >(*mActivityDestroyListeners);
        }
    }

    if (list != NULL) {
        List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> >::Iterator it;
        for (it = list->Begin(); it != list->End(); ++it) {
            (*it)->OnActivityDestroy();
        }
    }

    // Dismiss any PreferenceScreens still showing
    DismissAllScreens();
    return NOERROR;
}

ECode PreferenceManager::GetNextRequestCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)
    {    AutoLock syncLock(this);
        *code = mNextRequestCode++;
    }
    return NOERROR;
}

ECode PreferenceManager::AddPreferencesScreen(
    /* [in] */ IDialogInterface* screen)
{
    {    AutoLock syncLock(this);
        if (mPreferencesScreens == NULL) {
            mPreferencesScreens = new List<AutoPtr<IDialogInterface> >();
        }

        mPreferencesScreens->PushBack(screen);
    }
    return NOERROR;
}

ECode PreferenceManager::RemovePreferencesScreen(
    /* [in] */ IDialogInterface* screen)
{
    {    AutoLock syncLock(this);
        if (mPreferencesScreens == NULL) {
            return NOERROR;
        }

        mPreferencesScreens->Remove(screen);
    }
    return NOERROR;
}

ECode PreferenceManager::DispatchNewIntent(
    /* [in] */ IIntent* intent)
{
    DismissAllScreens();
    return NOERROR;
}

void PreferenceManager::DismissAllScreens()
{
     // Remove any of the previously shown preferences screens
    AutoPtr<List<AutoPtr<IDialogInterface> > > screensToDismiss;

    {    AutoLock syncLock(this);
        if (mPreferencesScreens == NULL) {
            return;
        }

        screensToDismiss = new List<AutoPtr<IDialogInterface> >(*mPreferencesScreens);
        mPreferencesScreens->Clear();
    }

    List<AutoPtr<IDialogInterface> >::ReverseIterator rit;
    for (rit = screensToDismiss->RBegin(); rit != screensToDismiss->REnd(); ++rit) {
        (*rit)->Dismiss();
    }
}

ECode PreferenceManager::SetOnPreferenceTreeClickListener(
    /* [in] */ IPreferenceManagerOnPreferenceTreeClickListener* listener)
{
    mOnPreferenceTreeClickListener = listener;
    return NOERROR;
}

ECode PreferenceManager::GetOnPreferenceTreeClickListener(
    /* [out] */ IPreferenceManagerOnPreferenceTreeClickListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    *listener = mOnPreferenceTreeClickListener;
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

}
}
}
