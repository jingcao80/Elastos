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

#include "CFragmentTwo.h"
#include "CFragmentThree.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

static const String TAG("FragmentDemo::CFragmentTwo");

//=======================================================================
// CFragmentTwo::MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CFragmentTwo::MyListener, Object, IViewOnClickListener)

CFragmentTwo::MyListener::MyListener(
    /* [in] */ CFragmentTwo* host)
    : mHost(host)
{
}

ECode CFragmentTwo::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::I(TAG, "MyListener::OnClick");

    AutoPtr<IFragmentManager> fm;
    mHost->GetFragmentManager((IFragmentManager**)&fm);
    AutoPtr<IFragmentTransaction> trans;
    fm->BeginTransaction((IFragmentTransaction**)&trans);
    trans->Hide(mHost);

    AutoPtr<IFragment> fragment;
    CFragmentThree::New((IFragment**)&fragment);
    trans->Add(R::id::id_content , fragment, String("THREE"));
    trans->Replace(R::id::id_content, fragment, String("THREE"));
    trans->AddToBackStack(String(NULL));
    Int32 ival;
    trans->Commit(&ival);
    return NOERROR;
}

//=======================================================================
// CFragmentTwo
//=======================================================================
CAR_OBJECT_IMPL(CFragmentTwo)

CFragmentTwo::CFragmentTwo()
{
    Logger::I(TAG, " >> Create CFragmentTwo: %p", this);
}

CFragmentTwo::~CFragmentTwo()
{
    Logger::I(TAG, " >> Destroy CFragmentTwo: %p", this);
}

ECode CFragmentTwo::constructor()
{
    return Fragment::constructor();
}

ECode CFragmentTwo::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    Logger::I(TAG, " >> OnCreateView");
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::fragment_two, container, FALSE, (IView**)&view);
    AutoPtr<IView> btnView;
    view->FindViewById(R::id::fragment_two_btn, (IView**)&btnView);

    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    btnView->SetOnClickListener(l);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // namespace FragmentDemo
} // namespace DevSamples
} // namespace Elastos

