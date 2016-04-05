
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

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
    CArrayList::New((IArrayList**)&mContents);
    CArrayList::New((IArrayList**)&mListeners);
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_FOLDER;
    return NOERROR;
}

ECode FolderInfo::Add(
    /* [in] */ ShortcutInfo* item)
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
    /* [in] */ ShortcutInfo* item)
{
    mContents->Remove(TO_IINTERFACE(item));
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IFolderListener> listener = IFolderListener::Probe(obj);
        listener->OnRemove(item);
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