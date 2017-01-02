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

#include "elastos/droid/incallui/Presenter.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL(Presenter, Object, IPresenter);

ECode Presenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    mUi = ui;
    return NOERROR;
}

ECode Presenter::OnUiDestroy(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(OnUiUnready(ui));
    mUi = NULL;
    return NOERROR;
}

ECode Presenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    return NOERROR;
}

ECode Presenter::GetUi(
    /* [out] */ IUi** ui)
{
    VALIDATE_NOT_NULL(ui);
    *ui = mUi;
    REFCOUNT_ADD(*ui);
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
