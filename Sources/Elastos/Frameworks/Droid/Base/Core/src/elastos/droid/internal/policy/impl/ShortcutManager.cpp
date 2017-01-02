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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/policy/impl/ShortcutManager.h"
#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/provider/CSettingsBookmarks.h"
#include <elastos/core/Character.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Character;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Provider::CSettingsBookmarks;
using Elastos::Droid::Provider::ISettingsBookmarks;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

const String ShortcutManager::TAG("ShortcutManager");

const Int32 ShortcutManager::COLUMN_SHORTCUT = 0;
const Int32 ShortcutManager::COLUMN_INTENT = 1;
const AutoPtr< ArrayOf<String> > ShortcutManager::sProjection = ShortcutManager::InitProjection();

ShortcutManager::ShortcutManager(
    /* [in] */ IContext* context)
    : mContext(context)
{
    mShortcutIntents = new HashMap< Int32, AutoPtr<IInterface> >();
}

void ShortcutManager::Observe()
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsBookmarks> settingsBookmarks;
    CSettingsBookmarks::AcquireSingleton((ISettingsBookmarks**)&settingsBookmarks);
    AutoPtr<IUri> contentUri;
    settingsBookmarks->GetCONTENT_URI((IUri**)&contentUri);
    contentResolver->Query(
        contentUri, sProjection, String(NULL), NULL, String(NULL), (ICursor**)&mCursor);
    if (mCursor) {
        mCursor->RegisterContentObserver(this);
    }
    UpdateShortcuts();
}

ECode ShortcutManager::OnChange(
    /* [in] */ Boolean selfChange)
{
    UpdateShortcuts();
    return NOERROR;
}

void ShortcutManager::UpdateShortcuts()
{
    AutoPtr<ICursor> c = mCursor;
    Boolean isRequerySuccess = FALSE;
    if (c == NULL || (c->Requery(&isRequerySuccess), !isRequerySuccess)) {
        Slogger::E(TAG, "ShortcutObserver could not re-query shortcuts.");
        return;
    }

    mShortcutIntents->Clear();
    Boolean isMoveSuccess = FALSE;
    while (c->MoveToNext(&isMoveSuccess), isMoveSuccess) {
        Int32 shortcut = 0;
        c->GetInt32(COLUMN_SHORTCUT, &shortcut);
        if (shortcut == 0) {
            continue;
        }
        String intentURI;
        c->GetString(COLUMN_INTENT, &intentURI);
        AutoPtr<IIntent> intent;
        AutoPtr<IIntentHelper> intentHelper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&intentHelper);
        // try {
        ECode ec = intentHelper->GetIntent(intentURI, (IIntent**)&intent);
        // } catch (URISyntaxException e) {
        if (FAILED(ec)) {
            Slogger::W(TAG, "Intent URI for shortcut invalid.");
        }
        // }
        if (intent == NULL) {
            continue;
        }
        (*mShortcutIntents)[shortcut] = intent;
    }
}

AutoPtr<IIntent> ShortcutManager::GetIntent(
    /* [in] */ IKeyCharacterMap* kcm,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState)
{
    AutoPtr<IIntent> intent;

    // First try the exact keycode (with modifiers).
    Int32 shortcut = 0;
    kcm->Get(keyCode, metaState, &shortcut);
    if (shortcut != 0) {
        HashMap <Int32, AutoPtr<IInterface> >::Iterator it;
        it = mShortcutIntents->Find(shortcut);
        if (it != mShortcutIntents->End()) {
            AutoPtr<IInterface> obj = it->mSecond;
            intent = IIntent::Probe(obj);
        }
    }

    // Next try the primary character on that key.
    if (intent == NULL) {
        Char32 lable;
        kcm->GetDisplayLabel(keyCode, &lable);
        shortcut = Character::ToLowerCase(lable);
        if (shortcut != 0) {
            HashMap <Int32, AutoPtr<IInterface> >::Iterator it;
            it = mShortcutIntents->Find(shortcut);
            if (it != mShortcutIntents->End()) {
                AutoPtr<IInterface> obj = it->mSecond;
                intent = IIntent::Probe(obj);
            }
        }
    }

    return intent;
}

AutoPtr< ArrayOf<String> > ShortcutManager::InitProjection()
{
    AutoPtr< ArrayOf<String> > projection = ArrayOf<String>::Alloc(2);
    (*projection)[0] = ISettingsBookmarks::SHORTCUT;
    (*projection)[1] = ISettingsBookmarks::INTENT;
    return projection;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
