
#include "elastos/droid/teleservice/phone/CdmaDisplayInfo.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "R.h"
#include <elastos/droid/R.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/CoreUtils.h>
#include "elastos/core/StringBuilder.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CdmaDisplayInfo::TAG("CdmaDisplayInfo");

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

void CdmaDisplayInfo::DisplayInfoRecord(
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
        IDialogInterface::Probe(sDisplayInfoDialog)->Dismiss();
    }

    // displaying system alert dialog on the screen instead of
    // using another activity to display the message.  This
    // places the message at the forefront of the UI.
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetIcon(Elastos::Droid::R::drawable::ic_dialog_info);
    String text;
    AutoPtr<ICharSequence> obj;
    context->GetText(Elastos::Droid::TeleService::R::string::network_message
        , (ICharSequence**)&obj);
    builder->SetTitle(obj);
    AutoPtr<ICharSequence> cs;
    CString::New(infoMsg, (ICharSequence**)&cs);
    builder->SetMessage(cs);
    builder->SetCancelable(TRUE);
    builder->Create((IAlertDialog**)&sDisplayInfoDialog);

    AutoPtr<IWindow> window;
    IDialog::Probe(sDisplayInfoDialog)->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

    IDialog::Probe(sDisplayInfoDialog)->Show();
    AutoPtr<PhoneGlobals> phoneGlobals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&phoneGlobals);
    phoneGlobals->WakeUpScreen();
}

void CdmaDisplayInfo::DismissDisplayInfoRecord()
{
    if (DBG) Log(String("Dissmissing Display Info Record..."));

    if (sDisplayInfoDialog != NULL) {
        IDialogInterface::Probe(sDisplayInfoDialog)->Dismiss();
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
    Logger::D(TAG, sb.ToString());
    return;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos