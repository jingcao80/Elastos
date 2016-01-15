
#include "CMainActivity.h"
#include "R.h"
#include <elastos/Logger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InteractA {

CAR_INTERFACE_IMPL(CMainActivity::ButtonClickListener, IViewOnClickListener)

CMainActivity::ButtonClickListener::ButtonClickListener(
    /* [in] */ CMainActivity* host)
    : mHost(host)
{}

ECode CMainActivity::ButtonClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId){
        case R::id::btn_getmsg: {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetAction(IIntent::ACTION_SEND);
            intent->SetType(String("text/plain"));
            mHost->StartActivityForResult(intent, REQUEST_CODE);
            Logger::D(TAG, "get text\n");
            break;
        }
        default:
            break;
    }

    return NOERROR;
}


const String CMainActivity::TAG("InteractA.CMainActivity");

ECode CMainActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == REQUEST_CODE) {
        if (resultCode == RESULT_OK) {
            String msg("");
            data->GetStringExtra(String("msg"), &msg);
            AutoPtr<IView> view = FindViewById(R::id::message);
            AutoPtr<ITextView> textview = ITextView::Probe(view);
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(msg, (ICharSequence**)&cs);
            textview->SetText(cs);
        }
    }
    return NOERROR;
}

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);

    mButtonListener = new ButtonClickListener(this);

    AutoPtr<IView> button = FindViewById(R::id::btn_getmsg);
    button->SetOnClickListener(mButtonListener);

    Logger::D(TAG, "OnCreate\n");

    return NOERROR;
}

} //namespace InteractA
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos
