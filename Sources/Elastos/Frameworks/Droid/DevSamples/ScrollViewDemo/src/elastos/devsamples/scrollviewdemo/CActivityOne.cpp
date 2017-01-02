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

#include "CActivityOne.h"
#include <stdio.h>
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace DevSamples {
namespace ScrollViewDemo {

//=======================================================================
// CActivityOne::MyListener::
//=======================================================================
CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
   /* [in] */ IView* v)
{
    Slogger::D("CActivityOne", "............................The Button in ScrollView was Clicked");

    Int32 height = 0;
    IView::Probe(mHost->mScroll)->GetHeight(&height);
    Slogger::D("CActivityOne", "mScroll height = %d", height);

    Int32 count = 0, len = 0;
    IViewGroup::Probe(mHost->mScroll)->GetChildCount(&count);
    Slogger::D("CActivityOne", "mScroll childlen count = %d", count);

    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mHost->mScroll)->GetChildAt(i, (IView**)&v);
        Int32 childlen = 0;
        v->GetHeight(&childlen);
        len += childlen;
        Slogger::D("CActivityOne", "childlen = %d, len = %d", childlen, len);
    }
    mHost->mScroll->Fling(2000);
    return NOERROR;
}

//=======================================================================
// CActivityOne::
//=======================================================================
CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Slogger::D("CActivityOne", "CActivityOnefgfg::constructor");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D("CActivityOne", "CActivityOne::OnCreate");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    // AutoPtr<IView> v = FindViewById(R::id::view3);
    // AutoPtr<IEditText> mEdit = IEditText::Probe(v);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    PRINT_ENTER_LEAVE("OnActivityResult");
    return NOERROR;
}

} // namespace ScrollViewDemo
} // namespace DevSamples
} // namespace Elastos
