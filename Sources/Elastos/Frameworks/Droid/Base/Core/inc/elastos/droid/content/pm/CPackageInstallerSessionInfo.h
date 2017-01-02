//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    CARAPI SetSessionId(
        /* [in] */ Int32 id);

    /**
     * Return the ID for this session.
     */
    CARAPI GetSessionId(
        /* [out] */ Int32* id);

    CARAPI SetInstallerPackageName(
        /* [in] */ const String& packageName);

    /**
     * Return the package name of the app that owns this session.
     */
    CARAPI GetInstallerPackageName(
        /* [out] */ String* name);

    CARAPI GetResolvedBaseCodePath(
        /* [out] */ String* path);

    CARAPI SetResolvedBaseCodePath(
        /* [in] */ const String& path);

    CARAPI SetProgress(
        /* [in] */ Float progress);

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

    CARAPI SetSealed(
        /* [in] */ Boolean sealed);

    CARAPI IsSealed(
        /* [out] */ Boolean* isSealed);

    CARAPI SetActive(
        /* [in] */ Boolean active);

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

    CARAPI SetMode(
        /* [in] */ Int32 mode);

    CARAPI GetMode(
        /* [out] */ Int32* mode);

    CARAPI SetSizeBytes(
        /* [in] */ Int64 sizeBytes);

    CARAPI GetSizeBytes(
        /* [out] */ Int64* sizeBytes);

    CARAPI SetAppPackageName(
        /* [in] */ const String& name);

    /**
     * Return the package name this session is working with. May be {@code null}
     * if unknown.
     */
    CARAPI GetAppPackageName(
        /* [out] */ String* name);

    CARAPI SetAppIcon(
        /* [in] */ IBitmap* icon);

    /**
     * Return an icon representing the app being installed. May be {@code null}
     * if unavailable.
     */
    CARAPI GetAppIcon(
        /* [out] */ IBitmap** icon);

    CARAPI SetAppLabel(
        /* [in] */ ICharSequence* label);

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
