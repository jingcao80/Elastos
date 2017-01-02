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

#include "Elastos.Droid.App.h"
#include "elastos/droid/internal/app/CMediaRouteDialogPresenter.h"
#include "elastos/droid/internal/app/MediaRouteDialogPresenter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(CMediaRouteDialogPresenter, Singleton, IMediaRouteDialogPresenter)
CAR_SINGLETON_IMPL(CMediaRouteDialogPresenter)

ECode CMediaRouteDialogPresenter::ShowDialogFragment(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
    /* [out] */ IDialogFragment** dialogFragment)
{
    VALIDATE_NOT_NULL(dialogFragment)
    AutoPtr<IDialogFragment> temp = MediaRouteDialogPresenter::ShowDialogFragment(
        activity, routeTypes, extendedSettingsClickListener);
    *dialogFragment = temp;
    REFCOUNT_ADD(*dialogFragment)
    return NOERROR;
}

ECode CMediaRouteDialogPresenter::CreateDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IDialog> temp = MediaRouteDialogPresenter::CreateDialog(
        context, routeTypes, extendedSettingsClickListener);
    *dialog = temp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
