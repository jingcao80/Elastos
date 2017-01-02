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
#include "LoadImageTask.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace AsyncTaskDemo {

static const String TAG("AsyncTaskDemo::CActivityOne");
static const String sImageUrl("http://fashion.qqread.com/ArtImage/20110225/0083_13.jpg");
static const Int32 MSG_LOAD_SUCCESS = 0;
static const Int32 MSG_LOAD_FAILURE = 1;

//=======================================================================
// MyListener
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
    Logger::I(TAG, "MyListener::OnClick");

    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
    params->Set(0, CoreUtils::Convert(sImageUrl));

    AutoPtr<IContext> ctx;
    v->GetContext((IContext**)&ctx);
    AutoPtr<LoadImageTask> task = new LoadImageTask(mHost, ctx);
    task->Execute(params);
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

CActivityOne::CActivityOne()
{}

CActivityOne::~CActivityOne()
{
    Logger::I(TAG, " >> Destroy AysncTask::CActivityOne");
}

UInt32 CActivityOne::AddRef()
{
    UInt32 count = Activity::AddRef();
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    // Logger::I(TAG, "AysncTaskActivity::AddRef, refcount: %d, callstack:\n%s", count, backtrace.string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    return count;
}

UInt32 CActivityOne::Release()
{
    UInt32 count = Activity::Release();
    // Logger::I(TAG, "AysncTaskActivity::Release, refcount: %d", count);
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "-------------------------------------------------------");
    // Logger::I(TAG, "AysncTaskActivity::Release, refcount: %d, callstack:\n%s", GetStrongCount(), backtrace.string());
    // Logger::I(TAG, "-------------------------------------------------------");
    return count;
}

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::ImageVivew);
    mImageView = IImageView::Probe(view);

    view = FindViewById(R::id::LoadButton);
    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());
    mLoadButton = IButton::Probe(view);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}


} // namespace AsyncTaskDemo
} // namespace DevSamples
} // namespace Elastos
