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

#ifndef __ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__
#define __ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace InCallUIBind {

class ObjectFactory
{
public:
    /** @return An {@link Intent} to be broadcast when the InCallUI is visible. */
    static CARAPI_(AutoPtr<IIntent>) GetUiReadyBroadcastIntent(
        /* [in] */ IContext* context);

    /**
     * @return An {@link Intent} to be broadcast when the call state button in the InCallUI is
     * touched while in a call.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallStateButtonBroadcastIntent(
        /* [in] */ IContext* context);
};

} // namespace InCallUIBind
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__

