#ifndef __ELASTOS_DROID_SERVER_PM_CUSERMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_PM_CUSERMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Server_Pm_CUserManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IIUserManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class CPackageManagerService;

CarClass(CUserManagerService)
    , public Object
    , public IIUserManager
    , public IBinder
{
public:
    class RestrictionsPinState : public Object
    {
    public:
        RestrictionsPinState()
            : mSalt()
            , mFailedAttempts(0)
            , mLastAttemptTime(0)
        {}

    public:
        Int64 mSalt;
        String mPinHash;
        Int32 mFailedAttempts;
        Int64 mLastAttemptTime;
    };

private:
    class FinishRemoveUserReceiver : public BroadcastReceiver
    {
    private:
        class FinishRemoveUserThread : public Thread
        {
        public:
            FinishRemoveUserThread(
                /* [in] */ CUserManagerService* host,
                /* [in] */ Int32 userHandle);

            CARAPI Run();

        private:
            CUserManagerService* mHost;
            Int32 mUserHandle;
        };

    public:
        TO_STRING_IMPL("CUserManagerService::FinishRemoveUserReceiver")

        FinishRemoveUserReceiver(
            /* [in] */ CUserManagerService* host,
            /* [in] */ Int32 userHandle)
            : mHost(host)
            , mUserHandle(userHandle)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CUserManagerService* mHost;
        Int32 mUserHandle;
    };

    class RemoveUserStateRunnable : public Runnable
    {
    public:
        RemoveUserStateRunnable(
            /* [in] */ CUserManagerService* host,
            /* [in] */ Int32 userHandle)
            : mHost(host)
            , mUserHandle(userHandle)
        {}

        CARAPI Run();

    private:
        CUserManagerService* mHost;
        Int32 mUserHandle;
    };

    class UnhideAllInstalledAppsRunnable : public Runnable
    {
    public:
        UnhideAllInstalledAppsRunnable(
            /* [in] */ CUserManagerService* host,
            /* [in] */ Int32 userHandle)
            : mHost(host)
            , mUserHandle(userHandle)
        {}

        CARAPI Run();

    private:
        CUserManagerService* mHost;
        Int32 mUserHandle;
    };

public:
    CUserManagerService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IFile* dataDir,
        /* [in] */ IFile* baseUserPath);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIPackageManager* pm,
        /* [in] */ IObject* installLock,
        /* [in] */ IObject* packagesLock);

    static CARAPI_(AutoPtr<CUserManagerService>) GetInstance();

    CARAPI_(void) SystemReady();

    // @Override
    CARAPI GetUsers(
        /* [in] */ Boolean excludeDying,
        /* [out] */ IList** users);

    CARAPI GetProfiles(
        /* [in] */ Int32 userHandle,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ IList** result);

    CARAPI GetProfileParent(
        /* [in] */ Int32 userHandle,
        /* [out] */ IUserInfo** info);

    CARAPI SetUserEnabled(
        /* [in] */ Int32 userHandle);

    // @Override
    CARAPI GetUserInfo(
        /* [in] */ Int32 userId,
        /* [out] */ IUserInfo** userInfo);

    CARAPI IsRestricted(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) Exists(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI SetUserName(
        /* [in] */ Int32 userId,
        /* [in] */ const String& name);

    // @Override
    CARAPI SetUserIcon(
        /* [in] */ Int32 userId,
        /* [in] */ IBitmap* bitmap);

    // @Override
    CARAPI GetUserIcon(
        /* [in] */ Int32 userId,
        /* [out] */ IBitmap** userIcon);

    CARAPI MakeInitialized(
        /* [in] */ Int32 userId);

    CARAPI GetDefaultGuestRestrictions(
        /* [out] */ IBundle** result);

    CARAPI SetDefaultGuestRestrictions(
        /* [in] */ IBundle* restrictions);

    CARAPI HasUserRestriction(
        /* [in] */ const String& restrictionKey,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI GetUserRestrictions(
        /* [in] */ Int32 userHandle,
        /* [out] */ IBundle** bundle);

    CARAPI SetUserRestrictions(
        /* [in] */ IBundle* restrictions,
        /* [in] */ Int32 userHandle);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetUserIds();

    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetUserIdsLPr();

    CARAPI CreateProfileForUser(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [out] */ IUserInfo** userInfo);

    // @Override
    CARAPI CreateUser(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IUserInfo** userInfo);

    /**
     * Mark this guest user for deletion to allow us to create another guest
     * and switch to that user before actually removing this guest.
     * @param userHandle the userid of the current guest
     * @return whether the user could be marked for deletion
     */
    CARAPI MarkGuestForDeletion(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Removes a user and all data directories created for that user. This method should be called
     * after the user's processes have been terminated.
     * @param userHandle the user's id
     */
    CARAPI RemoveUser(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* succeeded);

    CARAPI_(void) FinishRemoveUser(
        /* [in] */ Int32 userHandle);

    CARAPI GetApplicationRestrictions(
        /* [in] */ const String& packageName,
        /* [out] */ IBundle** bundle);

    CARAPI GetApplicationRestrictionsForUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [out] */ IBundle** bundle);

    CARAPI SetApplicationRestrictions(
        /* [in] */ const String& packageName,
        /* [in] */ IBundle* restrictions,
        /* [in] */ Int32 userHandle);

    CARAPI SetRestrictionsChallenge(
        /* [in] */ const String& newPin,
        /* [out] */ Boolean* result);

    CARAPI CheckRestrictionsChallenge(
        /* [in] */ const String& pin,
        /* [out] */ Int32* result);

    CARAPI HasRestrictionsChallenge(
        /* [out] */ Boolean* result);

    CARAPI RemoveRestrictions();

    // @Override
    CARAPI GetUserSerialNumber(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* serialNo);

    // @Override
    CARAPI GetUserHandle(
        /* [in] */ Int32 userSerialNumber,
        /* [out] */ Int32* userHandle);

    /**
     * Make a note of the last started time of a user and do some cleanup.
     * @param userId the user that was just foregrounded
     */
    CARAPI_(void) UserForeground(
        /* [in] */ Int32 userId);

protected:
    // @Override
    // protected void dump(FileDescriptor fd, PrintWriter pw, String[] args);

private:
    // CUserManagerService(Context context, PackageManagerService pm,
    //         Object installLock, Object packagesLock,
    //         File dataDir, File baseUserPath);
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IIPackageManager* pm,
        /* [in] */ IObject* installLock,
        /* [in] */ IObject* packagesLock,
        /* [in] */ IFile* dataDir,
        /* [in] */ IFile* baseUserPath);

    /** Assume permissions already checked and caller's identity cleared */
    CARAPI_(AutoPtr<IList>) GetProfilesLocked(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean enabledOnly);

    CARAPI_(Boolean) IsProfileOf(
        /* [in] */ IUserInfo* user,
        /* [in] */ IUserInfo* profile);

    CARAPI_(AutoPtr<IUserInfo>) GetUserInfoLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) SendUserInfoChangedBroadcast(
        /* [in] */ Int32 userId);

    /**
     * If default guest restrictions haven't been initialized yet, add the basic
     * restrictions.
     */
    CARAPI_(void) InitDefaultGuestRestrictions();

    CARAPI_(Boolean) IsUserLimitReachedLocked();

    /**
     * Enforces that only the system UID or root's UID or apps that have the
     * {@link android.Manifest.permission#MANAGE_USERS MANAGE_USERS}
     * permission can make certain calls to the UserManager.
     *
     * @param message used as message if SecurityException is thrown
     * @throws SecurityException if the caller is not system or root
     */
    static CARAPI CheckManageUsersPermission(
        /* [in] */ const String& message);

    CARAPI_(void) WriteBitmapLocked(
        /* [in] */ IUserInfo* info,
        /* [in] */ IBitmap* bitmap);

    CARAPI_(void) ReadUserListLocked();

    CARAPI_(void) UpgradeIfNecessaryLock();

    CARAPI_(void) FallbackToSingleUserLocked();

    CARAPI_(void) WriteUserLocked(
        /* [in] */ IUserInfo* userInfo);

    CARAPI_(void) WriteUserListLocked();

    CARAPI_(void) WriteRestrictionsLocked(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ IBundle* restrictions);

    CARAPI_(AutoPtr<IUserInfo>) ReadUserLocked(
        /* [in] */ Int32 id);

    CARAPI_(void) ReadRestrictionsLocked(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IBundle* restrictions);

    CARAPI_(void) ReadBoolean(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IBundle* restrictions,
        /* [in] */ const String& restrictionKey);

    CARAPI_(void) WriteBoolean(
        /* [in] */ IXmlSerializer* xml,
        /* [in] */ IBundle* restrictions,
        /* [in] */ const String& restrictionKey);

    CARAPI_(Int32) ReadInt32Attribute(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& attr,
        /* [in] */ Int32 defaultValue);

    CARAPI_(Int64) ReadInt64Attribute(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& attr,
        /* [in] */ Int64 defaultValue);

    CARAPI_(Boolean) IsPackageInstalled(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    /**
     * Removes all the restrictions files (res_<packagename>) for a given user.
     * Does not do any permissions checking.
     */
    CARAPI_(void) CleanAppRestrictions(
        /* [in] */ Int32 userId);

    /**
     * Removes the app restrictions file for a specific package and user id, if it exists.
     */
    CARAPI_(void) CleanAppRestrictionsForPackage(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IUserInfo>) CreateUserInternal(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 parentId);

    CARAPI_(Int32) NumberOfUsersOfTypeLocked(
        /* [in] */ Int32 flags,
        /* [in] */ Boolean excludeDying);

    /**
     * Find the current guest user. If the Guest user is partial,
     * then do not include it in the results as it is about to die.
     * This is different than {@link #numberOfUsersOfTypeLocked(int, boolean)} due to
     * the special handling of Guests being removed.
     */
    CARAPI_(AutoPtr<IUserInfo>) FindCurrentGuestUserLocked();

    CARAPI_(void) RemoveUserStateLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) RemoveDirectoryRecursive(
        /* [in] */ IFile* parent);

    CARAPI_(void) SendProfileRemovedBroadcast(
        /* [in] */ Int32 parentUserId,
        /* [in] */ Int32 removedUserId);

    CARAPI_(Int32) GetRemainingTimeForPinAttempt(
        /* [in] */ RestrictionsPinState* pinState);

    CARAPI_(Boolean) HasRestrictionsPinLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveRestrictionsForUser(
        /* [in] */ Int32 userHandle,
        /* [in] */ Boolean unhideApps);

    CARAPI_(void) UnhideAllInstalledAppsForUser(
        /* [in] */ Int32 userHandle);

    /*
     * Generate a hash for the given password. To avoid brute force attacks, we use a salted hash.
     * Not the most secure, but it is at least a second level of protection. First level is that
     * the file is in a location only readable by the system process.
     * @param password the password.
     * @param salt the randomly generated salt
     * @return the hash of the pattern in a String.
     */
    CARAPI_(String) PasswordToHash(
        /* [in] */ const String& password,
        /* [in] */ Int64 salt);

    static CARAPI_(String) ToHex(
        /* [in] */ ArrayOf<Byte>* ary);

    CARAPI_(Int32) GetUidForPackage(
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr<IBundle>) ReadApplicationRestrictionsLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI_(void) WriteApplicationRestrictionsLocked(
        /* [in] */ const String& packageName,
        /* [in] */ IBundle* restrictions,
        /* [in] */ Int32 userId);

    CARAPI_(void) UpdateUserIdsLocked();

    CARAPI_(Int32) GetNextAvailableIdLocked();

    CARAPI_(String) PackageToRestrictionsFileName(
        /* [in] */ const String& packageName);

    CARAPI_(String) RestrictionsFileNameToPackage(
        /* [in] */ const String& fileName);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    static const Boolean DBG = FALSE;

    static const String TAG_NAME;
    static const String ATTR_FLAGS;
    static const String ATTR_ICON_PATH;
    static const String ATTR_ID;
    static const String ATTR_CREATION_TIME;
    static const String ATTR_LAST_LOGGED_IN_TIME;
    static const String ATTR_SALT;
    static const String ATTR_PIN_HASH;
    static const String ATTR_FAILED_ATTEMPTS;
    static const String ATTR_LAST_RETRY_MS;
    static const String ATTR_SERIAL_NO;
    static const String ATTR_NEXT_SERIAL_NO;
    static const String ATTR_PARTIAL;
    static const String ATTR_GUEST_TO_REMOVE;
    static const String ATTR_USER_VERSION;
    static const String ATTR_PROFILE_GROUP_ID;
    static const String TAG_GUEST_RESTRICTIONS;
    static const String TAG_USERS;
    static const String TAG_USER;
    static const String TAG_RESTRICTIONS;
    static const String TAG_ENTRY;
    static const String TAG_VALUE;
    static const String ATTR_KEY;
    static const String ATTR_VALUE_TYPE;
    static const String ATTR_MULTIPLE;

    static const String ATTR_TYPE_STRING_ARRAY;
    static const String ATTR_TYPE_STRING;
    static const String ATTR_TYPE_BOOLEAN;
    static const String ATTR_TYPE_INTEGER;

    static const String USER_INFO_DIR;
    static const String USER_LIST_FILENAME;
    static const String USER_PHOTO_FILENAME;

    static const String RESTRICTIONS_FILE_PREFIX;
    static const String XML_SUFFIX;

    static const Int32 MIN_USER_ID = 10;

    static const Int32 USER_VERSION = 5;

    static const Int64 EPOCH_PLUS_30_YEARS = 30LL * 365 * 24 * 60 * 60 * 1000LL; // ms

    // Number of attempts before jumping to the next BACKOFF_TIMES slot
    static const Int32 BACKOFF_INC_INTERVAL = 5;

    // Maximum number of managed profiles permitted is 1. This cannot be increased
    // without first making sure that the rest of the framework is prepared for it.
    static const Int32 MAX_MANAGED_PROFILES = 1;

    // Amount of time to force the user to wait before entering the PIN again, after failing
    // BACKOFF_INC_INTERVAL times.
    static const AutoPtr<ArrayOf<Int32> > BACKOFF_TIMES;

    AutoPtr<IContext> mContext;
    CPackageManagerService* mPm;
    AutoPtr<IObject> mInstallLock;
    AutoPtr<IObject> mPackagesLock;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IFile> mUsersDir;
    AutoPtr<IFile> mUserListFile;
    AutoPtr<IFile> mBaseUserPath;

    HashMap<Int32, AutoPtr<IUserInfo> > mUsers;
    HashMap<Int32, AutoPtr<IBundle> > mUserRestrictions;
    AutoPtr<IBundle> mGuestRestrictions;

    HashMap<Int32, AutoPtr<RestrictionsPinState> > mRestrictionsPinStates;

    /**
     * Set of user IDs being actively removed. Removed IDs linger in this set
     * for several seconds to work around a VFS caching issue.
     */
    // @GuardedBy("mPackagesLock")
    HashMap<Int32, Boolean> mRemovingUserIds;// = new SparseBooleanArray();

    AutoPtr< ArrayOf<Int32> > mUserIds;
    Int32 mNextSerialNumber;

    Int32 mUserVersion;

    AutoPtr<IIAppOpsService> mAppOpsService;

    static AutoPtr<CUserManagerService> sInstance;
    static Object sLock;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_CUSERMANAGERSERVICE_H__
