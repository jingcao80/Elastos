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

#ifndef __ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__
#define __ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__

#include "_Elastos_Droid_Telecom_CRemoteServiceCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telecom::IRemoteServiceCallback;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteServiceCallback)
    , public Object
    , public IRemoteServiceCallback
{
public:
    class ResultRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ResultRunnable(
            /* [in] */ IList* componentNames,
            /* [in] */ IList* services,
            /* [in] */ CRemoteServiceCallback* host);

        CARAPI Run();

    public:
        AutoPtr<IList> mComponentNames;
        AutoPtr<IList> mServices;
        CRemoteServiceCallback* mHost;
    };

    class ErrorRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ErrorRunnable(
            /* [in] */ CRemoteServiceCallback* host);

        CARAPI Run();

    public:
        CRemoteServiceCallback* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetHost(
        /* [in] */ IConnectionService* host);

    CARAPI OnResult(
        /* [in] */ IList* componentNames,
        /* [in] */ IList* services);

    CARAPI OnError();

public:
    AutoPtr<IConnectionService> mHost;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__