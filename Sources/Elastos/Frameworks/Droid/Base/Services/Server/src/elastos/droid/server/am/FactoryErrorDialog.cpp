
#include "elastos/droid/server/am/FactoryErrorDialog.h"
#include "elastos/droid/R.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

FactoryErrorDialog::MyHandler::MyHandler()
{}

FactoryErrorDialog::MyHandler::~MyHandler()
{}

ECode FactoryErrorDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_RUNTIME_EXCEPTION;
}

FactoryErrorDialog::FactoryErrorDialog(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* msg)
    : BaseErrorDialog(context)
{
    mHandler = new MyHandler();

    SetCancelable(FALSE);
    AutoPtr<ICharSequence> title;
    context->GetText(R::string::factorytest_failed,
            (ICharSequence**)&title);
    SetTitle(title);
    SetMessage(msg);

    AutoPtr<ICharSequence> text;
    context->GetText(R::string::factorytest_reboot, (ICharSequence**)&text);
    AutoPtr<IMessage> msg2;
    mHandler->ObtainMessage(0, (IMessage**)&msg2);
    AlertDialog::SetButton(IDialogInterface::BUTTON_POSITIVE, text, msg2);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    AutoPtr<ICharSequence> fcs;
    CString::New(String("Factory Error"), (ICharSequence**)&fcs);
    attrs->SetTitle(fcs);
    window->SetAttributes(attrs);
}

void FactoryErrorDialog::OnStop()
{}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
