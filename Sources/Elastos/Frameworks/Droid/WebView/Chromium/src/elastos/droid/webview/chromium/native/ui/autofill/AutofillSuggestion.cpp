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

#include "elastos/droid/webkit/webview/chromium/native/ui/autofill/AutofillSuggestion.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Autofill {

//=====================================================================
//                          AutofillSuggestion
//=====================================================================
AutofillSuggestion::AutofillSuggestion(
    /* [in] */ const String& name,
    /* [in] */ const String& label,
    /* [in] */ Int32 uniqueId)
    : mLabel(name)
    , mSublabel(label)
    , mUniqueId(uniqueId)
{
    // ==================before translated======================
    // mLabel = name;
    // mSublabel = label;
    // mUniqueId = uniqueId;
}

String AutofillSuggestion::GetLabel()
{
    // ==================before translated======================
    // return mLabel;

    return mLabel;
}

String AutofillSuggestion::GetSublabel()
{
    // ==================before translated======================
    // return mSublabel;

    return mSublabel;
}

Boolean AutofillSuggestion::IsEnabled()
{
    // ==================before translated======================
    // return true;

    return TRUE;
}

Boolean AutofillSuggestion::IsGroupHeader()
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

Int32 AutofillSuggestion::GetUniqueId()
{
    // ==================before translated======================
    // return mUniqueId;

    return mUniqueId;
}

} // namespace Autofill
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


