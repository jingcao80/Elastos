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

#include "elastos/droid/internal/app/ChooserActivity.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(ChooserActivity, ResolverActivity, IChooserActivity)

ECode ChooserActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> targetParcelable;
    intent->GetParcelableExtra(IIntent::EXTRA_INTENT, (IParcelable**)&targetParcelable);
    IIntent* target = IIntent::Probe(targetParcelable);
    if (!target) {
        String str;
        IObject::Probe(targetParcelable)->ToString(&str);
        Logger::W("ChooserActivity", "Target is not an intent: %s", str.string());
        Finish();
        return ResolverActivity::OnCreate(NULL);
    }
    ModifyTargetIntent(target);

    intent->GetBundleExtra(IIntent::EXTRA_REPLACEMENT_EXTRAS, (IBundle**)&mReplacementExtras);
    AutoPtr<ICharSequence> title;
    intent->GetCharSequenceExtra(IIntent::EXTRA_TITLE, (ICharSequence**)&title);
    Int32 defaultTitleRes = 0;
    if (title == NULL) {
        defaultTitleRes = R::string::chooseActivity;
    }
    AutoPtr<ArrayOf<IParcelable*> > pa;
    intent->GetParcelableArrayExtra(IIntent::EXTRA_INITIAL_INTENTS, (ArrayOf<IParcelable*>**)&pa);
    AutoPtr<ArrayOf<IIntent*> > initialIntents;
    if (pa != NULL) {
        initialIntents = ArrayOf<IIntent*>::Alloc(pa->GetLength());
        for (Int32 i = 0; i < pa->GetLength(); i++) {
            IIntent* in = IIntent::Probe((*pa)[i]);
            if (!in) {
                String str;
                IObject::Probe((*pa)[i])->ToString(&str);
                Logger::W("ChooserActivity", "Initial intent #%d not an Intent: ", i, str.string());
                Finish();
                return ResolverActivity::OnCreate(NULL);
            }
            ModifyTargetIntent(in);
            initialIntents->Set(i, in);
        }
    }
    SetSafeForwardingMode(TRUE);
    return ResolverActivity::OnCreate(savedInstanceState, target, title, defaultTitleRes, initialIntents,
            NULL, FALSE);
}

ECode ChooserActivity::GetReplacementIntent(
    /* [in] */ const String& packageName,
    /* [in] */ IIntent* defIntent,
    /* [out] */ IIntent** outIntent)
{
    VALIDATE_NOT_NULL(outIntent);
    if (mReplacementExtras != NULL) {
        AutoPtr<IBundle> replExtras;
        mReplacementExtras->GetBundle(packageName, (IBundle**)&replExtras);
        if (replExtras != NULL) {
            CIntent::New(defIntent, outIntent);
            (*outIntent)->PutExtras(replExtras);
            return NOERROR;
        }
    }
    *outIntent = defIntent;
    REFCOUNT_ADD(*outIntent);
    return NOERROR;
}

void ChooserActivity::ModifyTargetIntent(
    /* [in] */ IIntent* in)
{
    String action;
    in->GetAction(&action);
    if (IIntent::ACTION_SEND.Equals(action) ||
        IIntent::ACTION_SEND_MULTIPLE.Equals(action)) {
        in->AddFlags(IIntent::FLAG_ACTIVITY_NEW_DOCUMENT |
            IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
    }
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
