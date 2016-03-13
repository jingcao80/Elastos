
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/NavigationHistory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                          NavigationHistory
//=====================================================================

ECode NavigationHistory::SetCurrentEntryIndex(
    /* [in] */ Int32 currentEntryIndex)
{
    mCurrentEntryIndex = currentEntryIndex;
    return NOERROR;
}

Int32 NavigationHistory::GetEntryCount()
{
    Int32 size;
    mEntries->GetSize(&size);
    return size;
}

AutoPtr<NavigationEntry> NavigationHistory::GetEntryAtIndex(
    /* [in] */ Int32 index)
{
    AutoPtr<NavigationEntry> entry;
    mEntries->Get(index, (IInterface**)&entry);
    return entry;
}

Int32 NavigationHistory::GetCurrentEntryIndex()
{
    return mCurrentEntryIndex;
}

ECode NavigationHistory::AddEntry(
    /* [in] */ NavigationEntry* entry)
{
    mEntries->Add((IObject*)entry);
    return NOERROR;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
