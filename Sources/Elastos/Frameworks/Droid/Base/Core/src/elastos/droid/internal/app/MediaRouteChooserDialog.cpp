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

#include "elastos/droid/internal/app/MediaRouteChooserDialog.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(MediaRouteChooserDialog::RouteAdapter, ArrayAdapter,
    IAdapterViewOnItemClickListener)

MediaRouteChooserDialog::RouteAdapter::RouteAdapter(
    /* [in] */ IContext* context,
    /* [in] */ MediaRouteChooserDialog* host)
    : mHost(host)
{
    ArrayAdapter::constructor(context, 0);
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
}

ECode MediaRouteChooserDialog::RouteAdapter::Update()
{
    Clear();
    Int32 count;
    mHost->mRouter->GetRouteCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IMediaRouterRouteInfo> route;
        mHost->mRouter->GetRouteAt(i, (IMediaRouterRouteInfo**)&route);
        Boolean res;
        if (mHost->OnFilterRoute(route, &res), res) {
            Add(route);
        }
    }
    Sort(RouteComparator::sInstance);
    NotifyDataSetChanged();
    return NOERROR;
}

ECode MediaRouteChooserDialog::RouteAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode MediaRouteChooserDialog::RouteAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    return IMediaRouterRouteInfo::Probe(item)->IsEnabled(result);
}

ECode MediaRouteChooserDialog::RouteAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** retView)
{
    AutoPtr<IView> view = convertView;
    if (view == NULL) {
        mInflater->Inflate(R::layout::media_route_list_item, parent, FALSE, (IView**)&view);
    }
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    IMediaRouterRouteInfo* route = IMediaRouterRouteInfo::Probe(item);
    AutoPtr<IView> view1;
    view->FindViewById(R::id::text1, (IView**)&view1);
    ITextView* text1 = ITextView::Probe(view1);
    AutoPtr<IView> view2;
    view->FindViewById(R::id::text2, (IView**)&view2);
    ITextView* text2 = ITextView::Probe(view2);
    AutoPtr<ICharSequence> name;
    route->GetName((ICharSequence**)&name);
    text1->SetText(name);
    AutoPtr<ICharSequence> description;
    route->GetDescription((ICharSequence**)&description);
    if (TextUtils::IsEmpty(description)) {
        view1->SetVisibility(IView::GONE);
        text2->SetText(CoreUtils::Convert(String("")));
    }
    else {
        view2->SetVisibility(IView::VISIBLE);
        text2->SetText(description);
    }
    Boolean isEnabled;
    route->IsEnabled(&isEnabled);
    view->SetEnabled(isEnabled);
    *retView = view;
    REFCOUNT_ADD(*retView)
    return NOERROR;
}

ECode MediaRouteChooserDialog::RouteAdapter::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    IMediaRouterRouteInfo* route = IMediaRouterRouteInfo::Probe(item);
    Boolean isEnabled;
    route->IsEnabled(&isEnabled);
    if (isEnabled) {
        route->Select();
        mHost->Dismiss();
    }
    return NOERROR;
}

MediaRouteChooserDialog::MediaRouterCallback::MediaRouterCallback(
    /* [in] */ MediaRouteChooserDialog* host)
    : mHost(host)
{}

ECode MediaRouteChooserDialog::MediaRouterCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return mHost->RefreshRoutes();
}

ECode MediaRouteChooserDialog::MediaRouterCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return mHost->RefreshRoutes();
}

ECode MediaRouteChooserDialog::MediaRouterCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return mHost->RefreshRoutes();
}

ECode MediaRouteChooserDialog::MediaRouterCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return mHost->Dismiss();
}

const AutoPtr<MediaRouteChooserDialog::RouteComparator> MediaRouteChooserDialog::RouteComparator::sInstance
     = new MediaRouteChooserDialog::RouteComparator();

CAR_INTERFACE_IMPL(MediaRouteChooserDialog::RouteComparator, Object, IComparator)

ECode MediaRouteChooserDialog::RouteComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    IMediaRouterRouteInfo* lhs = IMediaRouterRouteInfo::Probe(_lhs);
    IMediaRouterRouteInfo* rhs = IMediaRouterRouteInfo::Probe(_rhs);
    assert(lhs && rhs);
    AutoPtr<ICharSequence> lhsName, rhsName;
    lhs->GetName((ICharSequence**)&lhsName);
    rhs->GetName((ICharSequence**)&rhsName);
    String lhsStrName, rhsStrName;
    lhsName->ToString(&lhsStrName);
    rhsName->ToString(&rhsStrName);
    *result = lhsStrName.Compare(rhsStrName);
    return NOERROR;
}

CAR_INTERFACE_IMPL(MediaRouteChooserDialog, Dialog, IMediaRouteChooserDialog)

MediaRouteChooserDialog::MediaRouteChooserDialog()
    : mRouteTypes(0)
    , mAttachedToWindow(FALSE)
{}

ECode MediaRouteChooserDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    FAIL_RETURN(Dialog::constructor(context, theme))

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&service);
    mRouter = IMediaRouter::Probe(service);
    assert(0);
    // mCallback = new MediaRouterCallback(this);
    return NOERROR;
}

ECode MediaRouteChooserDialog::GetRouteTypes(
    /* [out] */ Int32* types)
{
    VALIDATE_NOT_NULL(types)
    *types = mRouteTypes;
    return NOERROR;
}

ECode MediaRouteChooserDialog::SetRouteTypes(
    /* [in] */ Int32 types)
{
    if (mRouteTypes != types) {
        mRouteTypes = types;

        if (mAttachedToWindow) {
            mRouter->RemoveCallback(mCallback);
            mRouter->AddCallback(types, mCallback,
                IMediaRouter::CALLBACK_FLAG_PERFORM_ACTIVE_SCAN);
        }

        RefreshRoutes();
    }
    return NOERROR;
}

ECode MediaRouteChooserDialog::SetExtendedSettingsClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    if (listener != mExtendedSettingsClickListener) {
        mExtendedSettingsClickListener = listener;
        UpdateExtendedSettingsButton();
    }
    return NOERROR;
}

ECode MediaRouteChooserDialog::OnFilterRoute(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isDefault, isEnabled, res;
    route->IsDefault(&isDefault);
    *result = !isDefault && (route->IsEnabled(&isEnabled), isEnabled)
        && (route->MatchesTypes(mRouteTypes, &res), res);
    return NOERROR;
}

ECode MediaRouteChooserDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    Boolean res;
    window->RequestFeature(IWindow::FEATURE_LEFT_ICON, &res);

    SetContentView(R::layout::media_route_chooser_dialog);
    SetTitle(mRouteTypes == IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY
            ? R::string::media_route_chooser_title_for_remote_display
            : R::string::media_route_chooser_title);

    // Must be called after setContentView.
    window->SetFeatureDrawableResource(IWindow::FEATURE_LEFT_ICON,
            R::drawable::ic_media_route_off_holo_dark);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mAdapter = new RouteAdapter(context, this);
    AutoPtr<IView> tempView;
    FindViewById(R::id::media_route_list, (IView**)&tempView);
    mListView = IListView::Probe(tempView);
    IAdapterView* adapterView = IAdapterView::Probe(tempView);
    adapterView->SetAdapter(mAdapter);
    adapterView->SetOnItemClickListener(mAdapter);
    tempView = NULL;
    FindViewById(R::id::empty, (IView**)&tempView);
    adapterView->SetEmptyView(tempView);

    tempView = NULL;
    FindViewById(R::id::media_route_extended_settings_button, (IView**)&tempView);
    mExtendedSettingsButton = IButton::Probe(tempView);
    UpdateExtendedSettingsButton();
    return NOERROR;
}

void MediaRouteChooserDialog::UpdateExtendedSettingsButton()
{
    if (mExtendedSettingsButton != NULL) {
        IView::Probe(mExtendedSettingsButton)->SetOnClickListener(mExtendedSettingsClickListener);
        IView::Probe(mExtendedSettingsButton)->SetVisibility(
            mExtendedSettingsClickListener != NULL ? IView::VISIBLE : IView::GONE);
    }
}

ECode MediaRouteChooserDialog::OnAttachedToWindow()
{
    Dialog::OnAttachedToWindow();

    mAttachedToWindow = TRUE;
    mRouter->AddCallback(mRouteTypes, mCallback, IMediaRouter::CALLBACK_FLAG_PERFORM_ACTIVE_SCAN);
    RefreshRoutes();
    return NOERROR;
}

ECode MediaRouteChooserDialog::OnDetachedFromWindow()
{
    mAttachedToWindow = FALSE;
    mRouter->RemoveCallback(mCallback);

    return Dialog::OnDetachedFromWindow();
}

ECode MediaRouteChooserDialog::RefreshRoutes()
{
    if (mAttachedToWindow) {
        mAdapter->Update();
    }
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
