#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ROTATIONPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ROTATIONPOLICY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

/**
 * Provides helper functions for configuring the display rotation policy.
 */
class RotationPolicy
{
public:
    /**
     * Listener that is invoked whenever a change occurs that might affect the rotation policy.
     */
    class RotationPolicyListener
        : public Object
        , public IRotationPolicyListener
    {
    private:
        class RotationContentObserver
            : public ContentObserver
        {
        public:
            RotationContentObserver(
                /* [in] */ RotationPolicyListener* owner);

            // @Override
            CARAPI OnChange(
                /* [in] */ Boolean selfChange,
                /* [in] */ IUri* uri);

        private:
            RotationPolicyListener* mOwner;
        };

    public:
        RotationPolicyListener();

        CAR_INTERFACE_DECL()

    public:
        AutoPtr<ContentObserver> mObserver;
    };

private:
    class RotationRunnable
        : public Runnable
    {
    public:
        RotationRunnable(
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 rotation);

        // @Override
        CARAPI Run();

    private:
        const Boolean mEnabled;
        const Int32 mRotation;
    };

public:
    /**
     * Gets whether the device supports rotation. In general such a
     * device has an accelerometer and has the portrait and landscape
     * features.
     *
     * @param context Context for accessing system resources.
     * @return Whether the device supports rotation.
     */
    static Boolean IsRotationSupported(
        /* [in] */ IContext* context);

    /**
     * Returns the orientation that will be used when locking the orientation from system UI
     * with {@link #SetRotationLock}.
     *
     * If the device only supports locking to its natural orientation, this will be either
     * Configuration.ORIENTATION_PORTRAIT or Configuration.ORIENTATION_LANDSCAPE,
     * otherwise Configuration.ORIENTATION_UNDEFINED if any orientation is lockable.
     */
    static Int32 GetRotationLockOrientation(
        /* [in] */ IContext* context);

    /**
     * Returns true if the rotation-lock toggle should be shown in system UI.
     */
    static Boolean IsRotationLockToggleVisible(
        /* [in] */ IContext* context);

    /**
     * Returns true if rotation lock is enabled.
     */
    static Boolean IsRotationLocked(
        /* [in] */ IContext* context);

    /**
     * Enables or disables rotation lock from the system UI toggle.
     */
    static void SetRotationLock(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    /**
     * Enables or disables natural rotation lock from Accessibility settings.
     *
     * If rotation is locked for accessibility, the system UI toggle is hidden to avoid confusion.
     */
    static void SetRotationLockForAccessibility(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    /**
     * Registers a listener for rotation policy changes affecting the caller's user
     */
    static void RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);

    /**
     * Registers a listener for rotation policy changes affecting a specific user,
     * or USER_ALL for all users.
     */
    static void RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener,
        /* [in] */ Int32 userHandle);

    /**
     * Unregisters a listener for rotation policy changes.
     */
    static void UnregisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);

private:
    RotationPolicy();

    static Boolean AreAllRotationsAllowed(
        /* [in] */ IContext* context);

    static void SetRotationLock(
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 rotation);

private:
    static const String TAG;
    static const Int32 CURRENT_ROTATION;
    static const Int32 NATURAL_ROTATION;
};

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_ROTATIONPOLICY_H__
