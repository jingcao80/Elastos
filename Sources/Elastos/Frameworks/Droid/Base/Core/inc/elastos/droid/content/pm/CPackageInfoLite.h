
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFOLITE_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFOLITE_H__

#include "_Elastos_Droid_Content_Pm_CPackageInfoLite.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Basic information about a package as specified in its manifest.
 * Utility class used in PackageManager methods
 * @hide
 */
CarClass(CPackageInfoLite)
    , public Object
    , public IPackageInfoLite
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageInfoLite();

    ~CPackageInfoLite();

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI SetPackageName(
        /* [in] */ const String& name);

    CARAPI GetVersionCode(
        /* [out] */ Int32* versionCode);

    CARAPI SetVersionCode(
        /* [in] */ Int32 versionCode);

    CARAPI GetMultiArch(
        /* [out] */ Boolean* versionCode);

    CARAPI SetMultiArch(
        /* [in] */ Boolean versionCode);

    CARAPI GetRecommendedInstallLocation(
        /* [out] */ Int32* location);

    CARAPI SetRecommendedInstallLocation(
        /* [in] */ Int32 location);

    CARAPI GetInstallLocation(
        /* [out] */ Int32* location);

    CARAPI SetInstallLocation(
        /* [in] */ Int32 location);

    CARAPI GetVerifiers(
        /* [out, callee] */ ArrayOf<IVerifierInfo*>** verifiers);

    CARAPI SetVerifiers(
        /* [in] */ ArrayOf<IVerifierInfo*>* verifiers);

    CARAPI GetIsTheme(
        /* [out] */ Boolean* isTheme);

    CARAPI SetIsTheme(
        /* [in] */ Boolean isTheme);

public:
    /**
     * The name of this package.  From the &lt;manifest&gt; tag's "name"
     * attribute.
     */
    String mPackageName;

    /**
     * The android:versionCode of the package.
     */
    Int32 mVersionCode;

    /**
     * The android:multiArch flag from the package manifest. If set,
     * we will extract all native libraries for the given app, not just those
     * from the preferred ABI.
     */
    Boolean mMultiArch;

    /**
     * Specifies the recommended install location. Can be one of
     * {@link #PackageHelper.RECOMMEND_INSTALL_INTERNAL} to install on internal storage
     * {@link #PackageHelper.RECOMMEND_INSTALL_EXTERNAL} to install on external media
     * {@link PackageHelper.RECOMMEND_FAILED_INSUFFICIENT_STORAGE} for storage errors
     * {@link PackageHelper.RECOMMEND_FAILED_INVALID_APK} for parse errors.
     */
    Int32 mRecommendedInstallLocation;

    Int32 mInstallLocation;

    Boolean mIsTheme;

    AutoPtr< ArrayOf<IVerifierInfo*> > mVerifiers;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGEINFOLITE_H__
