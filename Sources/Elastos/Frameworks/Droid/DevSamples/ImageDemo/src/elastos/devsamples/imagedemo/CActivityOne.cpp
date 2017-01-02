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
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ImageDemo {

static const String TAG("ImageDemo::CActivityOne");

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
    return NOERROR;
}

CActivityOne::OrientationListener::OrientationListener()
{}

CActivityOne::OrientationListener::~OrientationListener()
{
    Logger::I("OrientationListener", " >> Destroy OnOrientationChanged: %p", this);
}

ECode CActivityOne::OrientationListener::OnOrientationChanged(
    /* [in] */ Int32 orientation)
{
    // Logger::I("OrientationListener", " >> OnOrientationChanged: %d", orientation);
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

CActivityOne::CActivityOne()
    : mDestroyed(0)
{}

CActivityOne::~CActivityOne()
{
    Logger::I(TAG, " >> Destroy ImageDemo::CActivityOne");
}

UInt32 CActivityOne::AddRef()
{
    UInt32 count = Activity::AddRef();
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    // Logger::I(TAG, "ImageDemoActivity::AddRef, refcount: %d, callstack:\n%s", count, backtrace.string());
    // Logger::I(TAG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    return count;
}

UInt32 CActivityOne::Release()
{
    UInt32 count = Activity::Release();
    // Logger::I(TAG, "ImageDemoActivity::Release, refcount: %d", count);
    // android::CallStack stack;
    // stack.update();
    // String backtrace(stack.toString("").string());
    // Logger::I(TAG, "-------------------------------------------------------");
    // Logger::I(TAG, "ImageDemoActivity::Release, refcount: %d, callstack:\n%s", GetStrongCount(), backtrace.string());
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

    AutoPtr<IView> view = FindViewById(R::id::ImageView);
    IImageView* imageView = IImageView::Probe(view);
    imageView->SetImageResource(R::drawable::cupple);
    imageView->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER);

    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

    SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR);

    // setup orientation evnet listener.
    mOrientationListener = new OrientationListener();
    mOrientationListener->constructor(this);
    mOrientationListener->Enable();

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
    ECode ec =  Activity::OnDestroy();
    mDestroyed = 1;
    return ec;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

ECode CActivityOne::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Logger::I(TAG, " >> OnConfigurationChanged: %s", TO_CSTR(newConfig));

    Int32 orientation;
    newConfig->GetOrientation(&orientation);
    if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
        Logger::I(TAG, " >> OnConfigurationChanged: LANDSCAPE");
    }
    else if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
        Logger::I(TAG, " >> OnConfigurationChanged: PORTRAIT");
    }
    else {
        Logger::I(TAG, " >> OnConfigurationChanged: UNDEFINED");
    }

    return Activity::OnConfigurationChanged(newConfig);
}


} // namespace ImageDemo
} // namespace DevSamples
} // namespace Elastos
