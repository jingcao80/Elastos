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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/internal/app/MediaRouteDialogPresenter.h"
#include "elastos/droid/internal/app/CMediaRouteChooserDialog.h"
#include "elastos/droid/internal/app/CMediaRouteChooserDialogFragment.h"
#include "elastos/droid/internal/app/CMediaRouteControllerDialog.h"
#include "elastos/droid/internal/app/CMediaRouteControllerDialogFragment.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IFragment;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

const String MediaRouteDialogPresenter::TAG("MediaRouter");

const String MediaRouteDialogPresenter::CHOOSER_FRAGMENT_TAG(
    "android.app.MediaRouteButton:MediaRouteChooserDialogFragment");

const String MediaRouteDialogPresenter::CONTROLLER_FRAGMENT_TAG(
    "android.app.MediaRouteButton:MediaRouteControllerDialogFragment");

AutoPtr<IDialogFragment> MediaRouteDialogPresenter::ShowDialogFragment(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener)
{
    AutoPtr<IInterface> service;
    IContext::Probe(activity)->GetSystemService(
        IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&service);
    AutoPtr<IMediaRouter> router = IMediaRouter::Probe(service);

    AutoPtr<IFragmentManager> fm;
    activity->GetFragmentManager((IFragmentManager**)&fm);

    AutoPtr<IMediaRouterRouteInfo> route;
    router->GetSelectedRoute((IMediaRouterRouteInfo**)&route);
    AutoPtr<IFragment> fragment;
    Boolean isDefault, matchesTypes;
    if ((route->IsDefault(&isDefault), isDefault) ||
        !(route->MatchesTypes(routeTypes, &matchesTypes), matchesTypes)) {
        fm->FindFragmentByTag(CHOOSER_FRAGMENT_TAG, (IFragment**)&fragment);
        if (fragment != NULL) {
            Logger::W(TAG, "showDialog(): Route chooser dialog already showing!");
            return NULL;
        }
        AutoPtr<IMediaRouteChooserDialogFragment> f;
        CMediaRouteChooserDialogFragment::New((IMediaRouteChooserDialogFragment**)&f);
        f->SetRouteTypes(routeTypes);
        f->SetExtendedSettingsClickListener(extendedSettingsClickListener);
        IDialogFragment::Probe(f)->Show(fm, CHOOSER_FRAGMENT_TAG);
        return IDialogFragment::Probe(f);
    }
    else {
        fm->FindFragmentByTag(CONTROLLER_FRAGMENT_TAG, (IFragment**)&fragment);
        if (fragment != NULL) {
            Logger::W(TAG, "showDialog(): Route controller dialog already showing!");
            return NULL;
        }
        AutoPtr<IMediaRouteControllerDialogFragment> f;
        CMediaRouteControllerDialogFragment::New((IMediaRouteControllerDialogFragment**)&f);
        IDialogFragment::Probe(f)->Show(fm, CONTROLLER_FRAGMENT_TAG);
        return IDialogFragment::Probe(f);
    }
}

AutoPtr<IDialog> MediaRouteDialogPresenter::CreateDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&service);
    AutoPtr<IMediaRouter> router = IMediaRouter::Probe(service);

    AutoPtr<IMediaRouterRouteInfo> route;
    router->GetSelectedRoute((IMediaRouterRouteInfo**)&route);

    Boolean isDefault, matchesTypes;
    if ((route->IsDefault(&isDefault), isDefault) ||
        !(route->MatchesTypes(routeTypes, &matchesTypes), matchesTypes)) {
        AutoPtr<IMediaRouteChooserDialog> d;
        CMediaRouteChooserDialog::New(context,
            R::style::Theme_DeviceDefault_Dialog, (IMediaRouteChooserDialog**)&d);
        d->SetRouteTypes(routeTypes);
        d->SetExtendedSettingsClickListener(extendedSettingsClickListener);
        return IDialog::Probe(d);
    }
    else {
        AutoPtr<IMediaRouteControllerDialog> d;
        CMediaRouteControllerDialog::New(context,
            R::style::Theme_DeviceDefault_Dialog, (IMediaRouteControllerDialog**)&d);
        return IDialog::Probe(d);
    }
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
