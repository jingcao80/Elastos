
#ifndef __ELASTOS_DROID_NET_NETWORKSCORERAPPMANAGER_H__
#define __ELASTOS_DROID_NET_NETWORKSCORERAPPMANAGER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

using Elastos::Core::ICharSequence;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Internal class for managing the primary network scorer application.
 *
 * TODO: Rename this to something more generic.
 *
 * @hide
 */
class NetworkScorerAppManager
{
public:
    /**
     * Returns the list of available scorer apps.
     *
     * <p>A network scorer is any application which:
     * <ul>
     * <li>Declares the {@link android.Manifest.permission#SCORE_NETWORKS} permission.
     * <li>Includes a receiver for {@link NetworkScoreManager#ACTION_SCORE_NETWORKS} guarded by the
     *     {@link android.Manifest.permission#BROADCAST_SCORE_NETWORKS} permission.
     * </ul>
     *
     * @return the list of scorers, or the empty list if there are no valid scorers.
     */
    static CARAPI GetAllValidScorers(
        /* [in] */ IContext* context,
        /* [out] */ ICollection** result);

    /**
     * Get the application to use for scoring networks.
     *
     * @return the scorer app info or null if scoring is disabled (including if no scorer was ever
     *     selected) or if the previously-set scorer is no longer a valid scorer app (e.g. because
     *     it was disabled or uninstalled).
     */
    static CARAPI GetActiveScorer(
        /* [in] */ IContext* context,
        /* [out] */ INetworkScorerAppData** result);

    /**
     * Set the specified package as the default scorer application.
     *
     * <p>The caller must have permission to write to {@link android.provider.Settings.Global}.
     *
     * @param context the context of the calling application
     * @param packageName the packageName of the new scorer to use. If null, scoring will be
     *     disabled. Otherwise, the scorer will only be set if it is a valid scorer application.
     * @return true if the scorer was changed, or false if the package is not a valid scorer.
     */
    static CARAPI SetActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    /** Determine whether the application with the given UID is the enabled scorer. */
    static CARAPI IsCallerActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ Int32 callingUid,
        /* [out] */ Boolean* result);

    /** Returns the {@link NetworkScorerAppData} for the given app, or null if it's not a scorer. */
    static CARAPI GetScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ INetworkScorerAppData** result);

private:
    static CARAPI_(AutoPtr<IIntent>) CreateScoreIntent();

private:
    static const AutoPtr<IIntent> SCORE_INTENT;

    /** This class cannot be instantiated. */
    NetworkScorerAppManager();

    static const String TAG;
};

class NetworkScorerAppData
    : public Object
    , public INetworkScorerAppData
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ ICharSequence* scorerName,
        /* [in] */ /* @Nullable */ const String& configurationActivityClassName);

    /** Package name of this scorer app. */
    /** Package name of this scorer app. */
    CARAPI GetPackageName(
        /* [out] */ String* result);

    /** Name of this scorer app for display. */
    /** Name of this scorer app for display. */
    CARAPI GetScorerName(
        /* [out] */ ICharSequence** result);

    /**
     * Optional class name of a configuration activity. Null if none is set.
     *
     * @see NetworkScoreManager#ACTION_CUSTOM_ENABLE
     */
    CARAPI GetConfigurationActivityClassName(
        /* [out] */ String* result);

private:
    /** Package name of this scorer app. */
    String mPackageName;

    /** Name of this scorer app for display. */
    AutoPtr<ICharSequence> mScorerName;

    /**
     * Optional class name of a configuration activity. Null if none is set.
     *
     * @see NetworkScoreManager#ACTION_CUSTOM_ENABLE
     */
    String mConfigurationActivityClassName;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSCORERAPPMANAGER_H__
