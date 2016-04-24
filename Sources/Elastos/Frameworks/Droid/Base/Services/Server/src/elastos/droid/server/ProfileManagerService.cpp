#include "elastos/droid/server/ProfileManagerService.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::ActivityManagerNative;
// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::App::CNotificationGroup;
using Elastos::Droid::App::CNotificationGroupHelper;
using Elastos::Droid::App::CProfileHelper;
using Elastos::Droid::App::CProfileGroup;
using Elastos::Droid::App::EIID_IIProfileManager;
using Elastos::Droid::App::INotificationGroupHelper;
using Elastos::Droid::App::IProfileHelper;
using Elastos::Droid::App::IProfileGroup;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::CFileWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileReader;
using Elastos::IO::IFileWriter;
using Elastos::IO::IReader;
using Elastos::IO::IWriter;
using Elastos::Core::AutoLock;
using Elastos::Core::IStringBuilder;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::IXmlPullParserFactory;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;

namespace Elastos {
namespace Droid {
namespace Server {

const String ProfileManagerService::TAG("ProfileService");

Boolean ProfileManagerService::LOCAL_LOGV = FALSE;

const String ProfileManagerService::PERMISSION_CHANGE_SETTINGS("android.permission.WRITE_SETTINGS");

static AutoPtr<IFile> initPROFILE_FILE()
{
    AutoPtr<IFile> _file = Environment::GetSystemSecureDirectory();
    AutoPtr<IFile> file;
    CFile::New(_file.Get(), String("profiles.xml"), (IFile**)&file);
    return file;
}

const AutoPtr<IFile> ProfileManagerService::PROFILE_FILE = initPROFILE_FILE();

static AutoPtr<IUUID> initWildcardUUID()
{
    AutoPtr<IUUIDHelper> uuidHelper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
    AutoPtr<IUUID> uuid;
    uuidHelper->FromString(String("a126d48a-aaef-47c4-baed-7f0e44aeffe5"), (IUUID**)&uuid);
    return uuid;
}

const AutoPtr<IUUID> ProfileManagerService::mWildcardUUID = initWildcardUUID();

//--------------------------------------------------------------------------------
//                ProfileManagerService::MyBroadcastReceiver
//--------------------------------------------------------------------------------
ProfileManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ ProfileManagerService* host)
    : mHost(host)
{}

ECode ProfileManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_LOCALE_CHANGED)) {
        mHost->PersistIfDirty();
        mHost->Initialize();
    }
    else if (action.Equals(IIntent::ACTION_SHUTDOWN)) {
        mHost->PersistIfDirty();
    }
    return NOERROR;
}

//--------------------------------------------------------------------------------
//                ProfileManagerService
//--------------------------------------------------------------------------------
ProfileManagerService::ProfileManagerService()
{}

CAR_INTERFACE_IMPL_2(ProfileManagerService, Object, IIProfileManager, IBinder)

ECode ProfileManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    assert(0 && "TODO");
    // CBackupManager::New(mContext.Get(), (IBackupManager**)&mBackupManager);
    mTriggerHelper = new ProfileTriggerHelper(mContext, this);
    String str;
    context->GetString(R::string::wildcardProfile, &str);
    CNotificationGroup::New(str, R::string::wildcardProfile, mWildcardUUID, (INotificationGroup**)&mWildcardGroup);
    Initialize();
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    filter->AddAction(IIntent::ACTION_SHUTDOWN);
    mIntentReceiver = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    return mContext->RegisterReceiver(mIntentReceiver.Get(), filter.Get(), (IIntent**)&intent);
}

void ProfileManagerService::Initialize()
{
    Initialize(FALSE);
}

void ProfileManagerService::Initialize(
    /* [in] */ Boolean skipFile)
{
    CHashMap::New((IHashMap**)&mProfiles);
    CHashMap::New((IHashMap**)&mProfileNames);
    CHashMap::New((IHashMap**)&mGroups);
    mDirty = FALSE;
    Boolean init = skipFile;
    if (!skipFile) {
        // try {
            LoadFromFile();
        // } catch (XmlPullParserException e) {
            // init = true;
        // } catch (IOException e) {
            // init = true;
        // }
    }

    if (init) {
        // try {
            InitialiseStructure();
        // } catch (Throwable ex) {
            // Log.e(TAG, "Error loading xml from resource: ", ex);
        // }
    }
}

ECode ProfileManagerService::ResetAll()
{
    EnforceChangePermissions();
    Initialize(TRUE);
    return NOERROR;
}

ECode ProfileManagerService::SetActiveProfileByName(
    /* [in] */ const String& profileName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    mProfileNames->ContainsKey(StringUtils::ParseCharSequence(profileName).Get(), &flag);
    if (!flag) {
        // Since profileName could not be casted into a UUID, we can call it a string.
        Logger::W(TAG, "Unable to find profile to set active, based on string: %s", profileName.string());
        *result = FALSE;
        return NOERROR;
    }

    if (LOCAL_LOGV) {
        Logger::V(TAG, "setActiveProfile(String) found profile name in mProfileNames.");
    }
    AutoPtr<IInterface> obj;
    mProfileNames->Get(StringUtils::ParseCharSequence(profileName).Get(), (IInterface**)&obj);
    AutoPtr<IUUID> uuid = IUUID::Probe(obj);
    obj = NULL;
    mProfiles->Get(uuid.Get(), (IInterface**)&obj);
    SetActiveProfile(IProfile::Probe(obj), TRUE);
    *result = TRUE;
    return NOERROR;
}

ECode ProfileManagerService::SetActiveProfile(
    /* [in] */ IParcelUuid* profileParcelUuid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUUID> uuid;
    profileParcelUuid->GetUuid((IUUID**)&uuid);
    return SetActiveProfile(uuid.Get(), TRUE, result);
}

ECode ProfileManagerService::SetActiveProfile(
    /* [in] */ IUUID* profileUuid,
    /* [in] */ Boolean doInit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    FAIL_RETURN(mProfiles->ContainsKey(profileUuid, &flag));
    if (!flag) {
        String tmp;
        profileUuid->ToString(&tmp);
        Logger::E(TAG, "Cannot set active profile to:  - does not exist.", tmp.string());
        *result = FALSE;
        return NOERROR;
    }

    if (LOCAL_LOGV) Logger::V(TAG, "setActiveProfile(UUID, boolean) found UUID in mProfiles.");
    AutoPtr<IInterface> obj;
    mProfiles->Get(profileUuid, (IInterface**)&obj);
    SetActiveProfile(IProfile::Probe(obj), doInit);
    *result = TRUE;
    return NOERROR;
}

void ProfileManagerService::SetActiveProfile(
    /* [in] */ IProfile* newActiveProfile,
    /* [in] */ Boolean doInit)
{
    /*
     * NOTE: Since this is not a public function, and all public functions
     * take either a string or a UUID, the active profile should always be
     * in the collection.  If writing another setActiveProfile which receives
     * a Profile object, run enforceChangePermissions, add the profile to the
     * list, and THEN add it.
     */

    EnforceChangePermissions();

    AutoPtr<IUUID> uuid;
    newActiveProfile->GetUuid((IUUID**)&uuid);
    String tmp;
    uuid->ToString(&tmp);
    String name;
    newActiveProfile->GetName(&name);
    Logger::D(TAG, "Set active profile to: %s - ", tmp.string(), name.string());

    AutoPtr<IProfile> lastProfile = mActiveProfile;
    mActiveProfile = newActiveProfile;
    mDirty = TRUE;

    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> all;
    uhh->GetALL((IUserHandle**)&all);

    if (doInit) {
        if (LOCAL_LOGV) Logger::V(TAG, "setActiveProfile(Profile, boolean) - Running init");

        /*
         * We need to clear the caller's identity in order to
         * - allow the profile switch to execute actions
         *   not included in the caller's permissions
         * - broadcast INTENT_ACTION_PROFILE_SELECTED
         */
        Int64 token = Binder::ClearCallingIdentity();

        // Call profile's "doSelect"
        mActiveProfile->DoSelect(mContext.Get());

        // Notify other applications of newly selected profile.
        AutoPtr<IIntent> broadcast;
        CIntent::New(IProfileManager::INTENT_ACTION_PROFILE_SELECTED, (IIntent**)&broadcast);
        mActiveProfile->GetName(&name);
        broadcast->PutExtra(IProfileManager::EXTRA_PROFILE_NAME, name);
        uuid =  NULL;
        mActiveProfile->GetUuid((IUUID**)&uuid);
        uuid->ToString(&tmp);
        broadcast->PutExtra(IProfileManager::EXTRA_PROFILE_UUID, tmp);
        lastProfile->GetName(&name);
        broadcast->PutExtra(IProfileManager::EXTRA_LAST_PROFILE_NAME, name);
        uuid = NULL;
        lastProfile->GetUuid((IUUID**)&uuid);
        uuid->ToString(&tmp);
        broadcast->PutExtra(IProfileManager::EXTRA_LAST_PROFILE_UUID, tmp);

        mContext->SendBroadcastAsUser(broadcast.Get(), all.Get());
        Binder::RestoreCallingIdentity(token);
        PersistIfDirty();
    }
    else if (lastProfile != mActiveProfile && ActivityManagerNative::IsSystemReady()) {
        // Something definitely changed: notify.
        Int64 token = Binder::ClearCallingIdentity();
        AutoPtr<IIntent> broadcast;
        CIntent::New(IProfileManager::INTENT_ACTION_PROFILE_UPDATED, (IIntent**)&broadcast);
        mActiveProfile->GetName(&name);
        broadcast->PutExtra(IProfileManager::EXTRA_PROFILE_NAME, name);
        uuid = NULL;
        uuid->ToString(&tmp);
        broadcast->PutExtra(IProfileManager::EXTRA_PROFILE_UUID, tmp);
        mContext->SendBroadcastAsUser(broadcast.Get(), all);
        Binder::RestoreCallingIdentity(token);
    }
}

ECode ProfileManagerService::AddProfile(
    /* [in] */ IProfile* profile,
    /* [out] */ Boolean* result)
{
    EnforceChangePermissions();
    AddProfileInternal(profile);
    PersistIfDirty();
    *result = TRUE;
    return NOERROR;
}

void ProfileManagerService::AddProfileInternal(
    /* [in] */ IProfile* profile)
{
    // Make sure this profile has all of the correct groups.
    AutoPtr<ICollection> values;
    mGroups->GetValues((ICollection**)&values);
    AutoPtr<IArrayList> arrayList;
    CArrayList::New((IArrayList**)&arrayList);
    Boolean flag = FALSE;
    arrayList->AddAll(values, &flag);
    Int32 size;
    arrayList->GetSize(&size);
    AutoPtr<IInterface> obj;
    AutoPtr<INotificationGroup> group;
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        arrayList->Get(i, (IInterface**)&obj);
        group = INotificationGroup::Probe(obj);
        EnsureGroupInProfile(profile, group.Get(), FALSE);
    }

    EnsureGroupInProfile(profile, mWildcardGroup.Get(), true);
    AutoPtr<IUUID> uuid;
    profile->GetUuid((IUUID**)&uuid);
    mProfiles->Put(uuid, profile);
    String name;
    profile->GetName(&name);
    mProfileNames->Put(StringUtils::ParseCharSequence(name).Get(), uuid.Get());
    mDirty = TRUE;
}

void ProfileManagerService::EnsureGroupInProfile(
    /* [in] */ IProfile* profile,
    /* [in] */ INotificationGroup* group,
    /* [in] */ Boolean defaultGroup)
{
    AutoPtr<IUUID> uuid;
    group->GetUuid((IUUID**)&uuid);
    AutoPtr<IProfileGroup> gp;
    profile->GetProfileGroup(uuid, (IProfileGroup**)&gp);
    if (gp != NULL) {
        return;
    }

    /* enforce a matchup between profile and notification group, which not only
     * works by UUID, but also by name for backwards compatibility */
    AutoPtr<ArrayOf<IProfileGroup*> > groups;
    profile->GetProfileGroups((ArrayOf<IProfileGroup*>**)&groups);
    AutoPtr<IProfileGroup> pg;
    Boolean flag = FALSE;
    for (Int32 i = 0; i < groups->GetLength(); ++i) {
        ((*groups)[i])->Matches(group, defaultGroup, &flag);
        if (flag) {
            return;
        }
    }

    /* didn't find any, create new group */
    group->GetUuid((IUUID**)&uuid);
    AutoPtr<IProfileGroup> pfgroup;
    CProfileGroup::New(uuid, defaultGroup, (IProfileGroup**)&pfgroup);
    profile->AddProfileGroup(pfgroup.Get());
}

ECode ProfileManagerService::GetProfileByName(
    /* [in] */ const String& profileName,
    /* [out] */ IProfile** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    mProfiles->ContainsKey(StringUtils::ParseCharSequence(profileName).Get(), &flag);
    AutoPtr<IInterface> obj;
    AutoPtr<IUUID> uuid;
    if (flag) {
        mProfileNames->Get(StringUtils::ParseCharSequence(profileName).Get(), (IInterface**)&obj);
        uuid = IUUID::Probe(obj);
        obj = NULL;
        mProfiles->Get(uuid.Get(), (IInterface**)&obj);
        *result = IProfile::Probe(obj);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        AutoPtr<IUUIDHelper> uudh;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uudh);
        uudh->FromString(profileName, (IUUID**)&uuid);
        mProfiles->ContainsKey(uuid.Get(), &flag);
        if (flag) {
            AutoPtr<IInterface> obj;
            mProfiles->Get(uuid.Get(), (IInterface**)&obj);
            *result = IProfile::Probe(obj);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        else {
            *result = NULL;
            return NOERROR;
        }
    }
}

ECode ProfileManagerService::GetProfile(
    /* [in] */ IParcelUuid* profileParcelUuid,
    /* [out] */ IProfile** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUUID> profileUuid;
    profileParcelUuid->GetUuid((IUUID**)&profileUuid);
    *result = GetProfile(profileUuid.Get());
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IProfile> ProfileManagerService::GetProfile(
    /* [in] */ IUUID* profileUuid)
{
    // use primary UUID first
    Boolean flag = FALSE;
    mProfiles->ContainsKey(profileUuid, &flag);
    AutoPtr<IProfile> profile;
    AutoPtr<IInterface> obj;
    if (flag) {
        mProfiles->Get(profileUuid, (IInterface**)&obj);
        profile = IProfile::Probe(obj);
        return profile;
    }
    // if no match was found: try secondary UUID
    AutoPtr<ICollection> collection;
    mProfiles->GetValues((ICollection**)&collection);
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    list->AddAll(collection.Get());
    Int32 size;
    list->GetSize(&size);
    AutoPtr<IProfile> p;
    AutoPtr<ArrayOf<IUUID*> > uuids;
    AutoPtr<IUUID> uuid;
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        p = NULL;
        p = IProfile::Probe(obj);
        uuids = NULL;
        p->GetSecondaryUuids((ArrayOf<IUUID*>**)&uuids);
        Int32 length = uuids->GetLength();
        for (Int32 j = 0; j < length; ++j) {
            uuid = NULL;
            uuid = (*uuids)[j];
            profileUuid->Equals(uuid, &flag);
            if (flag) return p;
        }
    }
    // nothing found
    return NULL;
}

AutoPtr<ICollection> ProfileManagerService::GetProfileList()
{
    AutoPtr<ICollection> collection;
    mProfiles->GetValues((ICollection**)&collection);
    return collection;
}

ECode ProfileManagerService::GetProfiles(
    /* [out, callee] */ ArrayOf<IProfile*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollection> collection = GetProfileList();
    Int32 size;
    mProfiles->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inarray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > outarray;
    collection->ToArray(inarray.Get(), (ArrayOf<IInterface*>**)&outarray);

    AutoPtr<ArrayOf<IProfile*> > profiles = ArrayOf<IProfile*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        profiles->Set(i, IProfile::Probe((*outarray)[i]));
    }
    Arrays::Sort(profiles.Get());
    *result = profiles;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ProfileManagerService::GetActiveProfile(
    /* [out] */ IProfile** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActiveProfile;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ProfileManagerService::RemoveProfile(
    /* [in] */ IProfile* profile,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermissions();
    String name;
    profile->GetName(&name);
    AutoPtr<IInterface> obj;
    mProfileNames->Remove(StringUtils::ParseCharSequence(name).Get(), (IInterface**)&obj);
    if (obj != NULL) {
        AutoPtr<IUUID> uuid;
        profile->GetUuid((IUUID**)&uuid);
        AutoPtr<IInterface> tmpObj;
        mProfiles->Remove(uuid.Get(), (IInterface**)&tmpObj);
        if (tmpObj != NULL) {
            mDirty = TRUE;
            PersistIfDirty();
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ProfileManagerService::UpdateProfile(
    /* [in] */ IProfile* profile)
{
    EnforceChangePermissions();
    AutoPtr<IUUID> uuid;
    profile->GetUuid((IUUID**)&uuid);
    AutoPtr<IInterface> obj;
    mProfiles->Get(uuid.Get(), (IInterface**)&obj);
    AutoPtr<IProfile> old = IProfile::Probe(obj);

    if (old == NULL) {
        return NOERROR;
    }
    String name;
    old->GetName(&name);
    mProfileNames->Remove(StringUtils::ParseCharSequence(name).Get());
    String pname;
    profile->GetName(&pname);
    uuid = NULL;
    profile->GetUuid((IUUID**)&uuid);
    mProfileNames->Put(StringUtils::ParseCharSequence(pname).Get(), uuid.Get());
    mProfiles->Put(uuid.Get(), profile);
    /* no need to set mDirty, if the profile was actually changed,
     * it's marked as dirty by itself */
    PersistIfDirty();

    // Also update if we changed the active profile
    uuid = NULL;
    mActiveProfile->GetUuid((IUUID**)&uuid);
    AutoPtr<IUUID> puuid;
    profile->GetUuid((IUUID**)&puuid);
    Boolean flag = FALSE;
    uuid->Equals(puuid, &flag);
    if (mActiveProfile != NULL && flag) {
        SetActiveProfile(profile, TRUE);
    }
    return NOERROR;
}

ECode ProfileManagerService::ProfileExists(
    /* [in] */ IParcelUuid* profileUuid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUUID> uuid;
    profileUuid->GetUuid((IUUID**)&uuid);
    return mProfiles->ContainsKey(uuid.Get(), result);
}

ECode ProfileManagerService::ProfileExistsByName(
    /* [in] */ const String& profileName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISet> entrySet;
    mProfileNames->GetEntrySet((ISet**)&entrySet);

    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> k;
        entry->GetKey((IInterface**)&k);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(k);
        String str;
        cs->ToString(&str);
        if (str.EqualsIgnoreCase(profileName)) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}


ECode ProfileManagerService::NotificationGroupExistsByName(
    /* [in] */ const String& notificationGroupName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICollection> values;
    mGroups->GetValues((ICollection**)&values);

    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<INotificationGroup> group = INotificationGroup::Probe(p);
        String name;
        group->GetName(&name);
        if (name.EqualsIgnoreCase(notificationGroupName)) {
            *result = TRUE;
            return NOERROR;
        }
        it = IIterator::Probe(p);
    }

    *result = FALSE;
    return NOERROR;
}

ECode ProfileManagerService::GetNotificationGroups(
    /* [out] */ ArrayOf<INotificationGroup*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollection> values;
    mGroups->GetValues((ICollection**)&values);
    Int32 size;
    mGroups->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > outArray;
    values->ToArray(inArray.Get(), (ArrayOf<IInterface*>**)&outArray);
    Int32 length = outArray->GetLength();
    AutoPtr<ArrayOf<INotificationGroup*> > groups = ArrayOf<INotificationGroup*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        groups->Set(i, INotificationGroup::Probe((*outArray)[i]));
    }
    *result = groups.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ProfileManagerService::AddNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    EnforceChangePermissions();
    AddNotificationGroupInternal(group);
    PersistIfDirty();
    return NOERROR;
}

void ProfileManagerService::AddNotificationGroupInternal(
    /* [in] */ INotificationGroup* group)
{
    AutoPtr<IUUID> uuid;
    group->GetUuid((IUUID**)&uuid);
    AutoPtr<IInterface> obj;
    if ((mGroups->Put(uuid.Get(), group, (IInterface**)&obj), obj == NULL)) {
        // If the above is true, then the ProfileGroup shouldn't exist in
        // the profile. Ensure it is added.
        AutoPtr<ICollection> values;
        mProfiles->GetValues((ICollection**)&values);

        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IProfile> profile = IProfile::Probe(p);
            EnsureGroupInProfile(profile.Get(), group, FALSE);
        }
    }
    mDirty = TRUE;
}

ECode ProfileManagerService::RemoveNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    EnforceChangePermissions();
    AutoPtr<IUUID> uuid;
    group->GetUuid((IUUID**)&uuid);
    AutoPtr<IInterface> obj;
    mGroups->Remove(uuid.Get(), (IInterface**)&obj);
    mDirty |= (obj != NULL);
    // Remove the corresponding ProfileGroup from all the profiles too if
    // they use it.
    AutoPtr<ICollection> values;
    mProfiles->GetValues((ICollection**)&values);

    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IProfile> profile = IProfile::Probe(p);
        profile->RemoveProfileGroup(uuid);
    }

    PersistIfDirty();
    return NOERROR;
}

ECode ProfileManagerService::UpdateNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    EnforceChangePermissions();
    AutoPtr<IUUID> uuid;
    group->GetUuid((IUUID**)&uuid);
    AutoPtr<IInterface> obj;
    mGroups->Get(uuid, (IInterface**)&obj);
    AutoPtr<INotificationGroup> old = INotificationGroup::Probe(obj);
    if (old == NULL) {
        return NOERROR;
    }

    mGroups->Put(uuid.Get(), group);
    /* no need to set mDirty, if the group was actually changed,
     * it's marked as dirty by itself */
    PersistIfDirty();
    return NOERROR;
}

ECode ProfileManagerService::GetNotificationGroupForPackage(
    /* [in] */ const String& pkg,
    /* [out] */ INotificationGroup** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICollection> values;
    mGroups->GetValues((ICollection**)&values);

    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    Boolean flag = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<INotificationGroup> group = INotificationGroup::Probe(p);
        group->HasPackage(pkg, &flag);
        if (flag) {
            *result = group;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

void ProfileManagerService::SettingsRestored()
{
    Initialize();
    AutoPtr<ICollection> values;
    mProfiles->GetValues((ICollection**)&values);

    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IProfile> p = IProfile::Probe(obj);
        p->ValidateRingtones(mContext);
    }

    PersistIfDirty();
}

ECode ProfileManagerService::LoadFromFile()
{
    AutoPtr<IXmlPullParserFactoryHelper> ppf;
    CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&ppf);
    AutoPtr<IXmlPullParserFactory> xppf;
    FAIL_RETURN(ppf->NewInstance((IXmlPullParserFactory**)&xppf));
    AutoPtr<IXmlPullParser> xpp;
    FAIL_RETURN(xppf->NewPullParser((IXmlPullParser**)&xpp));
    AutoPtr<IFileReader> fr;
    CFileReader::New(PROFILE_FILE, (IFileReader**)&fr);
    FAIL_RETURN(xpp->SetInput(IReader::Probe(fr)));
    FAIL_RETURN(LoadXml(xpp.Get(), mContext.Get()));
    ICloseable::Probe(fr)->Close();
    PersistIfDirty();
    return NOERROR;
}

ECode ProfileManagerService::LoadXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context)
{
    Int32 event;
    xpp->Next(&event);
    String active;
    String _name;
    while (event != IXmlPullParser::END_TAG || !String("profiles").Equals((xpp->GetName(&_name), _name))) {
        if (event == IXmlPullParser::START_TAG) {
            String name;
            xpp->GetName(&name);
            if (name.Equals("active")) {
                xpp->NextText(&active);
                Logger::D(TAG, "Found active: %s", active.string());
            }
            else if (name.Equals("profile")) {
                AutoPtr<IProfileHelper> phl;
                CProfileHelper::AcquireSingleton((IProfileHelper**)&phl);
                AutoPtr<IProfile> prof;
                phl->FromXml(xpp, context, (IProfile**)&prof);
                AddProfileInternal(prof.Get());
                // Failsafe if no active found
                if (active.IsNull()) {
                    AutoPtr<IUUID> uuid;
                    prof->GetUuid((IUUID**)&uuid);
                    uuid->ToString(&active);
                }
            }
            else if (name.Equals("notificationGroup")) {
                AutoPtr<INotificationGroupHelper> ngh;
                CNotificationGroupHelper::AcquireSingleton((INotificationGroupHelper**)&ngh);
                AutoPtr<INotificationGroup> ng;
                ngh->FromXml(xpp, context, (INotificationGroup**)&ng);
                AddNotificationGroupInternal(ng);
            }
        }
        else if (event == IXmlPullParser::END_DOCUMENT) {
            Slogger::E("ProfileManagerService", "Premature end of file while reading %p", PROFILE_FILE.Get());
            // throw new IOException("Premature end of file while reading " + PROFILE_FILE);
            return E_IO_EXCEPTION;
        }
        xpp->Next(&event);
    }
    // Don't do initialisation on startup. The AudioManager doesn't exist yet
    // and besides, the volume settings will have survived the reboot.
    // try {
        // Try / catch block to detect if XML file needs to be upgraded.
        ECode ec = NOERROR;
        AutoPtr<IUUIDHelper> uudh;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uudh);
        AutoPtr<IUUID> uuid;
        uudh->FromString(active, (IUUID**)&uuid);
        Boolean flag = FALSE;
        ec = SetActiveProfile(uuid.Get(), FALSE, &flag);
    // } catch (IllegalArgumentException e) {
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            Boolean flag = FALSE;
            mProfileNames->ContainsKey(StringUtils::ParseCharSequence(active), &flag);
            if (flag) {
                AutoPtr<IInterface> obj;
                mProfileNames->Get(StringUtils::ParseCharSequence(active), (IInterface**)&obj);
                IUUID* uid = IUUID::Probe(obj);
                SetActiveProfile(uid, FALSE, &flag);
            }
            else {
                // Final fail-safe: We must have SOME profile active.
                // If we couldn't select one by now, we'll pick the first in the set.
                AutoPtr<ICollection> values;
                mProfiles->GetValues((ICollection**)&values);
                AutoPtr<IIterator> it;
                values->GetIterator((IIterator**)&it);
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IProfile* profile = IProfile::Probe(obj);
                SetActiveProfile(profile, FALSE);
            }
            // This is a hint that we probably just upgraded the XML file. Save changes.
            mDirty = TRUE;
        }
    // }
    return NOERROR;
}

void ProfileManagerService::InitialiseStructure()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> xml;
    res->GetXml(R::xml::profile_default, (IXmlResourceParser**)&xml);
    // try {
    LoadXml((IXmlPullParser*)((Object*)xml.Get()), mContext.Get());
    mDirty = TRUE;
    PersistIfDirty();
    // } finally {
        ((ICloseable*)((Object*)xml.Get()))->Close();
    // }
}

String ProfileManagerService::GetXmlString()
{
    AutoPtr<StringBuilder> builder = new StringBuilder();
    builder->Append("<profiles>\n<active>");
    AutoPtr<IProfile> profile;
    GetActiveProfile((IProfile**)&profile);
    AutoPtr<IUUID> uuid;
    profile->GetUuid((IUUID**)&uuid);
    String str;
    uuid->ToString(&str);
    builder->Append(TextUtils::HtmlEncode(str));
    builder->Append("</active>\n");

    AutoPtr<ICollection> values;
    mProfiles->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IProfile> p = IProfile::Probe(obj);
        p->GetXmlString(IStringBuilder::Probe(builder), mContext.Get());
        it = IIterator::Probe(obj);
    }

    AutoPtr<ICollection> groups;
    mProfiles->GetValues((ICollection**)&groups);

    values->GetIterator((IIterator**)&it);
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<INotificationGroup> g = INotificationGroup::Probe(obj);
        g->GetXmlString(IStringBuilder::Probe(builder), mContext.Get());
        it = IIterator::Probe(obj);
    }
    builder->Append("</profiles>\n");
    return builder->ToString();
}

ECode ProfileManagerService::GetNotificationGroup(
    /* [in] */ IParcelUuid* uuid,
    /* [out] */ INotificationGroup** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUUID> _uuid;
    mWildcardGroup->GetUuid((IUUID**)&_uuid);
    AutoPtr<IUUID> uid;
    uuid->GetUuid((IUUID**)&uid);
    Boolean flag = FALSE;
    if (uid->Equals(_uuid.Get(), &flag), flag) {
        *result = mWildcardGroup;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mGroups->Get(uid, (IInterface**)&obj);
    AutoPtr<INotificationGroup> ng = INotificationGroup::Probe(obj);
    *result = ng;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ProfileManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CProfileManagerService";
    return NOERROR;
}

void ProfileManagerService::PersistIfDirty()
{
    AutoLock lock(this);
    Boolean dirty = mDirty;
    Boolean hasNext = FALSE;
    Boolean flag = FALSE;
    if (!dirty) {
        AutoPtr<ICollection> profiles;
        mProfiles->GetValues((ICollection**)&profiles);
        AutoPtr<IIterator> it;
        profiles->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IProfile* profile = IProfile::Probe(obj);
            if (profile->IsDirty(&flag), flag) {
                dirty = TRUE;
                break;
            }
        }
    }

    if (!dirty) {
        AutoPtr<ICollection> groups;
        mGroups->GetValues((ICollection**)&groups);
        AutoPtr<IIterator> it;
        groups->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            INotificationGroup* group = INotificationGroup::Probe(obj);
            if (group->IsDirty(&flag), flag) {
                dirty = TRUE;
                break;
            }
        }
    }

    if (dirty) {
        // try {
            Logger::D(TAG, "Saving profile data...");
            AutoPtr<IFileWriter> fw;
            CFileWriter::New(PROFILE_FILE, (IFileWriter**)&fw);
            IWriter::Probe(fw)->Write(GetXmlString());
            ICloseable::Probe(fw)->Close();
            Logger::D(TAG, "Save completed.");
            mDirty = FALSE;

            Int64 token = Binder::ClearCallingIdentity();
            mBackupManager->DataChanged();
            Binder::RestoreCallingIdentity(token);
        // } catch (Throwable e) {
            // e.printStackTrace();
        // }
    }
}

void ProfileManagerService::EnforceChangePermissions()
{
    mContext->EnforceCallingOrSelfPermission(PERMISSION_CHANGE_SETTINGS,
            String("You do not have permissions to change the Profile Manager."));
}

} // Server
} // Droid
} // Elastos