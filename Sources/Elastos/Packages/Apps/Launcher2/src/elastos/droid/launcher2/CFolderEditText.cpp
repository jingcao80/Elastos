
#include "elastos/droid/launcher2/CFolderEditText.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CFolderEditText, EditText, IFolderEditText);

ECode CFolderEditText::SetFolder(
    /* [in] */ IFolder* folder)
{
    mFolder = folder;
    return NOERROR;
}

ECode CFolderEditText::OnKeyPreIme(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    // Catch the back button on the soft keyboard so that we can just close the activity
    Int32 code;
    event->GetKeyCode(&code);
    if (code == IKeyEvent::KEYCODE_BACK) {
        mFolder->DoneEditingFolderName(TRUE);
    }
    return EditText::OnKeyPreIme(keyCode, event, result);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos