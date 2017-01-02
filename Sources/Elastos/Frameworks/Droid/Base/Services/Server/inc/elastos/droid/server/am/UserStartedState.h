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

#ifndef __ELASTOS_DROID_SERVER_AM_USERSTARTEDSTATE_H__
#define __ELASTOS_DROID_SERVER_AM_USERSTARTEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IStopUserCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class UserStartedState
    : public Object
    , public IUserStartedState
{
public:
    UserStartedState(
        /* [in] */ IUserHandle * handle,
        /* [in] */ Boolean initial);

    CAR_INTERFACE_DECL()

    CARAPI_(void) Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

public:
    // User is first coming up.
    static const Int32 STATE_BOOTING;
    // User is in the normal running state.
    static const Int32 STATE_RUNNING;
    // User is in the initial process of being stopped.
    static const Int32 STATE_STOPPING;
    // User is in the final phase of stopping, sending Intent.ACTION_SHUTDOWN.
    static const Int32 STATE_SHUTDOWN;

    AutoPtr<IUserHandle> mHandle;
    List<AutoPtr<IStopUserCallback> > mStopCallbacks;

    Int32 mState;
    Boolean mSwitching;
    Boolean mInitializing;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_USERSTARTEDSTATE_H__
