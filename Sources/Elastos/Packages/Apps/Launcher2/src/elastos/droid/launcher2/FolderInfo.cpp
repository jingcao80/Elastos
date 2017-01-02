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

#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "Elastos.Droid.Service.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(FolderInfo, ItemInfo, IFolderInfo);

FolderInfo::FolderInfo()
    : mOpened(FALSE)
{
}

ECode FolderInfo::constructor()
{
    ItemInfo::constructor();
    CArrayList::New((IArrayList**)&mContents);
    CArrayList::New((IArrayList**)&mListeners);
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_FOLDER;
    return NOERROR;
}

ECode FolderInfo::Add(
    /* [in] */ IShortcutInfo* item)
{
    mContents->Add(TO_IINTERFACE(item));
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        IFolderListener::Probe(obj)->OnAdd(item);
    }
    return ItemsChanged();
}

ECode FolderInfo::Remove(
    /* [in] */ IShortcutInfo* item)
{
    mContents->Remove(TO_IINTERFACE(item));
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        IFolderListener::Probe(obj)->OnRemove(item);
    }
    return ItemsChanged();
}

ECode FolderInfo::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        IFolderListener::Probe(obj)->OnTitleChanged(title);
    }
    return NOERROR;
}

ECode FolderInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    ItemInfo::OnAddToDatabase(context, values);
    String str;
    mTitle->ToString(&str);
    return values->Put(LauncherSettings::Favorites::TITLE, str);
}

ECode FolderInfo::AddListener(
    /* [in] */ IFolderListener* listener)
{
    assert(listener);
    return mListeners->Add(listener);
}

ECode FolderInfo::RemoveListener(
    /* [in] */ IFolderListener* listener)
{
    Boolean res;
    mListeners->Contains(TO_IINTERFACE(listener), &res);
    if (res) {
        mListeners->Remove(TO_IINTERFACE(listener));
    }
    return NOERROR;
}

ECode FolderInfo::ItemsChanged()
{
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        IFolderListener::Probe(obj)->OnItemsChanged();
    }
    return NOERROR;
}

ECode FolderInfo::Unbind()
{
    ItemInfo::Unbind();
    return mListeners->Clear();
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos