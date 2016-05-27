
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace EditTextDemo {

static const String DBG_TAG("CActivityOne");

//=======================================================================
// CActivityOne::MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnKeyListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::MyListener::~MyListener()
{
    Logger::D("CActivityOne::MyListener", "destory ~MyListener(): %p", this);
}

ECode CActivityOne::MyListener::OnKey(
    /* [in] */ IView * view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    view->GetId(&id);

    Int32 action;
    event->GetAction(&action);
    Char32 label;
    event->GetDisplayLabel(&label);

    Logger::D("CActivityOne", "CActivityOne::MyListener::OnKey: view %08x, keyCode: %d, '%c'",
        id, keyCode, (char)label);

    if (action == IKeyEvent::ACTION_DOWN) {
        if (keyCode == IKeyEvent::KEYCODE_0) {
            Logger::D("CActivityOne", "Key 0 is down.");
        }
    }
    else if (action == IKeyEvent::ACTION_UP) {
    }

    if (result) {
        *result = FALSE;
    }

    return NOERROR;
}


//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(DBG_TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);

    // Setup ContentView
    //
    SetContentView(R::layout::main);

    AutoPtr<IView> temp = FindViewById(R::id::myEditText);
    mEditText = IEditText::Probe(temp);
    assert(mEditText != NULL);
    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnKeyListener* keyListener = (IViewOnKeyListener*)l.Get();
    IView::Probe(mEditText)->SetOnKeyListener(keyListener);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

} // namespace EditTextDemo
} // namespace DevSamples
} // namespace Elastos
