
#include "elastos/droid/launcher2/FolderInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

FolderInfo::FolderInfo()
    : mOpened(FALSE)
{
    CArrayList::New((IArrayList**)&mContents);
    CArrayList::New((IArrayList**)&mListeners);
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_FOLDER;
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
        AutoPtr<IFolderListener> listener = IFolderListener::Probe(obj);
        listener->OnAdd(item);
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
        AutoPtr<IFolderListener> listener = IFolderListener::Probe(obj);
        listener->onRemove(item);
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
        AutoPtr<IFolderListener> listener = IFolderListener::Probe(obj);
        listener->OnTitleChanged(title);
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
    return mListeners->Add(TO_IINTERFACE(listener));
}

ECode FolderInfo::RemoveListener(
    /* [in] */ IFolderListener* listener)
{
    Boolean res;
    listeners->Contains(TO_IINTERFACE(listener), &res);
    if (res) {
        listeners->Remove(TO_IINTERFACE(listener));
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
        AutoPtr<IFolderListener> listener = IFolderListener::Probe(obj);
        listener->OnItemsChanged();
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