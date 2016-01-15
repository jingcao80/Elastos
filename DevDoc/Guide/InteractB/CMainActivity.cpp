
#include "CMainActivity.h"
#include "R.h"
#include <elastos/Logger.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InteractB {

CAR_INTERFACE_IMPL(CMainActivity::ButtonClickListener, IViewOnClickListener)

CMainActivity::ButtonClickListener::ButtonClickListener(
    /* [in] */CMainActivity* host)
    : mHost(host)
{}

ECode CMainActivity::ButtonClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId){
        case R::id::btn_finish: {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<IView> msg = mHost->FindViewById(R::id::edt_text);
            AutoPtr<IEditText> text = IEditText::Probe(msg);
            AutoPtr<ICharSequence> msg_text;
            text->GetText((ICharSequence**)&msg_text);
            String msgs("");
            msg_text->ToString(&msgs);
            intent->PutStringExtra(String("msg"), msgs);
            mHost->SetResultEx(RESULT_OK, intent);
            mHost->Finish();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

const String CMainActivity::TAG("InteractB.CMainActivity");

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);

    mButtonListener = new ButtonClickListener(this);

    AutoPtr<IView> button = FindViewById(R::id::btn_finish);
    button->SetOnClickListener(mButtonListener);

    Logger::D(TAG, "OnCreate\n");
    return NOERROR;
}

} //namespace InteractB
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos
