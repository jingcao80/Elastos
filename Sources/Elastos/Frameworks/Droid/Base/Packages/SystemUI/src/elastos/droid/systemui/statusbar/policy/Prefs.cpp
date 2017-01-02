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

#include "elastos/droid/systemui/statusbar/policy/Prefs.h"
#include "Elastos.Droid.Content.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String Prefs::SHARED_PREFS_NAME = String("status_bar");
AutoPtr<ISharedPreferences> Prefs::Read(
    /* [in] */ IContext* context)
{
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(
        Prefs::SHARED_PREFS_NAME,
        IContext::MODE_PRIVATE,
        (ISharedPreferences**)&sp);
    return sp;
}

AutoPtr<ISharedPreferencesEditor> Prefs::Edit(
    /* [in] */ IContext* context)
{
    AutoPtr<ISharedPreferencesEditor> editor;
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(
        Prefs::SHARED_PREFS_NAME,
        IContext::MODE_PRIVATE,
        (ISharedPreferences**)&sp);
    if (sp)
        sp->Edit((ISharedPreferencesEditor**)&editor);
    return editor;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
