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

#include "elastos/droid/database/CrossProcessCursorWrapper.h"
#include "elastos/droid/database/DatabaseUtils.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_2(CrossProcessCursorWrapper, CursorWrapper, ICrossProcessCursorWrapper, ICrossProcessCursor)

ECode CrossProcessCursorWrapper::FillWindow(
    /* [in] */ Int32 position,
    /* [in] */ ICursorWindow* window)
{
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->FillWindow(position, window);
    }

    DatabaseUtils::CursorFillWindow(mCursor, position, window);
    return NOERROR;
}

ECode CrossProcessCursorWrapper::GetWindow(
    /* [out] */ ICursorWindow** window)
{
    VALIDATE_NOT_NULL(window)
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->GetWindow(window);
    }
    *window = NULL;
    return NOERROR;
}

ECode CrossProcessCursorWrapper::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->OnMove(oldPosition, newPosition, result);
    }
    *result = TRUE;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos