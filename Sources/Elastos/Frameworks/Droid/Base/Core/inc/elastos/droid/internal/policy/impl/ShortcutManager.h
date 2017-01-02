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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_SHORTCUTMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_SHORTCUTMANAGER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/utility/SparseArray.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
 * Manages quick launch shortcuts by:
 * <li> Keeping the local copy in sync with the database (this is an observer)
 * <li> Returning a shortcut-matching intent to clients
 */
class ShortcutManager
    : public ContentObserver
{
public:
    TO_STRING_IMPL("ShortcutManager")

    ShortcutManager(
        /* [in] */ IContext* context);

    /** Observes the provider of shortcut+intents */
    CARAPI_(void) Observe();

    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    /**
     * Gets the shortcut intent for a given keycode+modifier. Make sure you
     * strip whatever modifier is used for invoking shortcuts (for example,
     * if 'Sym+A' should invoke a shortcut on 'A', you should strip the
     * 'Sym' bit from the modifiers before calling this method.
     * <p>
     * This will first try an exact match (with modifiers), and then try a
     * match without modifiers (primary character on a key).
     *
     * @param kcm The key character map of the device on which the key was pressed.
     * @param keyCode The key code.
     * @param metaState The meta state, omitting any modifiers that were used
     * to invoke the shortcut.
     * @return The intent that matches the shortcut, or null if not found.
     */
    CARAPI_(AutoPtr<IIntent>) GetIntent(
        /* [in] */ IKeyCharacterMap* kcm,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 metaState);

private:
    CARAPI_(void) UpdateShortcuts();

    static CARAPI_(AutoPtr< ArrayOf<String> >) InitProjection();

private:
    static const String TAG;

    static const Int32 COLUMN_SHORTCUT;
    static const Int32 COLUMN_INTENT;
    static const AutoPtr< ArrayOf<String> > sProjection;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ICursor> mCursor;
    /** Map of a shortcut to its intent. */
    AutoPtr< HashMap <Int32, AutoPtr<IInterface> > > mShortcutIntents;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_SHORTCUTMANAGER_H__
