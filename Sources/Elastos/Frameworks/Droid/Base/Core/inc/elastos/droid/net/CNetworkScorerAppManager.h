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

#ifndef __ELASTOS_DROID_NET_CNETWORKSCORERAPPMANAGER_H__
#define __ELASTOS_DROID_NET_CNETWORKSCORERAPPMANAGER_H__

#include "_Elastos_Droid_Net_CNetworkScorerAppManager.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
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
CarClass(CNetworkScorerAppManager)
    , public Singleton
    , public INetworkScorerAppManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

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
    CARAPI GetAllValidScorers(
        /* [in] */ IContext* context,
        /* [out] */ ICollection** result);

    /**
     * Get the application to use for scoring networks.
     *
     * @return the scorer app info or null if scoring is disabled (including if no scorer was ever
     *     selected) or if the previously-set scorer is no longer a valid scorer app (e.g. because
     *     it was disabled or uninstalled).
     */
    CARAPI GetActiveScorer(
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
    CARAPI SetActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    /** Determine whether the application with the given UID is the enabled scorer. */
    CARAPI IsCallerActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ Int32 callingUid,
        /* [out] */ Boolean* result);

    /** Returns the {@link NetworkScorerAppData} for the given app, or null if it's not a scorer. */
    CARAPI GetScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ INetworkScorerAppData** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CNETWORKSCORERAPPMANAGER_H__
