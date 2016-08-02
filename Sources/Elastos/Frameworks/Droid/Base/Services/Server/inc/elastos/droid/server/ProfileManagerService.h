#ifndef __ELASTOS_DROID_SERVER_PROFILEMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_PROFILEMANAGERSERVICE_H__

#include "elastos/droid/server/ProfileTriggerHelper.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::App::IIProfileManager;
using Elastos::Droid::App::INotificationGroup;
using Elastos::Droid::App::IProfile;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IUUID;
using Elastos::IO::IFile;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {

class ProfileManagerService
    : public Object
    , public IIProfileManager
    , public IBinder
{
    friend class ProfileTriggerHelper;

private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ProfileManagerService::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ ProfileManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ProfileManagerService* mHost;
    };

public:
    ProfileManagerService();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI ResetAll();

    // @Override
    // @Deprecated
    CARAPI SetActiveProfileByName(
        /* [in] */ const String& profileName,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetActiveProfile(
        /* [in] */ IParcelUuid* profileParcelUuid,
        /* [out] */ Boolean* result);

    /* package */
    CARAPI_(void) SetActiveProfile(
        /* [in] */ IProfile* newActiveProfile,
        /* [in] */ Boolean doInit);

    //@Override
    CARAPI AddProfile(
        /* [in] */ IProfile* profile,
        /* [out] */ Boolean* result);

    // @Override
    // @Deprecated
    CARAPI GetProfileByName(
        /* [in] */ const String& profileName,
        /* [out] */ IProfile** result);

    // @Override
    CARAPI GetProfile(
        /* [in] */ IParcelUuid* profileParcelUuid,
        /* [out] */ IProfile** result);

    // @Override
    CARAPI GetProfiles(
        /* [out, callee] */ ArrayOf<IProfile*>** result);

    // @Override
    CARAPI GetActiveProfile(
        /* [out] */ IProfile** result);

    // @Override
    CARAPI RemoveProfile(
        /* [in] */ IProfile* profile,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI UpdateProfile(
        /* [in] */ IProfile* profile);

    // @Override
    CARAPI ProfileExists(
        /* [in] */ IParcelUuid* profileUuid,
        /* [out] */ Boolean* result);

    // @Override
    // @Deprecated
    CARAPI ProfileExistsByName(
        /* [in] */ const String& profileName,
        /* [out] */ Boolean* result);

    // @Override
    // @Deprecated
    CARAPI NotificationGroupExistsByName(
        /* [in] */ const String& notificationGroupName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetNotificationGroups(
        /* [out] */ ArrayOf<INotificationGroup*>** result);

    // @Override
    CARAPI AddNotificationGroup(
        /* [in] */ INotificationGroup* group);

    // @Override
    CARAPI RemoveNotificationGroup(
        /* [in] */ INotificationGroup* group);

    // @Override
    CARAPI UpdateNotificationGroup(
        /* [in] */ INotificationGroup* group);

    CARAPI GetNotificationGroupForPackage(
        /* [in] */ const String& pkg,
        /* [out] */ INotificationGroup** result);

    // Called by SystemBackupAgent after files are restored to disk.
    CARAPI_(void) SettingsRestored();

    // @Override
    CARAPI GetNotificationGroup(
        /* [in] */ IParcelUuid* uuid,
        /* [out] */ INotificationGroup** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) Initialize();

    CARAPI_(void) Initialize(
        /* [in] */ Boolean skipFile);

    CARAPI SetActiveProfile(
        /* [in] */ IUUID* profileUuid,
        /* [in] */ Boolean doInit,
        /* [out] */ Boolean* result);

    CARAPI_(void) AddProfileInternal(
        /* [in] */ IProfile* profile);

    CARAPI_(void) EnsureGroupInProfile(
        /* [in] */ IProfile* profile,
        /* [in] */ INotificationGroup* group,
        /* [in] */ Boolean defaultGroup);

    CARAPI_(AutoPtr<IProfile>) GetProfile(
        /* [in] */ IUUID* profileUuid);

    /* package */
    CARAPI_(AutoPtr<ICollection>) GetProfileList();

    CARAPI_(void) AddNotificationGroupInternal(
        /* [in] */ INotificationGroup* group);

    CARAPI LoadFromFile();

    CARAPI LoadXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context);

    CARAPI InitialiseStructure();

    CARAPI_(String) GetXmlString();

    CARAPI_(void) PersistIfDirty();

    CARAPI EnforceChangePermissions();

private:
    static const String TAG;
    // Enable the below for detailed logging of this class
    static Boolean LOCAL_LOGV;

    AutoPtr<IHashMap> mProfiles; // Map<UUID, Profile>

    // Match UUIDs and names, used for reverse compatibility
    AutoPtr<IHashMap> mProfileNames; // Map<String, UUID>

    AutoPtr<IHashMap> mGroups; // Map<UUID, NotificationGroup>

    AutoPtr<IProfile> mActiveProfile;

    AutoPtr<INotificationGroup> mWildcardGroup;

    AutoPtr<IContext> mContext;
    Boolean mDirty;
    AutoPtr<IBackupManager> mBackupManager;
    AutoPtr<ProfileTriggerHelper> mTriggerHelper;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;

public:
    static const String PERMISSION_CHANGE_SETTINGS;

    /* package */ static const AutoPtr<IFile> PROFILE_FILE;

private:
    // Well-known UUID of the wildcard group
    static const AutoPtr<IUUID> mWildcardUUID;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PROFILEMANAGERSERVICE_H__