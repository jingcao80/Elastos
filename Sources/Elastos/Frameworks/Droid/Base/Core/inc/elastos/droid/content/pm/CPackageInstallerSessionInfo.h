
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_INFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_INFO_H__

#include "_Elastos_Droid_Content_Pm_CPackageInstallerSessionInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Details for an active install session.
 */
CarClass(CPackageInstallerSessionInfo)
    , Object
    , IPackageInstallerSessionInfo
    , IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageInstallerSessionInfo();

    virtual ~CPackageInstallerSessionInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Return the ID for this session.
     */
    CARAPI GetSessionId(
        /* [out] */ Int32* id);

    /**
     * Return the package name of the app that owns this session.
     */
    CARAPI GetInstallerPackageName(
        /* [out] */ String* name);

    /**
     * Return current overall progress of this session, between 0 and 1.
     * <p>
     * Note that this progress may not directly correspond to the value
     * reported by
     * {@link PackageInstaller.Session#setStagingProgress(float)}, as the
     * system may carve out a portion of the overall progress to represent
     * its own internal installation work.
     */
    CARAPI GetProgress(
        /* [out] */ Float* progress);

    /**
     * Return if this session is currently active.
     * <p>
     * A session is considered active whenever there is ongoing forward
     * progress being made, such as the installer holding an open
     * {@link Session} instance while streaming data into place, or the
     * system optimizing code as the result of
     * {@link Session#commit(IntentSender)}.
     * <p>
     * If the installer closes the {@link Session} without committing, the
     * session is considered inactive until the installer opens the session
     * again.
     */
    CARAPI IsActive(
        /* [out] */ Boolean* active);

    CARAPI IsOpen(
        /* [out] */ Boolean* open);

    /**
     * Return the package name this session is working with. May be {@code null}
     * if unknown.
     */
    CARAPI GetAppPackageName(
        /* [out] */ String* name);

    /**
     * Return an icon representing the app being installed. May be {@code null}
     * if unavailable.
     */
    CARAPI GetAppIcon(
        /* [out] */ IBitmap** icon);

    /**
     * Return a label representing the app being installed. May be {@code null}
     * if unavailable.
     */
    CARAPI GetAppLabel(
        /* [out] */ ICharSequence** label);

    /**
     * Return an Intent that can be started to view details about this install
     * session. This may surface actions such as pause, resume, or cancel.
     * <p>
     * In some cases, a matching Activity may not exist, so ensure you safeguard
     * against this.
     *
     * @see PackageInstaller#ACTION_SESSION_DETAILS
     */
    CARAPI CreateDetailsIntent(
        /* [out] */ IIntent** intent);

    /** {@hide} */
    CARAPI GetDetailsIntent(
        /* [out] */ IIntent** intent);

private:
    /** {@hide} */
    Int32 mSessionId;
    /** {@hide} */
    String mInstallerPackageName;
    /** {@hide} */
    String mResolvedBaseCodePath;
    /** {@hide} */
    Float mProgress;
    /** {@hide} */
    Boolean mSealed;
    /** {@hide} */
    Boolean mActive;

    /** {@hide} */
    Int32 mMode;
    /** {@hide} */
    Int64 mSizeBytes;
    /** {@hide} */
    String mAppPackageName;
    /** {@hide} */
    AutoPtr<IBitmap> mAppIcon;
    /** {@hide} */
    AutoPtr<ICharSequence> mAppLabel;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_INFO_H__
