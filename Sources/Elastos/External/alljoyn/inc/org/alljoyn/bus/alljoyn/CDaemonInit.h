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

#ifndef __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
#define __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__

#include "_Org_Alljoyn_Bus_Alljoyn_CDaemonInit.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

CarClass(CDaemonInit)
    , public Singleton
    , public IDaemonInit
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI PrepareDaemon(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
