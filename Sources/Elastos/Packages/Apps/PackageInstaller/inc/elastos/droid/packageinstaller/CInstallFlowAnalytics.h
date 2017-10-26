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

#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLFLOWANALYTICS_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLFLOWANALYTICS_H__

#include "_Elastos_Droid_PackageInstaller_CInstallFlowAnalytics.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include "elastos/core/Object.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::IRunnable;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CarClass(CInstallFlowAnalytics)
    , public Object
    , public IParcelable
{
private:
    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable(
            /* [in] */ CInstallFlowAnalytics* owner);

        // @Override
        CARAPI Run();

    private:
        CInstallFlowAnalytics* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInstallFlowAnalytics();

    CARAPI constructor();

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    virtual CARAPI SetContext(
        /* [in] */ IContext* context);

    /** Sets whether the Unknown Sources setting is checked. */
    virtual CARAPI SetInstallsFromUnknownSourcesPermitted(
        /* [in] */ Boolean permitted);

    /** Sets whether this install attempt is from an unknown source. */
    virtual CARAPI SetInstallRequestFromUnknownSource(
        /* [in] */ Boolean unknownSource);

    /** Sets whether app verification is enabled. */
    virtual CARAPI SetVerifyAppsEnabled(
        /* [in] */ Boolean enabled);

    /** Sets whether at least one app verifier is installed. */
    virtual CARAPI SetAppVerifierInstalled(
        /* [in] */ Boolean installed);

    /**
      * Sets whether an APK file is being installed.
      *
      * @param fileUri {@code true} if an APK file is being installed, {@code false} if an already
      *        installed package is being installed to this user profile.
      */
    virtual CARAPI SetFileUri(
        /* [in] */ Boolean fileUri);

    /**
      * Sets the URI of the package being installed.
      */
    virtual CARAPI SetPackageUri(
        /* [in] */ const String& packageUri);

    /** Sets whether this is an attempt to replace an existing package. */
    virtual CARAPI SetReplace(
        /* [in] */ Boolean replace);

    /** Sets whether the package being updated is a system package. */
    virtual CARAPI SetSystemApp(
        /* [in] */ Boolean systemApp);

    /**
      * Sets whether the package being installed is requesting more permissions than the already
      * installed version of the package.
      */
    virtual CARAPI SetNewPermissionsFound(
        /* [in] */ Boolean found);

    /** Sets whether permissions were displayed to the user. */
    virtual CARAPI SetPermissionsDisplayed(
        /* [in] */ Boolean displayed);

    /**
      * Sets whether new permissions were displayed to the user (if permissions were displayed at
      * all).
      */
    virtual CARAPI SetNewPermissionsDisplayed(
        /* [in] */ Boolean displayed);

    /**
      * Sets whether all permissions were displayed to the user (if permissions were displayed at
      * all).
      */
    virtual CARAPI SetAllPermissionsDisplayed(
        /* [in] */ Boolean displayed);

    /**
      * Sets the time instant when the installation request arrived, measured in elapsed realtime
      * milliseconds. See {@link SystemClock#elapsedRealtime()}.
      */
    virtual CARAPI SetStartTimestampMillis(
        /* [in] */ Int64 timestampMillis);

    /**
      * Records that the information about the package info has been obtained or that there has been
      * a failure to obtain the information.
      */
    virtual CARAPI SetPackageInfoObtained();

    /**
      * Records that the Install button has been clicked.
      */
    virtual CARAPI SetInstallButtonClicked();

    /**
      * Marks this flow as finished due to {@code PackageManager} succeeding or failing to install
      * the package and reports this to the Event Log.
      */
    virtual CARAPI SetFlowFinishedWithPackageManagerResult(
        /* [in] */ Int32 packageManagerResult);

    /**
      * Marks this flow as finished and reports this to the Event Log.
      */
    virtual CARAPI SetFlowFinished(
        /* [in] */ Byte result);

private:
    static CARAPI_(void) WriteBoolean(
        /* [in] */ IParcel* dest,
        /* [in] */ Boolean value);

    static CARAPI_(Boolean) ReadBoolean(
        /* [in] */ IParcel* dest);

    /** Gets whether the Unknown Sources setting is checked. */
    CARAPI_(Boolean) IsInstallsFromUnknownSourcesPermitted();

    /** Gets whether this install attempt is from an unknown source. */
    CARAPI_(Boolean) IsInstallRequestFromUnknownSource();

    /** Gets whether app verification is enabled. */
    CARAPI_(Boolean) IsVerifyAppsEnabled();

    /** Gets whether at least one app verifier is installed. */
    CARAPI_(Boolean) IsAppVerifierInstalled();

    /**
      * Gets whether an APK file is being installed.
      *
      * @return {@code true} if an APK file is being installed, {@code false} if an already
      *         installed package is being installed to this user profile.
      */
    CARAPI_(Boolean) IsFileUri();

    /** Gets whether this is an attempt to replace an existing package. */
    CARAPI_(Boolean) IsReplace();

    /** Gets whether the package being updated is a system package. */
    CARAPI_(Boolean) IsSystemApp();

    /**
      * Gets whether the package being installed is requesting more permissions than the already
      * installed version of the package.
      */
    CARAPI_(Boolean) IsNewPermissionsFound();

    /** Gets whether permissions were displayed to the user. */
    CARAPI_(Boolean) IsPermissionsDisplayed();

    /**
      * Gets whether new permissions were displayed to the user (if permissions were displayed at
      * all).
      */
    CARAPI_(Boolean) IsNewPermissionsDisplayed();

    /**
      * Gets whether all permissions were displayed to the user (if permissions were displayed at
      * all).
      */
    CARAPI_(Boolean) IsAllPermissionsDisplayed();

    /**
      * Checks whether the information about the package info has been obtained or that there has
      * been a failure to obtain the information.
      */
    CARAPI_(Boolean) IsPackageInfoObtained();

    /**
      * Checks whether the Install button has been clicked.
      */
    CARAPI_(Boolean) IsInstallButtonClicked();

    CARAPI_(void) WriteToEventLog();

    static CARAPI_(Byte) ClipUnsignedValueToUnsignedByte(
        /* [in] */ Int64 value);

    static CARAPI_(Int32) ClipUnsignedLongToUnsignedInt(
        /* [in] */ Int64 value);

    /**
      * Sets or clears the specified flag in the {@link #mFlags} field.
      */
    CARAPI_(void) SetFlagState(
        /* [in] */ Int32 flag,
        /* [in] */ Boolean set);

    /**
      * Checks whether the specified flag is set in the {@link #mFlags} field.
      */
    CARAPI_(Boolean) IsFlagSet(
        /* [in] */ Int32 flag);

    /**
      * Checks whether the user has consented to app verification.
      */
    CARAPI_(Boolean) IsUserConsentToVerifyAppsGranted();

    /**
      * Gets the digest of the contents of the package being installed.
      */
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetPackageContentsDigest();

    /**
      * Gets the SHA-256 digest of the contents of the specified file.
      */
    static CARAPI_(AutoPtr< ArrayOf<Byte> >) GetSha256ContentsDigest(
        /* [in] */ IFile* file);

public:
    /** Installation has not yet terminated. */
    static const Byte RESULT_NOT_YET_AVAILABLE;
    /** Package successfully installed. */
    static const Byte RESULT_SUCCESS;
    /** Installation failed because scheme unsupported. */
    static const Byte RESULT_FAILED_UNSUPPORTED_SCHEME;
    /**
      * Installation of an APK failed because of a failure to obtain information from the provided
      * APK.
      */
    static const Byte RESULT_FAILED_TO_GET_PACKAGE_INFO;
    /**
      * Installation of an already installed package into the current user profile failed because the
      * specified package is not installed.
      */
    static const Byte RESULT_FAILED_PACKAGE_MISSING;
    /**
      * Installation failed because installation from unknown sources is prohibited by the Unknown
      * Sources setting.
      */
    static const Byte RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING;
    /** Installation cancelled by the user. */
    static const Byte RESULT_CANCELLED_BY_USER;
    /**
      * Installation failed due to {@code PackageManager} failure. PackageManager error code is
      * provided in {@link #mPackageManagerInstallResult}).
      */
    static const Byte RESULT_PACKAGE_MANAGER_INSTALL_FAILED;

private:
    static const String TAG;
    static const Int32 FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED = 1 << 0;
    static const Int32 FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE = 1 << 1;
    static const Int32 FLAG_VERIFY_APPS_ENABLED = 1 << 2;
    static const Int32 FLAG_APP_VERIFIER_INSTALLED = 1 << 3;
    static const Int32 FLAG_FILE_URI = 1 << 4;
    static const Int32 FLAG_REPLACE = 1 << 5;
    static const Int32 FLAG_SYSTEM_APP = 1 << 6;
    static const Int32 FLAG_PACKAGE_INFO_OBTAINED = 1 << 7;
    static const Int32 FLAG_INSTALL_BUTTON_CLICKED = 1 << 8;
    static const Int32 FLAG_NEW_PERMISSIONS_FOUND = 1 << 9;
    static const Int32 FLAG_PERMISSIONS_DISPLAYED = 1 << 10;
    static const Int32 FLAG_NEW_PERMISSIONS_DISPLAYED = 1 << 11;
    static const Int32 FLAG_ALL_PERMISSIONS_DISPLAYED = 1 << 12;
    /**
      * Information about this flow expressed as a collection of flags. See {@code FLAG_...}
      * constants.
      */
    Int32 mFlags;
    /** Outcome of the flow. See {@code RESULT_...} constants. */
    Byte mResult;
    /**
      * Result code returned by {@code PackageManager} to install the package or {@code 0} if
      * {@code PackageManager} has not yet been invoked to install the package.
      */
    Int32 mPackageManagerInstallResult;
    /**
      * Time instant when the installation request arrived, measured in elapsed realtime
      * milliseconds. See {@link SystemClock#elapsedRealtime()}.
      */
    Int64 mStartTimestampMillis;
    /**
      * Time instant when the information about the package being installed was obtained, measured in
      * elapsed realtime milliseconds. See {@link SystemClock#elapsedRealtime()}.
      */
    Int64 mPackageInfoObtainedTimestampMillis;
    /**
      * Time instant when the user clicked the Install button, measured in elapsed realtime
      * milliseconds. See {@link SystemClock#elapsedRealtime()}. This field is only valid if the
      * Install button has been clicked, as signaled by {@link #FLAG_INSTALL_BUTTON_CLICKED}.
      */
    Int64 mInstallButtonClickTimestampMillis;
    /**
      * Time instant when this flow terminated, measured in elapsed realtime milliseconds. See
      * {@link SystemClock#elapsedRealtime()}.
      */
    Int64 mEndTimestampMillis;
    /** URI of the package being installed. */
    String mPackageUri;
    /** Whether this attempt has been logged to the Event Log. */
    Boolean mLogged;
    AutoPtr<IContext> mContext;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLFLOWANALYTICS_H__

