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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatServiceFactory.h"
#include "elastos/droid/internal/telephony/cat/CCatService.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"

#include <elastos/core/AutoLock.h>

using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_UNKNOWN;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                          CatServiceFactory
//=====================================================================
AutoPtr<ArrayOf<ICatService*> > CatServiceFactory::sCatServices;

static Int32 InitSimCount()
{
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 count = 0;
    tm->GetSimCount(&count);
    return count;
}

const Int32 CatServiceFactory::sSimCount = InitSimCount();
Object CatServiceFactory::sInstanceLock;

AutoPtr<ICatService> CatServiceFactory::MakeCatService(
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IContext* context,
    /* [in] */ IUiccCard* ic,
    /* [in] */ Int32 slotId)
{
    AutoPtr<IUiccCardApplication> ca;
    AutoPtr<IIccFileHandler> fh;

    if (sCatServices == NULL) {
        sCatServices = ArrayOf<ICatService*>::Alloc(sSimCount);
    }

    if (ci == NULL || context == NULL || ic == NULL) {
        return NULL;
    }

    //get first valid filehandler in the card.
    Int32 num = 0;
    ic->GetNumApplications(&num);
    for (Int32 i = 0; i < num; i++) {
        ic->GetApplicationIndex(i, (IUiccCardApplication**)&ca);
        AppType type;
        if (ca != NULL && ((ca->GetType(&type), type) != APPTYPE_UNKNOWN)) {
            ca->GetIccFileHandler((IIccFileHandler**)&fh);
            break;
        }
    }

    {
        AutoLock lock(sInstanceLock);
        if (fh == NULL) {
            return NULL;
        }

        if ((*sCatServices)[slotId] == NULL) {
            CCatService::New(ci, context, fh, slotId, (ICatService**)&((*sCatServices)[slotId]));
        }
    }
    return (*sCatServices)[slotId];
}

AutoPtr<ICatService> CatServiceFactory::GetCatService(
    /* [in] */ Int32 slotId)
{
    return ((sCatServices == NULL) ? NULL : (*sCatServices)[slotId]);
}

ECode CatServiceFactory::DisposeCatService(
    /* [in] */ Int32 slotId)
{
    if (sCatServices != NULL) {
        (*sCatServices)[slotId]->Dispose();
        (*sCatServices)[slotId] = NULL;
    }
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
