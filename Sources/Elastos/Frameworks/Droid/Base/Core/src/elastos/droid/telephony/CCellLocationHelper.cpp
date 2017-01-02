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

#include "elastos/droid/telephony/CCellLocationHelper.h"
#include "elastos/droid/telephony/CellLocation.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CCellLocationHelper, Singleton, ICellLocationHelper)

CAR_SINGLETON_IMPL(CCellLocationHelper)

ECode CCellLocationHelper::RequestLocationUpdate()
{
    CellLocation::RequestLocationUpdate();
    return NOERROR;
}

ECode CCellLocationHelper::NewFromBundle(
    /* [in] */ IBundle* bundle,
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    CellLocation::NewFromBundle(bundle, cl);
    return NOERROR;
}

ECode CCellLocationHelper::GetEmpty(
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    CellLocation::GetEmpty(cl);
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony