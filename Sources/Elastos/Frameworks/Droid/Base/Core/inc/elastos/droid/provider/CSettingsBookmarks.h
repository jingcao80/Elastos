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

#ifndef __ELASTOS_DROID_PROVIDER_CSETTINGSBOOKMARKS_H__
#define __ELASTOS_DROID_PROVIDER_CSETTINGSBOOKMARKS_H__

#include "_Elastos_Droid_Provider_CSettingsBookmarks.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSettingsBookmarks)
    , public Singleton
    , public ISettingsBookmarks
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Convenience function to retrieve the bookmarked Intent for a
     * particular shortcut key.
     *
     * @param cr The ContentResolver to query.
     * @param shortcut The shortcut key.
     *
     * @return Intent The bookmarked URL, or null if there is no bookmark
     *         matching the given shortcut.
     */
    CARAPI GetIntentForShortcut(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Char32 shortcut,
        /* [out] */ IIntent** intent);

    /**
     * Add a new bookmark to the system.
     *
     * @param cr The ContentResolver to query.
     * @param intent The desired target of the bookmark.
     * @param title Bookmark title that is shown to the user; null if none
     *            or it should be resolved to the intent's title.
     * @param folder Folder in which to place the bookmark; null if none.
     * @param shortcut Shortcut that will invoke the bookmark; 0 if none. If
     *            this is non-zero and there is an existing bookmark entry
     *            with this same shortcut, then that existing shortcut is
     *            cleared (the bookmark is not removed).
     * @return The unique content URL for the new bookmark entry.
     */
    CARAPI Add(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& title,
        /* [in] */ const String& folder,
        /* [in] */ Char32 shortcut,
        /* [in] */ Int32 ordering,
        /* [out] */ IUri** uri);

    /**
     * Return the folder name as it should be displayed to the user.  This
     * takes care of localizing special folders.
     *
     * @param r Resources object for current locale; only need access to
     *          system resources.
     * @param folder The value found in the {@link #FOLDER} column.
     *
     * @return CharSequence The label for this folder that should be shown
     *         to the user.
     */
    CARAPI GetLabelForFolder(
        /* [in] */ IResources* r,
        /* [in] */ const String& folder,
        /* [out] */ ICharSequence** fName);

    /**
     * Return the title as it should be displayed to the user. This takes
     * care of localizing bookmarks that point to activities.
     *
     * @param context A context.
     * @param cursor A cursor pointing to the row whose title should be
     *        returned. The cursor must contain at least the {@link #TITLE}
     *        and {@link #INTENT} columns.
     * @return A title that is localized and can be displayed to the user,
     *         or the empty string if one could not be found.
     */
    CARAPI GetTitle(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** title);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSETTINGSBOOKMARKS_H__
