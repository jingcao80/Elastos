#include "Utils.h"
#include <elastos/Logger.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String Utils::TAG("Utils");
AutoPtr<IToastHelper> Utils::sToastHelper;

void Utils::MakeToast(
    /* [in] */ IContext* context,
    /* [in] */ const String& text,
    /* [in] */ Int32 toastType)
{
    Logger::D(TAG, "MakeToast()---");
    if (sToastHelper == NULL) {
        CToastHelper::AcquireSingleton((IToastHelper**)&sToastHelper);
    }
    StringBuilder sb(text);
    AutoPtr<ICharSequence> seq = sb.ToCharSequence();
    AutoPtr<IToast> toast;
    sToastHelper->MakeText(context, seq, toastType, (IToast**)&toast);
    toast->Show();
}

} // namespace Gallery
