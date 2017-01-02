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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Content_Pm_CPackageInstaller.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::IContext;
using Elastos::Core::ICharSequence;
using Elastos::IO::IInputStream;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Offers the ability to install, upgrade, and remove applications on the
 * device. This includes support for apps packaged either as a single
 * "monolithic" APK, or apps packaged as multiple "split" APKs.
 * <p>
 * An app is delivered for installation through a
 * {@link PackageInstaller.Session}, which any app can create. Once the session
 * is created, the installer can stream one or more APKs into place until it
 * decides to either commit or destroy the session. Committing may require user
 * intervention to complete the installation.
 * <p>
 * Sessions can install brand new apps, upgrade existing apps, or add new splits
 * into an existing app.
 * <p>
 * Apps packaged as multiple split APKs always consist of a single "base" APK
 * (with a {@code null} split name) and zero or more "split" APKs (with unique
 * split names). Any subset of these APKs can be installed together, as long as
 * the following constraints are met:
 * <ul>
 * <li>All APKs must have the exact same package name, version code, and signing
 * certificates.
 * <li>All APKs must have unique split names.
 * <li>All installations must contain a single base APK.
 * </ul>
 */
CarClass(CPackageInstaller)
    , public Object
    , public IPackageInstaller
{
private:
    class SessionCallbackDelegate
        : public Object
        , public IPackageInstallerCallback
        , public IHandlerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        SessionCallbackDelegate(
            /* [in] */ IPackageInstallerSessionCallback* callback,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI OnSessionCreated(
            /* [in] */ Int32 sessionId);

        CARAPI OnSessionBadgingChanged(
            /* [in] */ Int32 sessionId);

        CARAPI OnSessionActiveChanged(
            /* [in] */ Int32 sessionId,
            /* [in] */ Boolean active);

        CARAPI OnSessionProgressChanged(
            /* [in] */ Int32 sessionId,
            /* [in] */ Float progress);

        CARAPI OnSessionFinished(
            /* [in] */ Int32 sessionId,
            /* [in] */ Boolean success);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        friend class CPackageInstaller;

        static const Int32 MSG_SESSION_CREATED;// = 1;
        static const Int32 MSG_SESSION_BADGING_CHANGED;// = 2;
        static const Int32 MSG_SESSION_ACTIVE_CHANGED;// = 3;
        static const Int32 MSG_SESSION_PROGRESS_CHANGED;// = 4;
        static const Int32 MSG_SESSION_FINISHED;// = 5;

        AutoPtr<IPackageInstallerSessionCallback> mCallback;
        AutoPtr<IHandler> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageInstaller();

    ~CPackageInstaller();

    CARAPI constructor();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPackageManager* pm,
        /* [in] */ IIPackageInstaller* installer,
        /* [in] */ const String& installerPackageName,
        /* [in] */ Int32 userId);

    /**
     * Create a new session using the given parameters, returning a unique ID
     * that represents the session. Once created, the session can be opened
     * multiple times across multiple device boots.
     * <p>
     * The system may automatically destroy sessions that have not been
     * finalized (either committed or abandoned) within a reasonable period of
     * time, typically on the order of a day.
     *
     * @throws IOException if parameters were unsatisfiable, such as lack of
     *             disk space or unavailable media.
     * @throws SecurityException when installation services are unavailable,
     *             such as when called from a restricted user.
     * @throws IllegalArgumentException when {@link SessionParams} is invalid.
     * @return positive, non-zero unique ID that represents the created session.
     *         This ID remains consistent across device reboots until the
     *         session is finalized. IDs are not reused during a given boot.
     */
    CARAPI CreateSession(
        /* [in] */ IPackageInstallerSessionParams* params,
        /* [out] */ Int32* sessionId);

    /**
     * Open an existing session to actively perform work. To succeed, the caller
     * must be the owner of the install session.
     *
     * @throws IOException if parameters were unsatisfiable, such as lack of
     *             disk space or unavailable media.
     * @throws SecurityException when the caller does not own the session, or
     *             the session is invalid.
     */
    CARAPI OpenSession(
        /* [in] */ Int32 sessionId,
        /* [out] */ IPackageInstallerSession** session);

    /**
     * Update the icon representing the app being installed in a specific
     * session. This should be roughly
     * {@link ActivityManager#getLauncherLargeIconSize()} in both dimensions.
     *
     * @throws SecurityException when the caller does not own the session, or
     *             the session is invalid.
     */
    CARAPI UpdateSessionAppIcon(
        /* [in] */ Int32 sessionId,
        /* [in] */ IBitmap* appIcon);

    /**
     * Update the label representing the app being installed in a specific
     * session.
     *
     * @throws SecurityException when the caller does not own the session, or
     *             the session is invalid.
     */
    CARAPI UpdateSessionAppLabel(
        /* [in] */ Int32 sessionId,
        /* [in] */ ICharSequence* appLabel);

    /**
     * Completely abandon the given session, destroying all staged data and
     * rendering it invalid. Abandoned sessions will be reported to
     * {@link SessionCallback} listeners as failures. This is equivalent to
     * opening the session and calling {@link Session#abandon()}.
     *
     * @throws SecurityException when the caller does not own the session, or
     *             the session is invalid.
     */
    CARAPI AbandonSession(
        /* [in] */ Int32 sessionId);

    /**
     * Return details for a specific session. No special permissions are
     * required to retrieve these details.
     *
     * @return details for the requested session, or {@code null} if the session
     *         does not exist.
     */
    CARAPI GetSessionInfo(
        /* [in] */ Int32 sessionId,
        /* [out] */ IPackageInstallerSessionInfo** session);

    /**
     * Return list of all known install sessions, regardless of the installer.
     */
    CARAPI GetAllSessions(
        /* [out] */ IList** sessions);

    /**
     * Return list of all known install sessions owned by the calling app.
     */
    CARAPI GetMySessions(
        /* [out] */ IList** sessions);

    /**
     * Uninstall the given package, removing it completely from the device. This
     * method is only available to the current "installer of record" for the
     * package.
     */
    CARAPI Uninstall(
        /* [in] */ const String& packageName,
        /* [in] */ IIntentSender* statusReceiver);

    CARAPI SetPermissionsResult(
        /* [in] */ Int32 sessionId,
        /* [in] */ Boolean accepted);

    /** {@hide} */
    CARAPI AddSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback);

    /**
     * Register to watch for session lifecycle events. No special permissions
     * are required to watch for these events.
     */
    CARAPI RegisterSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback);

    CARAPI AddSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback,
        /* [in] */ IHandler* handler);

    /**
     * Register to watch for session lifecycle events. No special permissions
     * are required to watch for these events.
     *
     * @param handler to dispatch callback events through, otherwise uses
     *            calling thread.
     */
    CARAPI RegisterSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback,
        /* [in] */ IHandler* handler);

    /** {@hide} */
    CARAPI RemoveSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback);

    /**
     * Unregister a previously registered callback.
     */
    CARAPI UnregisterSessionCallback(
        /* [in] */ IPackageInstallerSessionCallback* callback);

private:
    static const String TAG;// = "PackageInstaller";

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IIPackageInstaller> mInstaller;
    Int32 mUserId;
    String mInstallerPackageName;

    AutoPtr<IArrayList> mDelegates;
    // ArrayList<SessionCallbackDelegate> mDelegates = new ArrayList<>();

};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_H__
