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

#include "elastos/droid/internal/widget/CLockPatternViewCellHelper.h"
#include "elastos/droid/internal/widget/LockPatternView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_SINGLETON_IMPL(CLockPatternViewCellHelper);
CAR_INTERFACE_IMPL(CLockPatternViewCellHelper, Singleton, ILockPatternViewCellHelper);
ECode CLockPatternViewCellHelper::Of(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [in] */ Byte size,
    /* [out] */ ILockPatternViewCell** cell)
{
    VALIDATE_NOT_NULL(cell);
    return LockPatternView::Cell::Of(row, column, size, cell);
}

ECode CLockPatternViewCellHelper::UpdateSize(
    /* [in] */ Byte size)
{
    return LockPatternView::Cell::UpdateSize(size);
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
