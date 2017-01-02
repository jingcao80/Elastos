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

#include "elastos/droid/internal/telephony/cat/CCatServiceFactory.h"
#include "elastos/droid/internal/telephony/cat/CatServiceFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CAR_INTERFACE_IMPL(CCatServiceFactory, Singleton, ICatServiceFactory)
CAR_SINGLETON_IMPL(CCatServiceFactory)

ECode CCatServiceFactory::MakeCatService(
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IContext* context,
    /* [in] */ IUiccCard* ic,
    /* [in] */ Int32 slotId,
    /* [out] */ ICatService** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICatService> cs = CatServiceFactory::MakeCatService(ci, context, ic, slotId);
    *result = cs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCatServiceFactory::GetCatService(
    /* [in] */ Int32 slotId,
    /* [out] */ ICatService** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICatService> cs = CatServiceFactory::GetCatService(slotId);
    *result = cs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCatServiceFactory::DisposeCatService(
    /* [in] */ Int32 slotId)
{
    CatServiceFactory::DisposeCatService(slotId);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
