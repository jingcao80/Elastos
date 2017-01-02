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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatServiceFactory.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatServiceFactory)
    , public Singleton
    , public ICatServiceFactory
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI MakeCatService(
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IContext* context,
        /* [in] */ IUiccCard* ic,
        /* [in] */ Int32 slotId,
        /* [out] */ ICatService** result);

    CARAPI GetCatService(
        /* [in] */ Int32 slotId,
        /* [out] */ ICatService** result);

    CARAPI DisposeCatService(
        /* [in] */ Int32 slotId);

};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATSERVICEFACTORY_H__
