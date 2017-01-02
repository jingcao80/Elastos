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

#include "CFragmentThree.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

static const String TAG("FragmentDemo::CFragmentThree");

//=======================================================================
// CFragmentThree::MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CFragmentThree::MyListener, Object, IViewOnClickListener)

CFragmentThree::MyListener::MyListener(
    /* [in] */ CFragmentThree* host)
    : mHost(host)
{
}

ECode CFragmentThree::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::I(TAG, "MyListener::OnClick");

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<ICharSequence> info = CoreUtils::Convert("Click in Fragment Three");

    AutoPtr<IToast> toast;
    helper->MakeText(context, info, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    return NOERROR;
}

//=======================================================================
// CFragmentThree
//=======================================================================
CAR_OBJECT_IMPL(CFragmentThree)

CFragmentThree::CFragmentThree()
{
    Logger::I(TAG, " >> Create CFragmentThree: %p", this);
}

CFragmentThree::~CFragmentThree()
{
    Logger::I(TAG, " >> Destroy CFragmentThree: %p", this);
}

ECode CFragmentThree::constructor()
{
    return Fragment::constructor();
}

ECode CFragmentThree::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    Logger::I(TAG, " >> OnCreateView");
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::fragment_three, container, FALSE, (IView**)&view);
    AutoPtr<IView> btnView;
    view->FindViewById(R::id::fragment_three_btn, (IView**)&btnView);

    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    btnView->SetOnClickListener(l);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace FragmentDemo
} // namespace DevSamples
} // namespace Elastos

