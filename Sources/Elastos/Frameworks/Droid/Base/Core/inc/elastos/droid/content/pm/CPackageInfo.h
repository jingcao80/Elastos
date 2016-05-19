
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFO_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Content_Pm_CPackageInfo.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {


/**
 * Overall information about the contents of a package.  This corresponds
 * to all of the information collected from AndroidManifest.xml.
 */
CarClass(CPackageInfo)
    , public Object
    , public IPackageInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageInfo();

    virtual ~CPackageInfo();

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI SetSplitNames(
        /* [in] */ ArrayOf<String>* names);

    CARAPI GetSplitNames(
        /* [out, callee] */ ArrayOf<String>** names);

    CARAPI GetVersionCode(
        /* [out] */ Int32* versionCode);

    CARAPI SetVersionCode(
        /* [in] */ Int32 versionCode);

    CARAPI GetVersionName(
        /* [out] */ String* versionName);

    CARAPI SetVersionName(
        /* [in] */ const String& versionName);

    CARAPI GetSharedUserId(
        /* [out] */ String* id);

    CARAPI SetSharedUserId(
        /* [in] */ const String& id);

    CARAPI GetSharedUserLabel(
        /* [out] */ Int32* label);

    CARAPI SetSharedUserLabel(
        /* [in] */ Int32 label);

    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    CARAPI SetApplicationInfo(
        /* [in] */ IApplicationInfo* info);

    CARAPI GetFirstInstallTime(
        /* [out] */ Int64* time);

    CARAPI SetFirstInstallTime(
        /* [in] */ Int64 time);

    CARAPI GetLastUpdateTime(
        /* [out] */ Int64* time);

    CARAPI SetLastUpdateTime(
        /* [in] */ Int64 time);

    CARAPI GetGids(
        /* [out, callee] */ ArrayOf<Int32>** gids);

    CARAPI SetGids(
        /* [in] */ ArrayOf<Int32>* gids);

    CARAPI GetActivities(
        /* [out, callee] */ ArrayOf<IActivityInfo*>** activities);

    CARAPI SetActivities(
        /* [in] */ ArrayOf<IActivityInfo*>* activities);

    CARAPI GetReceivers(
        /* [out, callee] */ ArrayOf<IActivityInfo*>** receivers);

    CARAPI SetReceivers(
        /* [in] */ ArrayOf<IActivityInfo*>* receivers);

    CARAPI GetServices(
        /* [out, callee] */ ArrayOf<IServiceInfo*>** services);

    CARAPI SetServices(
        /* [in] */ ArrayOf<IServiceInfo*>* services);

    CARAPI GetProviders(
        /* [out, callee] */ ArrayOf<IProviderInfo*>** providers);

    CARAPI SetProviders(
        /* [in] */ ArrayOf<IProviderInfo*>* providers);

    CARAPI GetInstrumentation(
        /* [out, callee] */ ArrayOf<IInstrumentationInfo*>** instrumentation);

    CARAPI SetInstrumentation(
        /* [in] */ ArrayOf<IInstrumentationInfo*>* instrumentation);

    CARAPI GetPermissions(
        /* [out, callee] */ ArrayOf<IPermissionInfo*>** permissions);

    CARAPI SetPermissions(
        /* [in] */ ArrayOf<IPermissionInfo*>* permissions);

    CARAPI GetRequestedPermissions(
        /* [out, callee] */ ArrayOf<String>** requestedPermissions);

    CARAPI SetRequestedPermissions(
        /* [in] */ ArrayOf<String>* requestedPermissions);

    CARAPI GetRequestedPermissionsFlags(
        /* [out, callee] */ ArrayOf<Int32>** requestedPermissionsFlags);

    CARAPI SetRequestedPermissionsFlags(
        /* [in] */ ArrayOf<Int32>* requestedPermissionsFlags);

    CARAPI GetSignatures(
        /* [out, callee] */ ArrayOf<ISignature*>** signatures);

    CARAPI SetSignatures(
        /* [in] */ ArrayOf<ISignature*>* signatures);

    CARAPI GetConfigPreferences(
        /* [out, callee] */ ArrayOf<IConfigurationInfo*>** configPreferences);

    CARAPI SetConfigPreferences(
        /* [in] */ ArrayOf<IConfigurationInfo*>* configPreferences);

    CARAPI GetReqFeatures(
        /* [out, callee] */ ArrayOf<IFeatureInfo*>** reqFeatures);

    CARAPI SetReqFeatures(
        /* [in] */ ArrayOf<IFeatureInfo*>* reqFeatures);

    CARAPI GetFeatureGroups(
        /* [out, callee] */ ArrayOf<IFeatureGroupInfo*>** featureGroups);

    CARAPI SetFeatureGroups(
        /* [in] */ ArrayOf<IFeatureGroupInfo*>* featureGroups);

    CARAPI GetInstallLocation(
        /* [out] */ Int32* installLocation);

    CARAPI SetInstallLocation(
        /* [in] */ Int32 installLocation);

    CARAPI GetIsLegacyIconPackPkg(
        /* [out] */ Boolean* result);

    CARAPI SetIsLegacyIconPackPkg(
        /* [in] */ Boolean isLegacyIconPackPkg);

    /** @hide */
    CARAPI GetRequiredAccountType(
        /* [out] */ String* type);

    /** @hide */
    CARAPI SetRequiredAccountType(
        /* [in] */ const String& type);

    /** @hide */
    CARAPI GetRestrictedAccountType(
        /* [out] */ String* type);

    /** @hide */
    CARAPI SetRestrictedAccountType(
        /* [in] */ const String& type);

    /**
     * {@hide}
     */
    CARAPI GetIsThemeApk(
        /* [out] */ Boolean* isThemeApk);

    /**
     * {@hide}
     */
    CARAPI SetIsThemeApk(
        /* [in] */ Boolean isThemeApk);

    /**
     * {@hide}
     */
    CARAPI GetHasIconPack(
        /* [out] */ Boolean* hasIconPack);

    /**
     * {@hide}
     */
    CARAPI SetHasIconPack(
        /* [in] */ Boolean hasIconPack);

    /**
     * {@hide}
     */
    CARAPI GetOverlayTargets(
        /* [out] */ IArrayList** targets);

    /**
     * {@hide}
     */
    CARAPI SetOverlayTargets(
        /* [in] */ IArrayList* targets);

    /**
     * {@hide}
     */
    CARAPI GetIsLegacyIconPackApk(
        /* [out] */ Boolean* isLegacyIconPackApk);

    /**
     * {@hide}
     */
    CARAPI SetIsLegacyIconPackApk(
        /* [in] */ Boolean isLegacyIconPackApk);

    /**
     * {@hide}
     */
    CARAPI GetThemeInfo(
        /* [out] */ IThemeInfo** themeInfo);

    /**
     * {@hide}
     */
    CARAPI SetThemeInfo(
        /* [in] */ IThemeInfo* themeInfo);

    CARAPI GetCoreApp(
        /* [out] */ Boolean* coreApp);

    CARAPI SetCoreApp(
        /* [in] */ Boolean coreApp);

    CARAPI GetRequiredForAllUsers(
        /* [out] */ Boolean* requiredForAllUsers);

    CARAPI SetRequiredForAllUsers(
        /* [in] */ Boolean requiredForAllUsers);

    CARAPI GetOverlayTarget(
        /* [out] */ String* overlayTarget);

    CARAPI SetOverlayTarget(
        /* [in] */ const String& overlayTarget);

public:
    /**
     * The name of this package.  From the &lt;manifest&gt; tag's "name"
     * attribute.
     */
    String mPackageName;

    /**
     * The names of any installed split APKs for this package.
     */
    AutoPtr<ArrayOf<String> > mSplitNames;

    /**
     * The version number of this package, as specified by the &lt;manifest&gt;
     * tag's {@link android.R.styleable#AndroidManifest_versionCode versionCode}
     * attribute.
     */
    Int32 mVersionCode;

    /**
     * The version name of this package, as specified by the &lt;manifest&gt;
     * tag's {@link android.R.styleable#AndroidManifest_versionName versionName}
     * attribute.
     */
    String mVersionName;

    /**
     * The shared user ID name of this package, as specified by the &lt;manifest&gt;
     * tag's {@link android.R.styleable#AndroidManifest_sharedUserId sharedUserId}
     * attribute.
     */
    String mSharedUserId;

    /**
     * The shared user ID label of this package, as specified by the &lt;manifest&gt;
     * tag's {@link android.R.styleable#AndroidManifest_sharedUserLabel sharedUserLabel}
     * attribute.
     */
    Int32 mSharedUserLabel;

    /**
     * Information collected from the &lt;application&gt; tag, or null if
     * there was none.
     */
    AutoPtr<IApplicationInfo> mApplicationInfo;

    /**
     * The time at which the app was first installed.  Units are as
     * per {@link System#currentTimeMillis()}.
     */
    Int64 mFirstInstallTime;

    /**
     * The time at which the app was last updated.  Units are as
     * per {@link System#currentTimeMillis()}.
     */
    Int64 mLastUpdateTime;

    /**
     * All kernel group-IDs that have been assigned to this package.
     * This is only filled in if the flag {@link PackageManager#GET_GIDS} was set.
     */
    AutoPtr< ArrayOf<Int32> > mGids;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestActivity
     * &lt;activity&gt;} tags included under &lt;application&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_ACTIVITIES} was set.
     */
    AutoPtr< ArrayOf<IActivityInfo*> > mActivities;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestReceiver
     * &lt;receiver&gt;} tags included under &lt;application&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_RECEIVERS} was set.
     */
    AutoPtr< ArrayOf<IActivityInfo*> > mReceivers;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestService
     * &lt;service&gt;} tags included under &lt;application&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_SERVICES} was set.
     */
    AutoPtr< ArrayOf<IServiceInfo*> > mServices;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestProvider
     * &lt;provider&gt;} tags included under &lt;application&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_PROVIDERS} was set.
     */
    AutoPtr< ArrayOf<IProviderInfo*> > mProviders;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestInstrumentation
     * &lt;instrumentation&gt;} tags included under &lt;manifest&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_INSTRUMENTATION} was set.
     */
    AutoPtr< ArrayOf<IInstrumentationInfo*> > mInstrumentation;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestPermission
     * &lt;permission&gt;} tags included under &lt;manifest&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_PERMISSIONS} was set.
     */
    AutoPtr< ArrayOf<IPermissionInfo*> > mPermissions;

    /**
     * Array of all {@link android.R.styleable#AndroidManifestUsesPermission
     * &lt;uses-permission&gt;} tags included under &lt;manifest&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_PERMISSIONS} was set.  This list includes
     * all permissions requested, even those that were not granted or known
     * by the system at install time.
     */
    AutoPtr< ArrayOf<String> > mRequestedPermissions;

    /**
     * Array of flags of all {@link android.R.styleable#AndroidManifestUsesPermission
     * &lt;uses-permission&gt;} tags included under &lt;manifest&gt;,
     * or null if there were none.  This is only filled in if the flag
     * {@link PackageManager#GET_PERMISSIONS} was set.  Each value matches
     * the corresponding entry in {@link #requestedPermissions}, and will have
     * the flags {@link #REQUESTED_PERMISSION_REQUIRED} and
     * {@link #REQUESTED_PERMISSION_GRANTED} set as appropriate.
     */
    AutoPtr< ArrayOf<Int32> > mRequestedPermissionsFlags;

    /**
     * Array of all signatures read from the package file.  This is only filled
     * in if the flag {@link PackageManager#GET_SIGNATURES} was set.
     */
    AutoPtr< ArrayOf<ISignature*> > mSignatures;

    /**
     * Application specified preferred configuration
     * {@link android.R.styleable#AndroidManifestUsesConfiguration
     * &lt;uses-configuration&gt;} tags included under &lt;manifest&gt;,
     * or null if there were none. This is only filled in if the flag
     * {@link PackageManager#GET_CONFIGURATIONS} was set.
     */
    AutoPtr< ArrayOf<IConfigurationInfo*> > mConfigPreferences;

    /**
     * The features that this application has said it requires.
     */
    AutoPtr< ArrayOf<IFeatureInfo*> > mReqFeatures;

   /**
     * Groups of features that this application has requested.
     * Each group contains a set of features that are required.
     * A device must match the features listed in {@link #reqFeatures} and one
     * or more FeatureGroups in order to have satisfied the feature requirement.
     *
     * @see FeatureInfo#FLAG_REQUIRED
     */
    AutoPtr<ArrayOf<IFeatureGroupInfo*> > mFeatureGroups;

    /**
     * The install location requested by the activity.  From the
     * {@link android.R.attr#installLocation} attribute, one of
     * {@link #INSTALL_LOCATION_AUTO},
     * {@link #INSTALL_LOCATION_INTERNAL_ONLY},
     * {@link #INSTALL_LOCATION_PREFER_EXTERNAL}
     * @hide
     */
    Int32 mInstallLocation;

    /** @hide */
    Boolean mCoreApp;

    // Is Theme Apk
    /**
     * {@hide}
     */
    Boolean mIsThemeApk;

    /**
     * {@hide}
     */
    Boolean mHasIconPack;

    /**
     * {@hide}
     */
    AutoPtr<IArrayList> mOverlayTargets;

    // Is Legacy Icon Apk
    /**
     * {@hide}
     */
    Boolean mIsLegacyIconPackApk;

    // ThemeInfo
    /**
     * {@hide}
     */
    AutoPtr<IThemeInfo> mThemeInfo;

    /** @hide */
    Boolean mRequiredForAllUsers;

    /** @hide */
    String mRestrictedAccountType;

    /** @hide */
    String mRequiredAccountType;

    /**
     * What package, if any, this package will overlay.
     *
     * Package name of target package, or null.
     * @hide
     */
    String mOverlayTarget;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFO_H__
