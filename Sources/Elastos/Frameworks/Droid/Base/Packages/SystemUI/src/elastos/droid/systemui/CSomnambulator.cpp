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

#include "elastos/droid/systemui/CSomnambulator.h"
#include "elastos/droid/systemui/Utils.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentShortcutIconResourceHelper;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntentShortcutIconResourceHelper;
using Elastos::Droid::Content::IIntentShortcutIconResource;
using Elastos::Droid::Service::Dreams::CSandman;
using Elastos::Droid::Service::Dreams::ISandman;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_OBJECT_IMPL(CSomnambulator)
ECode CSomnambulator::constructor()
{
    return NOERROR;
}

ECode CSomnambulator::OnStart()
{
    Activity::OnStart();

    AutoPtr<IIntent> launchIntent;
    GetIntent((IIntent**)&launchIntent);
    String action;
    launchIntent->GetAction(&action);
    if (IIntent::ACTION_CREATE_SHORTCUT.Equals(action)) {
        AutoPtr<IIntent> shortcutIntent;
        AutoPtr<IClassInfo> info = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.CSomnambulator"));
        assert(info != NULL);
        ClassID id;
        info->GetId(&id);
        CIntent::New(this, id, (IIntent**)&shortcutIntent);
        shortcutIntent->SetFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
                | IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<IIntent> resultIntent;
        CIntent::New((IIntent**)&resultIntent);
        AutoPtr<IIntentShortcutIconResourceHelper> helper;
        CIntentShortcutIconResourceHelper::AcquireSingleton((IIntentShortcutIconResourceHelper**)&helper);
        AutoPtr<IIntentShortcutIconResource> sr;
        helper->FromContext(this, R::mipmap::ic_launcher_dreams, (IIntentShortcutIconResource**)&sr);
        resultIntent->PutExtra(IIntent::EXTRA_SHORTCUT_ICON_RESOURCE, IParcelable::Probe(sr));
        resultIntent->PutExtra(IIntent::EXTRA_SHORTCUT_INTENT, IParcelable::Probe(shortcutIntent));
        String value;
        GetString(R::string::start_dreams, &value);
        resultIntent->PutExtra(IIntent::EXTRA_SHORTCUT_NAME, value);
        SetResult(RESULT_OK, resultIntent);
    }
    else {
        Boolean docked = FALSE;
        launchIntent->HasCategory(IIntent::CATEGORY_DESK_DOCK, &docked);
        AutoPtr<ISandman> s;
        CSandman::AcquireSingleton((ISandman**)&s);
        if (docked) {
            s->StartDreamWhenDockedIfAppropriate(this);
        }
        else {
            s->StartDreamByUserRequest(this);
        }
    }
    Finish();
    return NOERROR;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
