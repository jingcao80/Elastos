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

#ifndef  __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Represents a folder containing shortcuts or apps.
 */
class FolderInfo
    : public ItemInfo
    , public IFolderInfo
{
public:
    CAR_INTERFACE_DECL();

    FolderInfo();

    CARAPI constructor();

    /**
     * Add an app or shortcut
     *
     * @param item
     */
    CARAPI Add(
        /* [in] */ IShortcutInfo* item);

    /**
     * Remove an app or shortcut. Does not change the DB.
     *
     * @param item
     */
    CARAPI Remove(
        /* [in] */ IShortcutInfo* item);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI OnAddToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values);

    CARAPI AddListener(
        /* [in] */ IFolderListener* listener);

    CARAPI RemoveListener(
        /* [in] */ IFolderListener* listener);

    CARAPI ItemsChanged();

    //@Override
    CARAPI Unbind();

public:
    /**
     * Whether this folder has been opened
     */
    Boolean mOpened;

    /**
     * The apps and shortcuts
     */
    AutoPtr<IArrayList> mContents;

    AutoPtr<IArrayList> mListeners;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__