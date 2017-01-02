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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class HbpcdUtils
    : public Object
    , public IHbpcdUtils
{
public:
    CAR_INTERFACE_DECL()

    HbpcdUtils(
        /* [in] */ IContext* context);

    /**
     *  Resolves the unknown MCC with SID and Timezone information.
    */
    CARAPI GetMcc(
        /* [in] */ Int32 sid,
        /* [in] */ Int32 tz,
        /* [in] */ Int32 DSTflag,
        /* [in] */ Boolean isNitzTimeZone,
        /* [out] */ Int32* result);

    /**
     *  Gets country information with given MCC.
    */
    CARAPI GetIddByMcc(
        /* [in] */ Int32 mcc,
        /* [out] */ String* result);

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<IContentResolver> mResolver;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__
