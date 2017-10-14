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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/app/MediaRouteActionProvider.h"
#include "elastos/droid/app/CMediaRouteButton.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

MediaRouteActionProvider::MediaRouterCallback::MediaRouterCallback()
{}

ECode MediaRouteActionProvider::MediaRouterCallback::constructor(
    /* [in] */ IMediaRouteActionProvider* provider)
{
    assert(0 && "TODO");
    // CMediaRouter::SimpleCallback::constructor();

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(provider);
    wrs->GetWeakReference((IWeakReference**)&mProviderWeak);
    return NOERROR;
}

ECode MediaRouteActionProvider::MediaRouterCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    RefreshRoute(router);
    return NOERROR;
}

ECode MediaRouteActionProvider::MediaRouterCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    RefreshRoute(router);
    return NOERROR;
}

ECode MediaRouteActionProvider::MediaRouterCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    RefreshRoute(router);
    return NOERROR;
}

void MediaRouteActionProvider::MediaRouterCallback::RefreshRoute(
    /* [in] */ IMediaRouter* router)
{
    AutoPtr<IInterface> obj;
    mProviderWeak->Resolve(EIID_IInterface, (IInterface**)&obj);
    MediaRouteActionProvider* provider = (MediaRouteActionProvider*)IMediaRouteActionProvider::Probe(obj);
    if (provider != NULL) {
        provider->RefreshRoute();
    }
    else {
        router->RemoveCallback(this);
    }
}

//==========================================================================
// MediaRouteActionProvider
//==========================================================================
const String MediaRouteActionProvider::TAG("MediaRouteActionProvider");

CAR_INTERFACE_IMPL(MediaRouteActionProvider, ActionProvider, IMediaRouteActionProvider)

MediaRouteActionProvider::MediaRouteActionProvider()
    : mRouteTypes(0)
{}

MediaRouteActionProvider::~MediaRouteActionProvider()
{}

ECode MediaRouteActionProvider::constructor(
    /* [in] */ IContext* context)
{
    ActionProvider::constructor(context);

    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mRouter = IMediaRouter::Probe(obj);
    AutoPtr<MediaRouterCallback> cb = new MediaRouterCallback();
    cb->constructor(this);
    mCallback = cb.Get();

    // Start with live audio by default.
    // TODO Update this when new route types are added; segment by API level
    // when different route types were added.
    return SetRouteTypes(IMediaRouter::ROUTE_TYPE_LIVE_AUDIO);
}

ECode MediaRouteActionProvider::SetRouteTypes(
    /* [in] */ Int32 types)
{
    if (mRouteTypes != types) {
        // FIXME: We currently have no way of knowing whether the action provider
        // is still needed by the UI.  Unfortunately this means the action provider
        // may leak callbacks until garbage collection occurs.  This may result in
        // media route providers doing more work than necessary in the short term
        // while trying to discover routes that are no longer of Int32erest to the
        // application.  To solve this problem, the action provider will need some
        // indication from the framework that it is being destroyed.
        if (mRouteTypes != 0) {
            mRouter->RemoveCallback(mCallback);
        }
        mRouteTypes = types;
        if (types != 0) {
            mRouter->AddCallback(types, mCallback,
                IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
        }
        RefreshRoute();

        if (mButton != NULL) {
            mButton->SetRouteTypes(mRouteTypes);
        }
    }
    return NOERROR;
}

ECode MediaRouteActionProvider::SetExtendedSettingsClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mExtendedSettingsListener = listener;
    if (mButton != NULL) {
        mButton->SetExtendedSettingsClickListener(listener);
    }
    return NOERROR;
}

ECode MediaRouteActionProvider::OnCreateActionView(
    /* [out] */ IView** view)
{
    Logger::E(TAG, "Use onCreateActionView(MenuItem) instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MediaRouteActionProvider::OnCreateActionView(
    /* [in] */ IMenuItem* item,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    if (mButton != NULL) {
        Logger::E(TAG, "onCreateActionView: this ActionProvider is already associated "
                "with a menu item. Don't reuse MediaRouteActionProvider instances! "
                "Abandoning the old one...");
    }

    mButton = NULL;
    CMediaRouteButton::New(mContext, (IMediaRouteButton**)&mButton);
    ((CMediaRouteButton*)mButton.Get())->SetCheatSheetEnabled(TRUE);
    mButton->SetRouteTypes(mRouteTypes);
    mButton->SetExtendedSettingsClickListener(mExtendedSettingsListener);
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&params);
    IView::Probe(mButton)->SetLayoutParams(params);
    *view = IView::Probe(mButton);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode MediaRouteActionProvider::OnPerformDefaultAction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mButton != NULL) {
        *result = ((CMediaRouteButton*)mButton.Get())->ShowDialogInternal();
    }
    return NOERROR;
}

ECode MediaRouteActionProvider::OverridesItemVisibility(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode MediaRouteActionProvider::IsVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mRouter->IsRouteAvailable(mRouteTypes,
            IMediaRouter::AVAILABILITY_FLAG_IGNORE_DEFAULT_ROUTE, result);
}

void MediaRouteActionProvider::RefreshRoute()
{
    RefreshVisibility();
}


} // namespace App
} // namespace Droid
} // namespace Elastos
