
#include "elastos/droid/phone/CdmaDisplayInfo.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const String CdmaDisplayInfo::LOG_TAG("CdmaDisplayInfo");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);
    Boolean result = value == 1;
    return result;
}

Boolean CdmaDisplayInfo::DBG = initDBG();

AutoPtr<IAlertDialog> CdmaDisplayInfo::sDisplayInfoDialog;

ECode CdmaDisplayInfo::DisplayInfoRecord(
    /* [in] */ IContext* context,
    /* [in] */ const String& infoMsg)
{
    if (DBG) {
        StringBuilder sb;
        sb += "displayInfoRecord: infoMsg=";
        sb += infoMsg;
        Log(sb.ToString());
    }

    if (sDisplayInfoDialog != NULL) {
        sDisplayInfoDialog->Dismiss();
    }

    // displaying system alert dialog on the screen instead of
    // using another activity to display the message.  This
    // places the message at the forefront of the UI.
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetIcon(android.R.drawable.ic_dialog_info);
    String text;
    context->GetText(R.string.network_message, &text);
    AutoPtr<ICharSequence> obj = CoreUtil::Convert(text);
    builder->SetTitle(obj);
    builder->SetMessage(infoMsg);
    builder->SetCancelable(TRUE);
    builder->Create((IAlertDialog**)&sDisplayInfoDialog);

    AutoPtr<IWindow> window;
    sDisplayInfoDialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

    sDisplayInfoDialog->Show();
    AutoPtr<IPhoneGlobals> phoneGlobals = PhoneGlobals::GetInstance();
    return phoneGlobals->WakeUpScreen();
}

void CdmaDisplayInfo::DismissDisplayInfoRecord()
{
    if (DBG) Log(String("Dissmissing Display Info Record..."));

    if (sDisplayInfoDialog != NULL) {
        sDisplayInfoDialog->Dismiss();
        sDisplayInfoDialog = NULL;
    }
    return;
}

void CdmaDisplayInfo::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[CdmaDisplayInfo] ";
    sb += msg;
    Logger::D(LOG_TAG, sb.ToString());
    return;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos