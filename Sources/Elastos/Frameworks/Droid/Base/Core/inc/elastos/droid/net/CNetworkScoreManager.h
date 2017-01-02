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

#ifndef __ELASTOS_DROID_NET_CNETWORKSCOREMANAGER_H__
#define __ELASTOS_DROID_NET_CNETWORKSCOREMANAGER_H__

#include "_Elastos_Droid_Net_CNetworkScoreManager.h"
#include "elastos/droid/net/NetworkScoreManager.h"

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
CarClass(CNetworkScoreManager)
    , public NetworkScoreManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKSCOREMANAGER_H__
