
#ifndef __ELASTOS_DROID_CONTENT_PM_APPLICATIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_APPLICATIONINFO_H__

#include "PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular application.  This
 * corresponds to information collected from the AndroidManifest.xml's
 * &lt;application&gt; tag.
 */
class ApplicationInfo
    : public PackageItemInfo
    , public IApplicationInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ApplicationInfo();

    virtual ~ApplicationInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IApplicationInfo* orig);

    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    /**
     * @return true if "supportsRtl" has been set to true in the AndroidManifest
     * @hide
     */
    CARAPI HasRtlSupport(
        /* [out] */ Boolean* isSupport);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Retrieve the textual description of the application.  This
     * will call back on the given PackageManager to load the description from
     * the application.
     *
     * @param pm A PackageManager from which the label can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a CharSequence containing the application's description.
     * If there is no description, null is returned.
     */
    CARAPI LoadDescription(
            /* [in] */ IPackageManager* pm,
            /* [out] */ ICharSequence** label);

    CARAPI DisableCompatibilityMode();

     CARAPI GetTaskAffinity(
        /* [out] */ String* task);

    CARAPI SetTaskAffinity(
        /* [in] */ const String& task);

    CARAPI GetPermission(
        /* [out] */ String* permission);

    CARAPI SetPermission(
        /* [in] */ const String& permission);

    CARAPI GetProcessName(
        /* [out] */ String* name);

    CARAPI SetProcessName(
        /* [in] */ const String& name);

    CARAPI GetClassName(
        /* [out] */ String* name);

    CARAPI SetClassName(
        /* [in] */ const String& name);

    CARAPI GetDescriptionRes(
        /* [out] */ Int32* desRes);

    CARAPI SetDescriptionRes(
        /* [in] */ Int32 desRes);

    CARAPI GetTheme(
        /* [out] */ Int32* theme);

    CARAPI SetTheme(
        /* [in] */ Int32 theme);

    CARAPI GetManageSpaceActivityName(
        /* [out] */ String* name);

    CARAPI SetManageSpaceActivityName(
        /* [in] */ const String& name);

    CARAPI GetBackupAgentName(
        /* [out] */ String* name);

    CARAPI SetBackupAgentName(
        /* [in] */ const String& name);

    CARAPI GetUiOptions(
        /* [out] */ Int32* uiOptions);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetRequiresSmallestWidthDp(
        /* [out] */ Int32* requiresSmallestWidthDp);

    CARAPI SetRequiresSmallestWidthDp(
        /* [in] */ Int32 requiresSmallestWidthDp);

    CARAPI GetCompatibleWidthLimitDp(
        /* [out] */ Int32* compatibleWidthLimitDp);

    CARAPI SetCompatibleWidthLimitDp(
        /* [in] */ Int32 compatibleWidthLimitDp);

    CARAPI GetLargestWidthLimitDp(
        /* [out] */ Int32* largestWidthLimitDp);

    CARAPI SetLargestWidthLimitDp(
        /* [in] */ Int32 largestWidthLimitDp);

    CARAPI GetSourceDir(
        /* [out] */ String* sourceDir);

    CARAPI SetSourceDir(
        /* [in] */ const String& sourceDir);

    CARAPI GetPublicSourceDir(
        /* [out] */ String* publicSourceDir);

    CARAPI SetPublicSourceDir(
        /* [in] */ const String& pubicSourceDir);

    CARAPI GetResourceDirs(
        /* [out, callee] */ ArrayOf<String>** resourceDirs);

    CARAPI SetResourceDirs(
        /* [in] */ ArrayOf<String>* resourceDirs);

    CARAPI GetSharedLibraryFiles(
        /* [out, callee] */ ArrayOf<String>** sharedLibraryFiles);

    CARAPI SetSharedLibraryFiles(
        /* [in] */ ArrayOf<String>* sharedLibraryFiles);

    CARAPI GetDataDir(
        /* [out] */ String* dataDir);

    CARAPI SetDataDir(
        /* [in] */ const String& dataDir);

    CARAPI GetNativeLibraryDir(
        /* [out] */ String* libraryDir);

    CARAPI SetNativeLibraryDir(
        /* [in] */ const String& libraryDir);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

    CARAPI GetTargetSdkVersion(
        /* [out] */ Int32* sdkVersion);

    CARAPI SetTargetSdkVersion(
        /* [in] */ Int32 sdkVersion);

    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetEnabledSetting(
        /* [out] */ Int32* enabledSetting);

    CARAPI SetEnabledSetting(
        /* [in] */ Int32 enabledSetting);

    CARAPI GetInstallLocation(
        /* [out] */ Int32* location);

    CARAPI SetInstallLocation(
        /* [in] */ Int32 location);

    // @override
    CARAPI LoadDefaultIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /** {@hide} */ CARAPI SetCodePath(
        /* [in] */ const String& codePath);

    /** {@hide} */ CARAPI SetBaseCodePath(
        /* [in] */ const String& baseCodePath);

    /** {@hide} */ CARAPI SetSplitCodePaths(
        /* [in] */ ArrayOf<String>* splitCodePaths);

    /** {@hide} */ CARAPI SetResourcePath(
        /* [in] */ const String& resourcePath);

    /** {@hide} */ CARAPI SetBaseResourcePath(
        /* [in] */ const String& baseResourcePath);

    /** {@hide} */ CARAPI SetSplitResourcePaths(
        /* [in] */ ArrayOf<String>* splitResourcePaths);

    /** {@hide} */ CARAPI GetCodePath(
        /* [out] */ String* result);

    /** {@hide} */ CARAPI GetBaseCodePath(
        /* [out] */ String* result);

    /** {@hide} */ CARAPI GetSplitCodePaths(
        /* [out, callee] */ ArrayOf<String>** result);

    /** {@hide} */ CARAPI GetResourcePath(
        /* [out] */ String* result);

    /** {@hide} */ CARAPI GetBaseResourcePath(
        /* [out] */ String* result);

    /** {@hide} */ CARAPI GetSplitResourcePaths(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI GetSplitSourceDirs(
        /* [out, callee] */ ArrayOf<String>** resourceDirs);

    CARAPI SetSplitSourceDirs(
        /* [in] */ ArrayOf<String>* resourceDirs);

    CARAPI GetSplitPublicSourceDirs(
        /* [out, callee] */ ArrayOf<String>** resourceDirs);

    CARAPI SetSplitPublicSourceDirs(
        /* [in] */ ArrayOf<String>* resourceDirs);

    CARAPI GetPrimaryCpuAbi(
        /* [out] */ String* libraryDir);

    CARAPI SetPrimaryCpuAbi(
        /* [in] */ const String& libraryDir);

    CARAPI GetSecondaryCpuAbi(
        /* [out] */ String* libraryDir);

    CARAPI SetSecondaryCpuAbi(
        /* [in] */ const String& libraryDir);

    CARAPI GetVersionCode(
        /* [out] */ Int32* uid);

    CARAPI SetVersionCode(
        /* [in] */ Int32 uid);

    CARAPI SetScanSourceDir(
        /* [in] */ const String& value);

    CARAPI SetScanPublicSourceDir(
        /* [in] */ const String& value);

    CARAPI SetSeinfo(
        /* [in] */ const String& value);

    CARAPI SetSecondaryNativeLibraryDir(
        /* [in] */ const String& value);

    CARAPI SetNativeLibraryRootDir(
        /* [in] */ const String& value);

    CARAPI SetNativeLibraryRootRequiresIsa(
        /* [in] */ Boolean value);

    CARAPI GetScanSourceDir(
        /* [out */ String* value);

    CARAPI GetScanPublicSourceDir(
        /* [out */ String* value);

    CARAPI GetSeinfo(
        /* [out */ String* value);

    CARAPI GetSecondaryNativeLibraryDir(
        /* [out */ String* value);

    CARAPI GetNativeLibraryRootDir(
        /* [out */ String* value);

    CARAPI GetNativeLibraryRootRequiresIsa(
        /* [out */ Boolean* value);

    CARAPI GetIsThemeable(
        /* [out] */ Int32* isThemeable);

    CARAPI SetIsThemeable(
        /* [in] */ Int32 isThemeable);

    CARAPI GetProtect(
        /* [out] */ Boolean* protect);

    CARAPI SetProtect(
        /* [in] */ Boolean protect);

protected:

    // @override
    CARAPI_(AutoPtr<IApplicationInfo>) GetApplicationInfo();

private:
    CARAPI_(Boolean) IsPackageUnavailable(
        /* [in] */ IPackageManager* pm);

public:
    /**
     * Default task affinity of all activities in this application. See
     * {@link ActivityInfo#taskAffinity} for more information.  This comes
     * from the "taskAffinity" attribute.
     */
    String mTaskAffinity;

    /**
     * Optional name of a permission required to be able to access this
     * application's components.  From the "permission" attribute.
     */
    String mPermission;

    /**
     * The name of the process this application should run in.  From the
     * "process" attribute or, if not set, the same as
     * <var>packageName</var>.
     */
    String mProcessName;

    /**
     * Class implementing the Application object.  From the "class"
     * attribute.
     */
    String mClassName;

    /**
     * A style resource identifier (in the package's resources) of the
     * description of an application.  From the "description" attribute
     * or, if not set, 0.
     */
    Int32 mDescriptionRes;

    /**
     * A style resource identifier (in the package's resources) of the
     * default visual theme of the application.  From the "theme" attribute
     * or, if not set, 0.
     */
    Int32 mTheme;

    /**
     * Class implementing the Application's manage space
     * functionality.  From the "manageSpaceActivity"
     * attribute. This is an optional attribute and will be null if
     * applications don't specify it in their manifest
     */
    String mManageSpaceActivityName;

    /**
     * Class implementing the Application's backup functionality.  From
     * the "backupAgent" attribute.  This is an optional attribute and
     * will be null if the application does not specify it in its manifest.
     *
     * <p>If android:allowBackup is set to false, this attribute is ignored.
     */
    String mBackupAgentName;

    /**
     * The default extra UI options for activities in this application.
     * Set from the {@link android.R.attr#uiOptions} attribute in the
     * activity's manifest.
     */
    Int32 mUiOptions;

    /**
     * Flags associated with the application.  Any combination of
     * {@link #FLAG_SYSTEM}, {@link #FLAG_DEBUGGABLE}, {@link #FLAG_HAS_CODE},
     * {@link #FLAG_PERSISTENT}, {@link #FLAG_FACTORY_TEST}, and
     * {@link #FLAG_ALLOW_TASK_REPARENTING}
     * {@link #FLAG_ALLOW_CLEAR_USER_DATA}, {@link #FLAG_UPDATED_SYSTEM_APP},
     * {@link #FLAG_TEST_ONLY}, {@link #FLAG_SUPPORTS_SMALL_SCREENS},
     * {@link #FLAG_SUPPORTS_NORMAL_SCREENS},
     * {@link #FLAG_SUPPORTS_LARGE_SCREENS}, {@link #FLAG_SUPPORTS_XLARGE_SCREENS},
     * {@link #FLAG_RESIZEABLE_FOR_SCREENS},
     * {@link #FLAG_SUPPORTS_SCREEN_DENSITIES}, {@link #FLAG_VM_SAFE_MODE},
     * {@link #FLAG_INSTALLED}.
     */
    Int32 mFlags;

    /**
     * The required smallest screen width the application can run on.  If 0,
     * nothing has been specified.  Comes from
     * {@link android.R.styleable#AndroidManifestSupportsScreens_requiresSmallestWidthDp
     * android:requiresSmallestWidthDp} attribute of the &lt;supports-screens&gt; tag.
     */
    Int32 mRequiresSmallestWidthDp;

    /**
     * The maximum smallest screen width the application is designed for.  If 0,
     * nothing has been specified.  Comes from
     * {@link android.R.styleable#AndroidManifestSupportsScreens_compatibleWidthLimitDp
     * android:compatibleWidthLimitDp} attribute of the &lt;supports-screens&gt; tag.
     */
    Int32 mCompatibleWidthLimitDp;

    /**
     * The maximum smallest screen width the application will work on.  If 0,
     * nothing has been specified.  Comes from
     * {@link android.R.styleable#AndroidManifestSupportsScreens_largestWidthLimitDp
     * android:largestWidthLimitDp} attribute of the &lt;supports-screens&gt; tag.
     */
    Int32 mLargestWidthLimitDp;

    /** {@hide} */
    String mScanSourceDir;
    /** {@hide} */
    String mScanPublicSourceDir;

    /**
     * Full path to the base APK for this application.
     */
    String mSourceDir;

    /**
     * Full path to the publicly available parts of {@link #sourceDir},
     * including resources and manifest. This may be different from
     * {@link #sourceDir} if an application is forward locked.
     */
    String mPublicSourceDir;

    /**
     * Full paths to zero or more split APKs that, when combined with the base
     * APK defined in {@link #sourceDir}, form a complete application.
     */
    AutoPtr< ArrayOf<String> > mSplitSourceDirs;

    /**
     * Full path to the publicly available parts of {@link #splitSourceDirs},
     * including resources and manifest. This may be different from
     * {@link #splitSourceDirs} if an application is forward locked.
     */
    AutoPtr< ArrayOf<String> > mSplitPublicSourceDirs;

    /**
     * Full paths to the locations of extra resource packages this application
     * uses. This field is only used if there are extra resource packages,
     * otherwise it is null.
     *
     * {@hide}
     */
    AutoPtr< ArrayOf<String> > mResourceDirs;

    /**
     * String retrieved from the seinfo tag found in selinux policy. This value
     * is useful in setting an SELinux security context on the process as well
     * as its data directory.
     *
     * {@hide}
     */
    String mSeinfo;

    /**
     * Paths to all shared libraries this application is linked against.  This
     * field is only set if the {@link PackageManager#GET_SHARED_LIBRARY_FILES
     * PackageManager.GET_SHARED_LIBRARY_FILES} flag was used when retrieving
     * the structure.
     */
    AutoPtr< ArrayOf<String> > mSharedLibraryFiles;

    /**
     * Full path to a directory assigned to the package for its persistent
     * data.
     */
    String mDataDir;

    /**
     * Full path to the directory where native JNI libraries are stored.
     */
    String mNativeLibraryDir;

    /**
     * Full path where unpacked native libraries for {@link #secondaryCpuAbi}
     * are stored, if present.
     *
     * The main reason this exists is for bundled multi-arch apps, where
     * it's not trivial to calculate the location of libs for the secondary abi
     * given the location of the primary.
     *
     * TODO: Change the layout of bundled installs so that we can use
     * nativeLibraryRootDir & nativeLibraryRootRequiresIsa there as well.
     * (e.g {@code [ "/system/app-lib/Foo/arm", "/system/app-lib/Foo/arm64" ]}
     * instead of {@code [ "/system/lib/Foo", "/system/lib64/Foo" ]}.
     *
     * @hide
     */
    String mSecondaryNativeLibraryDir;

    /**
     * The root path where unpacked native libraries are stored.
     * <p>
     * When {@link #nativeLibraryRootRequiresIsa} is set, the libraries are
     * placed in ISA-specific subdirectories under this path, otherwise the
     * libraries are placed directly at this path.
     *
     * @hide
     */
    String mNativeLibraryRootDir;

    /**
     * Flag indicating that ISA must be appended to
     * {@link #nativeLibraryRootDir} to be useful.
     *
     * @hide
     */
    Boolean mNativeLibraryRootRequiresIsa;

    /**
     * The primary ABI that this application requires, This is inferred from the ABIs
     * of the native JNI libraries the application bundles. Will be {@code null}
     * if this application does not require any particular ABI.
     *
     * If non-null, the application will always be launched with this ABI.
     *
     * {@hide}
     */
    String mPrimaryCpuAbi;

    /**
     * The secondary ABI for this application. Might be non-null for multi-arch
     * installs. The application itself never uses this ABI, but other applications that
     * use its code might.
     *
     * {@hide}
     */
    String mSecondaryCpuAbi;

    /**
     * The kernel user-ID that has been assigned to this application;
     * currently this is not a unique ID (multiple applications can have
     * the same uid).
     */
    Int32 mUid;

    /**
     * The minimum SDK version this application targets.  It may run on earlier
     * versions, but it knows how to work with any new behavior added at this
     * version.  Will be {@link android.os.Build.VERSION_CODES#CUR_DEVELOPMENT}
     * if this is a development build and the app is targeting that.  You should
     * compare that this number is >= the SDK version number at which your
     * behavior was introduced.
     */
    Int32 mTargetSdkVersion;

    /**
     * The app's declared version code.
     * @hide
     */
    Int32 mVersionCode;

    /**
     * When false, indicates that all components within this application are
     * considered disabled, regardless of their individually set enabled status.
     */
    Boolean mEnabled;

    /**
     * For convenient access to the current enabled setting of this app.
     * @hide
     */
    Int32 mEnabledSetting;

    /**
     * For convenient access to package's install location.
     * @hide
     */
    Int32 mInstallLocation;

    /**
     * Is given application theme agnostic, i.e. behaves properly when default theme is changed.
     * {@hide}
     */
    Boolean mIsThemeable;

    /**
     * When true, indicates that any one component within this application is
     * protected.
     * @hide
     */
    Boolean mProtect;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_APPLICATIONINFO_H__
