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

#ifndef __ELASTOS_DROID_NET_NETWORKSCOREMANAGER_H__
#define __ELASTOS_DROID_NET_NETWORKSCOREMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that manages communication between network subsystems and a network scorer.
 *
 * <p>You can get an instance of this class by calling
 * {@link android.content.Context#getSystemService(String)}:
 *
 * <pre>NetworkScoreManager manager =
 *     (NetworkScoreManager) getSystemService(Context.NETWORK_SCORE_SERVICE)</pre>
 *
 * <p>A network scorer is any application which:
 * <ul>
 * <li>Declares the {@link android.Manifest.permission#SCORE_NETWORKS} permission.
 * <li>Includes a receiver for {@link #ACTION_SCORE_NETWORKS} guarded by the
 *     {@link android.Manifest.permission#BROADCAST_SCORE_NETWORKS} permission which scores networks
 *     and (eventually) calls {@link #updateScores} with the results. If this receiver specifies an
 *     android:label attribute, this label will be used when referring to the application throughout
 *     system settings; otherwise, the application label will be used.
 * </ul>
 *
 * <p>The system keeps track of an active scorer application; at any time, only this application
 * will receive {@link #ACTION_SCORE_NETWORKS} broadcasts and will be permitted to call
 * {@link #updateScores}. Applications may determine the current active scorer with
 * {@link #getActiveScorerPackage()} and request to change the active scorer by sending an
 * {@link #ACTION_CHANGE_ACTIVE} broadcast with another scorer.
 *
 * @hide
 */
// @SystemApi
class NetworkScoreManager
    : public Object
    , public INetworkScoreManager
{
public:
    CAR_INTERFACE_DECL()

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Obtain the package name of the current active network scorer.
     *
     * <p>At any time, only one scorer application will receive {@link #ACTION_SCORE_NETWORKS}
     * broadcasts and be allowed to call {@link #updateScores}. Applications may use this method to
     * determine the current scorer and offer the user the ability to select a different scorer via
     * the {@link #ACTION_CHANGE_ACTIVE} intent.
     * @return the full package name of the current active scorer, or null if there is no active
     *         scorer.
     */
    CARAPI GetActiveScorerPackage(
        /* [out] */ String* result);

    /**
     * Update network scores.
     *
     * <p>This may be called at any time to re-score active networks. Scores will generally be
     * updated quickly, but if this method is called too frequently, the scores may be held and
     * applied at a later time.
     *
     * @param networks the networks which have been scored by the scorer.
     * @return whether the update was successful.
     * @throws SecurityException if the caller is not the active scorer.
     */
    CARAPI UpdateScores(
        /* [in] */ ArrayOf<IScoredNetwork*>* networks,
        /* [out] */ Boolean* result);

    /**
     * Clear network scores.
     *
     * <p>Should be called when all scores need to be invalidated, i.e. because the scoring
     * algorithm has changed and old scores can no longer be compared to future scores.
     *
     * <p>Note that scores will be cleared automatically when the active scorer changes, as scores
     * from one scorer cannot be compared to those from another scorer.
     *
     * @return whether the clear was successful.
     * @throws SecurityException if the caller is not the active scorer or privileged.
     */
    CARAPI ClearScores(
        /* [out] */ Boolean* result);

    /**
     * Set the active scorer to a new package and clear existing scores.
     *
     * @return true if the operation succeeded, or false if the new package is not a valid scorer.
     * @throws SecurityException if the caller does not hold the
     *         {@link android.Manifest.permission#BROADCAST_SCORE_NETWORKS} permission indicating
     *         that it can manage scorer applications.
     * @hide
     */
    CARAPI SetActiveScorer(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    /**
     * Turn off network scoring.
     *
     * <p>May only be called by the current scorer app, or the system.
     *
     * @throws SecurityException if the caller is neither the active scorer nor the system.
     */
    CARAPI DisableScoring();

    /**
     * Request scoring for networks.
     *
     * <p>Note that this is just a helper method to assemble the broadcast, and will run in the
     * calling process.
     *
     * @return true if the broadcast was sent, or false if there is no active scorer.
     * @throws SecurityException if the caller does not hold the
     *         {@link android.Manifest.permission#BROADCAST_SCORE_NETWORKS} permission.
     * @hide
     */
    CARAPI RequestScores(
        /* [in] */ ArrayOf<INetworkKey*>* networks,
        /* [out] */ Boolean* result);

    /**
     * Register a network score cache.
     *
     * @param networkType the type of network this cache can handle. See {@link NetworkKey#type}.
     * @param scoreCache implementation of {@link INetworkScoreCache} to store the scores.
     * @throws SecurityException if the caller does not hold the
     *         {@link android.Manifest.permission#BROADCAST_SCORE_NETWORKS} permission.
     * @throws IllegalArgumentException if a score cache is already registered for this type.
     * @hide
     */
    CARAPI RegisterNetworkScoreCache(
        /* [in] */ Int32 networkType,
        /* [in] */ IINetworkScoreCache* scoreCache);

private:
    IContext* mContext;

    AutoPtr<IINetworkScoreService> mService;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSCOREMANAGER_H__
