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

#ifndef __ELASTOS_DROID_SERVER_CNETWORKSCORESERVICE_H__
#define __ELASTOS_DROID_SERVER_CNETWORKSCORESERVICE_H__

#include "_Elastos_Droid_Server_CNetworkScoreService.h"
#include <elastos/droid/net/NetworkScoreManager.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/os/Binder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IINetworkScoreCache;
using Elastos::Droid::Net::IINetworkScoreService;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::IScoredNetwork;
using Elastos::Droid::Os::IBinder;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Backing service for {@link android.net.NetworkScoreManager}.
 * @hide
 */
CarClass(CNetworkScoreService)
    , public Object
    , public IBinder
    , public IINetworkScoreService
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /** Called when the system is ready to run third-party code but before it actually does so. */
    CARAPI SystemReady();

    // @Override
    CARAPI UpdateScores(
        /* [in] */ ArrayOf<IScoredNetwork*>* networks,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearScores(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetActiveScorer(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DisableScoring();

    // @Override
    CARAPI RegisterNetworkScoreCache(
        /* [in] */ Int32 networkType,
        /* [in] */ IINetworkScoreCache* scoreCache);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    // for IBinder
    CARAPI ToString(
        /* [out] */ String* result);

private:
    /** Set the active scorer. Callers are responsible for checking permissions as appropriate. */
    CARAPI SetScorerInternal(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    /** Clear scores. Callers are responsible for checking permissions as appropriate. */
    CARAPI ClearInternal();

    /**
     * Returns a set of all score caches that are currently active.
     *
     * <p>May be used to perform an action on all score caches without potentially strange behavior
     * if a new scorer is registered during that action's execution.
     */
    CARAPI_(AutoPtr<ISet>) GetScoreCaches();

private:
    static const String TAG;

    AutoPtr<IContext> mContext;

    AutoPtr<IMap> mScoreCaches;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CNETWORKSCORESERVICE_H__
