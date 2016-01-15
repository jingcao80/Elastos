
#ifndef __ELASTOS_DROID_CONTENT_PM_PACKAGEMANAGER_H__
#define __ELASTOS_DROID_CONTENT_PM_PACKAGEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/PackageInstallObserver.h"
#include "elastos/droid/app/PackageDeleteObserver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::PackageInstallObserver;
using Elastos::Droid::App::PackageDeleteObserver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Class for retrieving various kinds of information related to the application
 * packages that are currently installed on the device.
 *
 * You can find this class through {@link Context#getPackageManager}.
 */
class PackageManager
    : public Object
    , public IPackageManager
{
public:
    CAR_INTERFACE_DECL()

    PackageManager();

    virtual ~PackageManager();

    /**
     * Returns an {@link Intent} suitable for passing to {@code startActivityForResult}
     * which prompts the user to grant {@code permissions} to this application.
     * @hide
     *
     * @throws NullPointerException if {@code permissions} is {@code null}.
     * @throws IllegalArgumentException if {@code permissions} contains {@code null}.
     */
    CARAPI BuildPermissionRequestIntent(
        /* [in] */ ArrayOf<String>* permissions,
        /* [out] */ IIntent** intent);

    /**
     * Retrieve overall information about an application package defined
     * in a package archive file
     *
     * @param archiveFilePath The path to the archive file
     * @param flags Additional option flags. Use any combination of
     * {@link #GET_ACTIVITIES},
     * {@link #GET_GIDS},
     * {@link #GET_CONFIGURATIONS},
     * {@link #GET_INSTRUMENTATION},
     * {@link #GET_PERMISSIONS},
     * {@link #GET_PROVIDERS},
     * {@link #GET_RECEIVERS},
     * {@link #GET_SERVICES},
     * {@link #GET_SIGNATURES}, to modify the data returned.
     *
     * @return Returns the information about the package. Returns
     * null if the package could not be successfully parsed.
     *
     * @see #GET_ACTIVITIES
     * @see #GET_GIDS
     * @see #GET_CONFIGURATIONS
     * @see #GET_INSTRUMENTATION
     * @see #GET_PERMISSIONS
     * @see #GET_PROVIDERS
     * @see #GET_RECEIVERS
     * @see #GET_SERVICES
     * @see #GET_SIGNATURES
     *
     */
    CARAPI GetPackageArchiveInfo(
        /* [in] */ const String& archiveFilePath,
        /* [in] */ Int32 flags,
        /* [out] */ IPackageInfo** info);

    /**
     * Like {@link #getPackageSizeInfo(String, int, IIPackageStatsObserver)}, but
     * returns the size for the calling user.
     *
     * @hide
     */
    CARAPI GetPackageSizeInfo(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageStatsObserver* observer);

    virtual CARAPI GetPackageSizeInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [in] */ IIPackageStatsObserver* observer) = 0;

    /**
     * Same as {@link #addPreferredActivity(IntentFilter, int,
            ComponentName[], ComponentName)}, but with a specific userId to apply the preference
            to.
     * @hide
     */
    CARAPI AddPreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    /**
     * Returns the data directory for a particular user and package, given the uid of the package.
     * @param uid uid of the package, including the userId and appId
     * @param packageName name of the package
     * @return the user-specific data directory for the package
     * @hide
     */
    static CARAPI_(String) GetDataDirForUser(
        /* [in] */ Int32 userId,
        /* [in] */ const String& packageName);

    /**
     * @hide
     */
    // @Deprecated
    CARAPI ReplacePreferredActivityAsUser(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    /**
     * Puts the package in a hidden state, which is almost like an uninstalled state,
     * making the package unavailable, but it doesn't remove the data or the actual
     * package file. Application can be unhidden by either resetting the hidden state
     * or by installing it, such as with {@link #installExistingPackage(String)}
     * @hide
     */
    virtual CARAPI SetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean hidden,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result) = 0;
    /**
     * Returns the hidden state of a package.
     * @see #setApplicationHiddenSettingAsUser(String, Boolean, UserHandle)
     * @hide
     */
    virtual CARAPI GetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result) = 0;

    /**
     * Return the {@link KeySet} associated with the String alias for this
     * application.
     *
     * @param alias The alias for a given {@link KeySet} as defined in the
     *        application's AndroidManifest.xml.
     * @hide
     */
    virtual CARAPI GetKeySetByAlias(
        /* [in] */ const String& packageName,
        /* [in] */ const String& alias,
        /* [out] */ IKeySet** ks) = 0;

    /** Return the signing {@link KeySet} for this application.
     * @hide
     */
    virtual CARAPI GetSigningKeySet(
        /* [in] */ const String& packageName,
        /* [out] */ IKeySet** ks) = 0;

    /**
     * Return whether the package denoted by packageName has been signed by all
     * of the keys specified by the {@link KeySet} ks.  This will return true if
     * the package has been signed by additional keys (a superset) as well.
     * Compare to {@link #isSignedByExactly(String packageName, KeySet ks)}.
     * @hide
     */
    virtual CARAPI IsSignedBy(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result) = 0;

    /**
     * Return whether the package denoted by packageName has been signed by all
     * of, and only, the keys specified by the {@link KeySet} ks. Compare to
     * {@link #isSignedBy(String packageName, KeySet ks)}.
     * @hide
     */
    virtual CARAPI IsSignedByExactly(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result) = 0;

    /**
     * @hide
     */
    virtual CARAPI LoadItemIcon(
        /* [in] */ IPackageItemInfo* itemInfo,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ IDrawable** drawable) = 0;

    /** {@hide} */
    virtual CARAPI IsPackageAvailable(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result) = 0;

    /** {@hide} */
    static CARAPI_(String) InstallStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg);

    /** {@hide} */
    static CARAPI_(String) InstallStatusToString(
        /* [in] */ Int32 status);

    /** {@hide} */
    static CARAPI_(Int32) InstallStatusToPublicStatus(
        /* [in] */ Int32 status);

    /** {@hide} */
    static CARAPI_(String) DeleteStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg);

    /** {@hide} */
    static CARAPI_(String) DeleteStatusToString(
        /* [in] */ Int32 status);

    /** {@hide} */
    static CARAPI_(Int32) DeleteStatusToPublicStatus(
        /* [in] */ Int32 status);

public:
    /** {@hide} */
    class LegacyPackageInstallObserver
        : public PackageInstallObserver
    {
    private:
        AutoPtr<IIPackageInstallObserver> mLegacy;

    public:
        LegacyPackageInstallObserver(
            /* [in] */ IIPackageInstallObserver* legacy)
            : mLegacy(legacy)
        {}

        CARAPI OnPackageInstalled(
            /* [in] */ const String& basePackageName,
            /* [in] */ Int32 returnCode,
            /* [in] */ const String& msg,
            /* [in] */ IBundle* extras)
        {
            if (mLegacy == NULL) return NOERROR;
            // try {
                return mLegacy->PackageInstalled(basePackageName, returnCode);
            // } catch (RemoteException ignored) {
            // }
        }
    };

    /** {@hide} */
    class LegacyPackageDeleteObserver
        : public PackageDeleteObserver
    {
    public:
        CARAPI constructor(
            /* [in] */ IIPackageDeleteObserver* legacy);

        CARAPI OnPackageDeleted(
            /* [in] */ const String& basePackageName,
            /* [in] */ Int32 returnCode,
            /* [in] */ const String& msg);

    private:
        AutoPtr<IIPackageDeleteObserver> mLegacy;
    };

public:
    static const String TAG;

};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_PACKAGEMANAGER_H__
