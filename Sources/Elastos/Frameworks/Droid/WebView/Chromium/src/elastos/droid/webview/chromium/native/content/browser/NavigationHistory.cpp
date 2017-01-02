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
    AutoPtr<IInterface> entryObj;
    mEntries->Get(index, (IInterface**)&entryObj);
    NavigationEntry* entry = (NavigationEntry*)(IObject::Probe(entryObj));
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
