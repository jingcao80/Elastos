
#include "elastos/droid/launcher2/LauncherModel.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherModel::ShortcutNameComparator, Object,
        IComparator);

LauncherModel::ShortcutNameComparator::ShortcutNameComparator()
{
    CHashMap::New((HashMap**)&mLabelCache);
    Collator::GetInstance((Collator**)&mCollator);
}

LauncherModel::ShortcutNameComparator::ShortcutNameComparator(
    /* [in] */ IHashMap* labelCache)
    : mLabelCache(labelCache)
{
    Collator::GetInstance((Collator**)&mCollator);
}

ECode LauncherModel::ShortcutNameComparator::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> labelA, labelB;
    AutoPtr<ILauncherActivityInfo> _a = ILauncherActivityInfo::Probe(a);
    AutoPtr<ILauncherActivityInfo> _b = ILauncherActivityInfo::Probe(b);
    AutoPtr<IComponentName> keyA;
    _a.getComponentName((IComponentName)&keyA);
    AutoPtr<IComponentName> keyB;
    _b.getComponentName((IComponentName)&keyB);
    if (mLabelCache.containsKey(keyA)) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(keyA, (IInterface**)&obj);
        labelA = ICharSequence::Probe(obj);
    }
    else {
        _a->GetLabel((ICharSequence**)&labelA);

        mLabelCache->Put(keyA, TO_IINTERFACE(labelA));
    }
    Boolean res;
    mLabelCache->ContainsKey(keyB, &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(keyB, (IInterface**)&obj);
        labelB = ICharSequence::Probe(obj);
    }
    else {
        _b->GetLabel((ICharSequence**)&labelB);

        mLabelCache->Put(keyB, TO_IINTERFACE_(labelB));
    }
    return mCollator->Compare(labelA, labelB, &result);
}

CAR_INTERFACE_IMPL(LauncherModel::WidgetAndShortcutNameComparator, Object,
        IComparator);

LauncherModel::WidgetAndShortcutNameComparator::WidgetAndShortcutNameComparator(
    /* [in] */ IPackageManager* pm)
    : mPackageManager(pm)
{
    CHashMap::New((IHashMap)&mLabelCache);
    Collator::GetInstance((Collator**)&mCollator);
}

ECode LauncherModel::WidgetAndShortcutNameComparator::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String labelA, labelB;
    Boolean res;
    mLabelCache->ContainsKey(TO_IINTERFACE(a), &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(TO_IINTERFACE(a), &obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&labelA);
    }
    else {
        AutoPtr<ICharSequence> cchar;
        if (IAppWidgetProviderInfo::Probe(a) != NULL) {
            IAppWidgetProviderInfo::Probe(a)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(labelA);
        }
        else {
            IResolveInfo::Probe(a)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(labelA);
        }
        mLabelCache->Put(TO_IINTERFACE(a), TO_IINTERFACE(cchar));
    }
    mLabelCache->ContainsKey(TO_IINTERFACE(b), &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(TO_IINTERFACE(b), &obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&labelB);
    }
    else {
        AutoPtr<ICharSequence> cchar;
        if (IAppWidgetProviderInfo::Probe(b) != NULL) {
            IAppWidgetProviderInfo::Probe(b)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(labelB);
        }
        else {
            IResolveInfo::Probe(b)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(labelB);
        }
        mLabelCache->Put(TO_IINTERFACE(b), TO_IINTERFACE(cchar));
    }
    Int32 compareResult;
    mCollator->Compare(labelA, labelB, &compareResult);
    if (compareResult != 0) {
        *result = compareResult;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

LauncherModel::MyRunnable::MyRunnable(
    /* [in] */ IArrayList* tmpWorkspaceItems,
    /* [in] */ IArrayList* tmpAppWidgets)
    : mTmpWorkspaceItems(tmpWorkspaceItems)
    , mTmpAppWidgets(tmpAppWidgets)
{
}

ECode LauncherModel::MyRunnable::Run()
{
    Int32 size;
    mTmpWorkspaceItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTmpWorkspaceItems->Get(i, (IInterface**)&obj);
        AutoPtr<IItemInfo> item = IItemInfo::Probe(obj);
        item->Unbind();
    }

    tmpAppWidgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        tmpAppWidgets->Get(i, (IInterface**)&obj);
        AutoPtr<IItemInfo> item = IItemInfo::Probe(obj);
        item->Unbind();
    }
    return NOERROR;
}

LauncherModel::MyRunnable2::MyRunnable2(
    /* [in] */ LauncherModel* host,
    /* [in] */ Int64 itemId,
    /* [in] */ ItemInfo* item)
    : mHost(host)
    , mItemId(itemId)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable2::Run()
{
    synchronized(mHost->sBgLock) {
        mHost->CheckItemInfoLocked(mItemId, mItem);
    }
    return NOERROR;
}

LauncherModel::MyRunnable3::MyRunnable3(
    /* [in] */ LauncherModel* host,
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ Int64 itemId,
    /* [in] */ ItemInfo* item)
    : mHost(host)
    , mCr(cr)
    , mUri(uri)
    , mValues(values)
    , mItemId(itemId)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable3::Run()
{
    mCr->Update(mUri, mValues, NULL, NULL);

    // Lock on mBgLock *after* the db operation
    synchronized(mHost->sBgLock) {
        CheckItemInfoLocked(mItemId, mItem);

        if (mItem->mContainer != LauncherSettings::Favorites::CONTAINER_DESKTOP &&
                mItem->mCcontainer != LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            // Item is in a folder, make sure this folder exists
            Boolean res;
            sBgFolders->ContainsKey(mItem->mContainer, &res);
            if (!res) {
                // An items container is being set to a that of an item which is not in
                // the list of Folders.
                StringBuilder sb;
                sb += "item: ";
                sb += mItem;
                sb += " container being set to: ";
                sb += mItem->mContainer;
                sb += ", not in the list of folders";
                String msg = sb.ToString();
                Slogger::E(TAG, msg);
                Launcher::DumpDebugLogsToConsole();
            }
        }

        // Items are added/removed from the corresponding FolderInfo elsewhere, such
        // as in Workspace.onDrop. Here, we just add/remove them from the list of items
        // that are on the desktop, as appropriate
        AutoPtr<IInterface> obj;
        sBgItemsIdMap->Get(itemId, (IInterface**)&obj);
        AutoPtr<IItemInfo> modelItem = IItemInfo::Probe(obj);
        ItemInfo* _modelItem = (ItemInfo*)modelItem;
        if (_modelItem->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP ||
                _modelItem->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            switch (modelItem->mItemType) {
                case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
                case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
                case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
                {
                    Boolean res;
                    sBgWorkspaceItems->Contains(modelItem, &res);
                    if (!res) {
                        sBgWorkspaceItems->Add(TO_IINTERFACE(modelItem));
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else {
            sBgWorkspaceItems->Remove(TO_IINTERFACE(modelItem));
        }
    }
    return NOERROR;
}

LauncherModel::MyRunnable4::MyRunnable4(
    /* [in] */ LauncherModel* host)
    : mHost(host)
{
}

ECode LauncherModel::MyRunnable4::Run()
{
    synchronized(mHost) {
        NotifyAll();
        mHost->mFlushingWorkerThread = FALSE;
    }
    return NOERROR;
}

LauncherModel::MyRunnable5::MyRunnable5(
    /* [in] */ LauncherModel* host,
    /* [in] */ IContentResolver* cr;
    /* [in] */ Boolean notify,
    /* [in] */ IContentValues* values,
    /* [in] */ ItemInfo* item)
    : mHost(host)
    , mCr(cr)
    , mNotify(notify)
    , mValues(values)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable5::Run()
{
    String transaction = "DbDebug    Add item (" + item.title + ") to db, id: "
            + item.id + " (" + container + ", " + screen + ", " + cellX + ", "
            + cellY + ")";
    Launcher::sDumpLogs->Add(transaction);
    Slogger::D(TAG, transaction);

    mCr->Insert(mNotify ? LauncherSettings::Favorites::CONTENT_URI :
            LauncherSettings::Favorites::CONTENT_URI_NO_NOTIFICATION, values);

    // Lock on mBgLock *after* the db operation
    synchronized(mHost->sBgLock) {
        CheckItemInfoLocked(mItem->mId, item/*, stackTrace*/);
        mHost->sBgItemsIdMap->Put(mItem->mId, TO_IINTERFACE(item));
        switch (mItem->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
                sBgFolders->Put(mItem->mId, (FolderInfo) item);
                // Fall through
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
            {
                if (mItem->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP ||
                        mItem->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
                    mHost->sBgWorkspaceItems->Add(TO_IINTERFACE(item));
                }
                else {
                    Boolean res;
                    mHost->sBgFolders->ContainsKey(mItem->mContainer, &res);
                    if (!res) {
                        // Adding an item to a folder that doesn't exist.
                        String msg = "adding item: " + item + " to a folder that " +
                                " doesn't exist";
                        Slogger::E(TAG, msg);
                        Launcher::DumpDebugLogsToConsole();
                    }
                }
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                mHost->sBgAppWidgets->Add(TO_IINTERFACE(item)); //(LauncherAppWidgetInfo) item
                break;
        }
    }
    return NOERROR;
}

LauncherModel::MyRunnable6::MyRunnable6(
    /* [in] */ LauncherModel* host,
    /* [in] */ IContentResolver* cr;
    /* [in] */ IUri* uriToDelete,
    /* [in] */ IContentValues* values,
    /* [in] */ ItemInfo* item)
    : mHost(host)
    , mCr(cr)
    , mUriToDelete(uriToDelete)
    , mValues(values)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable6::Run()
{
    String transaction = "DbDebug    Delete item (" + item.title + ") from db, id: "
            + item.id + " (" + item.container + ", " + item.screen + ", " + item.cellX +
            ", " + item.cellY + ")";
    Launcher::sDumpLogs->Add(transaction);
    Slogger::D(TAG, transaction);

    mCr->Delete(mUriToDelete, NULL, NULL);

    // Lock on mBgLock *after* the db operation
    synchronized(mHost->sBgLock) {
        switch (mItem->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
            {
                mHost->sBgFolders->Remove(mItem->mId);
                AutoPtr<ICollection> value;
                sBgItemsIdMap->GetValues((ICollection**)&value);
                AutoPtr<ArrayOf<IInterface*> > array;
                value->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); i++) {
                    AutoPtr<IItemInfo> info = IItemInfo::Probe((*array)[i]);

                    if (mItem->mContainer == mItem->mId) {
                        // We are deleting a folder which still contains items that
                        // think they are contained by that folder.
                        String msg = "deleting a folder (" + item + ") which still " +
                                "contains items (" + info + ")";
                        Slogger::E(TAG, msg);
                        Launcher::DumpDebugLogsToConsole();
                    }
                }
                mHost->sBgWorkspaceItems->Remove(item);
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
                mHost->sBgWorkspaceItems->Remove(item);
                break;
            case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                mHost->sBgAppWidgets->Remove((LauncherAppWidgetInfo) item);
                break;
        }
        sBgItemsIdMap->Remove(mItem->mId);
        sBgDbIconCache->Remove(mItem);
    }
    return NOERROR;
}

LauncherModel::MyRunnable7::MyRunnable7(
    /* [in] */ LauncherModel* host,
    /* [in] */ IContentResolver* cr;
    /* [in] */ FolderInfo* info)
    : mHost(host)
    , mCr(cr)
    , mInfo(info)
{
}

ECode LauncherModel::MyRunnable7::Run()
{
    mCr->Delete(LauncherSettings::Favorites::GetContentUri(mInfo->mId, FALSE),
            NULL, NULL);
    // Lock on mBgLock *after* the db operation
    synchronized(mHost->sBgLock) {
        mHost->sBgItemsIdMap->Remove(mInfo->mId);
        mHost->sBgFolders->Remove(mInfo->mId);
        mHost->sBgDbIconCache->Remove(mInfo);
        mHost->sBgWorkspaceItems->Remove(mInfo);
    }

    mCr->Delete(LauncherSettings::Favorites::CONTENT_URI_NO_NOTIFICATION,
            LauncherSettings::Favorites::CONTAINER + "=" + mInfo->mId, NULL);
    // Lock on mBgLock *after* the db operation
    synchronized(mHost->sBgLock) {
        Int32 size;
        mInfo->mContents->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mInfo->mContents->Get(i, (IInterface**)&obj);
            AutoPtr<IItemInfo> childInfo = IItemInfo::Probe(obj);

            sBgItemsIdMap->Remove(childInfo.id);
            sBgDbIconCache->Remove(childInfo);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(LauncherModel::LauncherAppsCallback, Object,
        ILauncherAppsCallback);

LauncherModel::LauncherAppsCallback::LauncherAppsCallback(
    /* [in] */ LauncherModel* host)
    : mHost(host)
{
}

ECode LauncherModel::LauncherAppsCallback::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
            PackageUpdatedTask::OP_UPDATE, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
            PackageUpdatedTask::OP_REMOVE, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
            PackageUpdatedTask::OP_ADD, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packageName,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    if (!replacing) {
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                PackageUpdatedTask::OP_ADD, packageNames, user);
        mHost->EnqueuePackageUpdated(task);
        if (mAppsCanBeOnRemoveableStorage) {
            // Only rebind if we support removable storage. It catches the
            // case where apps on the external sd card need to be reloaded.
            StartLoaderFromBackground();
        }
    }
    else {
        // If we are replacing then just update the packages in the list
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                PackageUpdatedTask::OP_UPDATE, packageNames, user);
        mHost->EnqueuePackageUpdated(task);
    }
    return NOERROR;
}

ECode LauncherModel::LauncherAppsCallback::OnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packageName,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    if (!replacing) {
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                PackageUpdatedTask::OP_UNAVAILABLE, packageNames, user);
        return mHost->EnqueuePackageUpdated(task);
    }
    return NOERROR;
}

LauncherModel::MyRunnable8::MyRunnable8(
    /* [in] */ LoaderTask* host)
    : mHost(host)
{
}

ECode LauncherModel::MyRunnable8::Run()
{
    synchronized(mHost) {
        mHost->mLoadAndBindStepFinished = TRUE;
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "done with previous binding step");
        }
        mHost->Notify();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(LauncherModel::MyComparator, Object,
        IComparator);

ECode LauncherModel::MyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result);
{
    VALIDATE_NOT_NULL(result);

    ItemInfo* lhsinfo = (ItemInfo*)(IObject::Probe(lhs));
    ItemInfo* rhsinfo = (ItemInfo*)(IObject::Probe(rhs));
    *result = (Int32)(lhsinfo->mContainer - rhsinfo->mContainer);
    return NOERROR;
}

CAR_INTERFACE_IMPL(LauncherModel::MyComparator2, Object,
        IComparator);

ECode LauncherModel::MyComparator2::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result);
{
    VALIDATE_NOT_NULL(result);

    Int32 cellCountX;
    LauncherModel->GetCellCountX(&cellCountX);
    Int32 cellCountY;
    LauncherModel->GetCellCountY(&cellCountY);
    Int32 screenOffset = cellCountX * cellCountY;
    Int32 containerOffset = screenOffset * (ILauncher::SCREEN_COUNT + 1); // +1 hotseat
    ItemInfo* lhsinfo = (ItemInfo*)(IObject::Probe(lhs));
    ItemInfo* rhsinfo = (ItemInfo*)(IObject::Probe(rhs));
    Int64 lr = (lhs->mContainer * containerOffset + lhs->mScreen * screenOffset +
            lhs->mCellY * cellCountX + lhs->mCellX);
    Int64 rr = (rhs->mContainer * containerOffset + rhs->mScreen * screenOffset +
            rhs->mCellY * cellCountX + rhs->mCellX);
    *result = (Int32)(lr - rr);
    return NOERROR;
}

LauncherModel::MyRunnable9::MyRunnable9(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IArrayList* workspaceItems,
    /* [in] */ Int32 start,
    /* [in] */ Int32 chunkSize)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mWorkspaceItems(workspaceItems)
    , mStart(start)
    , mChunkSize(chunkSize)
{
}

ECode LauncherModel::MyRunnable9::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        return callbacks->BindItems(mWorkspaceItems, mStart, mStart + mChunkSize);
    }
    return NOERROR;
}

LauncherModel::MyRunnable10::MyRunnable10(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IHashMap* folders)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mFolders(folders);
{
}

ECode LauncherModel::MyRunnable10::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        return callbacks->BindFolders(mFolders);
    }
    return NOERROR;
}

LauncherModel::MyRunnable11::MyRunnable11(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ ILauncherAppWidgetInfo* widget)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mWidget(widget)
{
}

ECode LauncherModel::MyRunnable11::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        return callbacks->BindAppWidget(mWidget);
    }
    return NOERROR;
}

LauncherModel::MyRunnable12::MyRunnable12(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
{
}

ECode LauncherModel::MyRunnable12::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        return callbacks->StartBinding();
    }
    return NOERROR;
}

LauncherModel::MyRunnable13::MyRunnable13(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ Int32 currentScreen)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mCurrentScreen(currentScreen)
{
}

ECode LauncherModel::MyRunnable13::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != null) {
        callbacks->OnPageBoundSynchronously(mCurrentScreen);
    }
    return NOERROR;
}

LauncherModel::MyRunnable14::MyRunnable14(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
{
}

ECode LauncherModel::MyRunnable14::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        callbacks->FinishBindingItems();
    }

    // If we're profiling, ensure this is the last thing in the queue.
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "bound workspace in "
            + (SystemClock.uptimeMillis()-t) + "ms");
    }

    mHost->mIsLoadingAndBindingWorkspace = FALSE;
    return NOERROR;
}

LauncherModel::MyRunnable15::MyRunnable15(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IArrayList* list)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mList(list)
{
}

ECode LauncherModel::MyRunnable15::Run()
{
    Int64 t = SystemClock::UptimeMillis();
    AutoPtr<ILauncherModelCallbacks> callbacks = mHost->TryGetCallbacks(mOldCallbacks);
    if (callbacks != NULL) {
        callbacks->BindAllApplications(mList);
    }
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "bound all " + list.size() + " apps from cache in "
                + (SystemClock.uptimeMillis()-t) + "ms");
    }
    return NOERROR;
}

LauncherModel::MyRunnable16::MyRunnable16(
    /* [in] */ ILauncherModelCallbacks* _callbacks,
    /* [in] */ Boolean firstProfile,
    /* [in] */ IArrayList* added)
    : mCallbacks(_callbacks)
    , mFirstProfile(firstProfile)
    , mAdded(added)
{
}

ECode LauncherModel::MyRunnable16::Run()
{
    Int64 t = SystemClock::UptimeMillis();
    if (mCallbacks != NULL) {
        if (mFirstProfile) {
            mCallbacks->BindAllApplications(mAdded);
        } else {
            mCallbacks->BindAppsAdded(mAdded);
        }
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "bound " + mAdded.size() + " apps in "
                + (SystemClock.uptimeMillis() - t) + "ms");
        }
    }
    else {
        Slogger::I(TAG, "not binding apps: no Launcher activity");
    }
    return NOERROR;
}

LauncherModel::MyRunnable17::MyRunnable17(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks;
    /* [in] */ IArrayList* addedFinal)
    : mCallbacks(_callbacks)
    , mModelCallbacks(modelCallbacks)
    , mAddedFinal(addedFinal)
{
}

ECode LauncherModel::MyRunnable17::Run()
{
    AutoPtr<ILauncherModelCallbacks> cb;
    if (mCallbacks != NULL) {
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&cb);
    }
    else {
        cb = NULL;
    }
    if (TO_IINTERFACE(mModelCallbacks) == TO_IINTERFACE(cb) && cb != NULL) {
        mModelCallbacks->BindAppsAdded(mAddedFinal);
    }
    return NOERROR;
}

LauncherModel::MyRunnable18::MyRunnable18(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks;
    /* [in] */ IArrayList* modifiedFinal)
    : mCallbacks(_callbacks)
    , mModelCallbacks(modelCallbacks)
    , mModifiedFinal(modifiedFinal)
{
}

ECode LauncherModel::MyRunnable18::Run()
{
    AutoPtr<ILauncherModelCallbacks> cb;
    if (mCallbacks != NULL) {
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&cb);
    }
    else {
        cb = NULL;
    }
    if (TO_IINTERFACE(mModelCallbacks) == TO_IINTERFACE(cb) && cb != NULL) {
        mModelCallbacks->BindAppsUpdated(mModifiedFinal);
    }
    return NOERROR;
}

LauncherModel::MyRunnable19::MyRunnable19(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks;
    /* [in] */ IArrayList* removedPackageNames,
    /* [in] */ IArrayList* removedApps,
    /* [in] */ Boolean permanent,
    /* [in] */ IUserHandle* user)
    : mCallbacks(_callbacks)
    , mModelCallbacks(modelCallbacks)
    , mRemovedPackageNames(removedPackageNames)
    , mRemovedApps(removedApps)
    , mPermanent(permanent)
    , mUser(user)
{
}

ECode LauncherModel::MyRunnable19::Run()
{
    AutoPtr<ILauncherModelCallbacks> cb;
    if (mCallbacks != NULL) {
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&cb);
    }
    else {
        cb = NULL;
    }
    if (TO_IINTERFACE(mModelCallbacks) == TO_IINTERFACE(cb) && cb != NULL) {
        mModelCallbacks->BindComponentsRemoved(mRemovedPackageNames,
                mRemovedApps, mPermanent, mUser);
    }
}

LauncherModel::MyRunnable20::MyRunnable20(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks;
    /* [in] */ IArrayList* widgetsAndShortcuts)
    : mCallbacks(_callbacks)
    , mModelCallbacks(modelCallbacks)
    , mWidgetsAndShortcuts(widgetsAndShortcuts)
{
}

ECode LauncherModel::MyRunnable20::Run()
{
    AutoPtr<ILauncherModelCallbacks> cb;
    if (mCallbacks != NULL) {
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&cb);
    }
    else {
        cb = NULL;
    }
    if (TO_IINTERFACE(mModelCallbacks) == TO_IINTERFACE(cb) && cb != NULL) {
        mModelCallbacks->BindPackagesUpdated(mWidgetsAndShortcuts);
    }
    return NOERROR;
}


CAR_INTERFACE_IMPL(LauncherModel::LoaderTask, Object,
        IRunnable);

LauncherModel::LoaderTask::LoaderTask(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isLaunching);
    : mContext(context)
    , mIsLaunching(isLaunching)
    , mIsLoadingAndBindingWorkspace(FALSE)
    , mStopped(FALSE)
    , mLoadAndBindStepFinished(FALSE)
{
    CHashMap::New((IHashMap**)&mLabelCache);
}

ECode LauncherModel::LoaderTask::IsLaunching(
    /* [out] */ Boolean* result);
{
    VALIDATE_NOT_NULL(result);

    *result = mIsLaunching;
    return NOERROR;
}

ECode LauncherModel::LoaderTask::IsLoadingWorkspace(
    /* [out] */ Boolean* result);
{
    VALIDATE_NOT_NULL(result);

    *result = mIsLoadingAndBindingWorkspace;
    return NOERROR;
}

void LauncherModel::LoaderTask::LoadAndBindWorkspace()
{
    mIsLoadingAndBindingWorkspace = TRUE;

    // Load the workspace
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "loadAndBindWorkspace mWorkspaceLoaded=" + mWorkspaceLoaded);
    }

    if (!mWorkspaceLoaded) {
        LoadWorkspace();
        synchronized(this) {
            if (mStopped) {
                return;
            }
            mWorkspaceLoaded = TRUE;
        }
    }

    // Bind the workspace
    BindWorkspace(-1);
}

void LauncherModel::LoaderTask::WaitForIdle()
{
    // Wait until the either we're stopped or the other threads are done.
    // This way we don't start loading all apps until the workspace has settled
    // down.
    synchronized(this) {
        Int64 workspaceWaitTime = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;

        AutoPtr<IRunnable> run = new MyRunnable8(this);
        mHandler->PostIdle(run);

        while (!mStopped && !mLoadAndBindStepFinished && !mFlushingWorkerThread) {
            //try {
            // Just in case mFlushingWorkerThread changes but we aren't woken up,
            // wait no longer than 1sec at a time
            Wait(1000);
            //} catch (InterruptedException ex) {
                // Ignore
            //}
        }
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "waited "
                    + (SystemClock.uptimeMillis()-workspaceWaitTime)
                    + "ms for previous step to finish binding");
        }
    }
}

ECode LauncherModel::LoaderTask::RunBindSynchronousPage(
    /* [in] */ Int32 synchronousBindPage)
{
    if (synchronousBindPage < 0) {
        // Ensure that we have a valid page index to load synchronously
        // throw new RuntimeException("Should not call runBindSynchronousPage() without " +
        //         "valid page index");
        Slogger::E("LauncherModel::LoaderTask", "Should not call runBindSynchronousPage() without " +
                "valid page index");
        return E_RUNTIME_EXCEPTION;
    }
    if (!mAllAppsLoaded || !mWorkspaceLoaded) {
        // Ensure that we don't try and bind a specified page when the pages have not been
        // loaded already (we should load everything asynchronously in that case)
        //throw new RuntimeException("Expecting AllApps and Workspace to be loaded");
        Slogger::E("LauncherModel::LoaderTask", "Expecting AllApps and Workspace to be loaded");
        return E_RUNTIME_EXCEPTION;
    }
    synchronized(mLock) {
        if (mIsLoaderTaskRunning) {
            // Ensure that we are never running the background loading at this point since
            // we also touch the background collections
            //throw new RuntimeException("Error! Background loading is already running");
            Slogger::E("LauncherModel::LoaderTask", "Error! Background loading is already running");
            return E_RUNTIME_EXCEPTION;
        }
    }

    // XXX: Throw an exception if we are already loading (since we touch the worker thread
    //      data structures, we can't allow any other thread to touch that data, but because
    //      this call is synchronous, we can get away with not locking).

    // The LauncherModel is static in the LauncherApplication and mHandler may have queued
    // operations from the previous activity.  We need to ensure that all queued operations
    // are executed before any synchronous binding work is done.
    mHandler->Flush();

    // Divide the set of loaded items into those that we are binding synchronously, and
    // everything else that is to be bound normally (asynchronously).
    BindWorkspace(synchronousBindPage);
    // XXX: For now, continue posting the binding of AllApps as there are other issues that
    //      arise from that.
    return OnlyBindAllApps();
}

ECode LauncherModel::LoaderTask::Run()
{
    synchronized(mLock) {
        mIsLoaderTaskRunning = TRUE;
    }

    keep_running: {
        // Elevate priority when Home launches for the first time to avoid
        // starving at boot time. Staring at a blank home is not cool.
        synchronized(mLock) {
            if (DEBUG_LOADERS) {
                Slogger::D(TAG, "Setting thread priority to " +
                    (mIsLaunching ? "DEFAULT" : "BACKGROUND"));
            }
            Process::SetThreadPriority(mIsLaunching
                    ? IProcess::THREAD_PRIORITY_DEFAULT
                    : IProcess::THREAD_PRIORITY_BACKGROUND);
        }

        // First step. Load workspace first, this is necessary since adding of apps from
        // managed profile in all apps is deferred until onResume. See http://b/17336902.
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "step 1: loading workspace");
        }
        LoadAndBindWorkspace();

        if (mStopped) {
            break keep_running;
        }

        // Whew! Hard work done.  Slow us down, and wait until the UI thread has
        // settled down.
        synchronized(mLock) {
            if (mIsLaunching) {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "Setting thread priority to BACKGROUND");
                }
                Process::SetThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
            }
        }
        WaitForIdle();

        // Second step. Load all apps.
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "step 2: loading all apps");
        }
        LoadAndBindAllApps();

        // Restore the default thread priority after we are done loading items
        synchronized(mLock) {
            Process::SetThreadPriority(IProcess::THREAD_PRIORITY_DEFAULT);
        }
    }


    // Update the saved icons if necessary
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "Comparing loaded icons to database icons");
    }
    synchronized(sBgLock) {
        AutoPtr<ISet> fkeyset;
        sBgDbIconCache->GetKeySet((ISet**)&fkeyset);
        AutoPtr<IIterator> fit;
        fkeyset->GetIterator((IIterator**)&fit);
        Boolean hasNext;
        while (fit->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> key;
            fit->GetNext((IInterface**)&ko);
            AutoPtr<IInterface> obj;
            sBgDbIconCache->Get(key, (IInterface**)&obj);
            UpdateSavedIcon(mContext, IShortcutInfo::Probe(key),
                    IArrayOf::Probe(obj));
        }
        sBgDbIconCache->Clear();
    }

    // Clear out this reference, otherwise we end up holding it until all of the
    // callback runnables are done.
    mContext = NULL;

    synchronized (mLock) {
        // If we are still the last one to be scheduled, remove ourselves.
        if (mLoaderTask == this) {
            mLoaderTask = NULL;
        }
        mIsLoaderTaskRunning = FALSE;
    }
    return NOERROR;
}

ECode LauncherModel::LoaderTask::StopLocked()
{
    synchronized(this) {
        mStopped = TRUE;
        Notify();
    }
    return NOERROR;
}

ECode LauncherModel::LoaderTask::TryGetCallbacks(
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [out] */ ILauncherModelCallbacks** newCallbacks)
{
    VALIDATE_NOT_NULL(newCallbacks);

    synchronized(mLock) {
        if (mStopped) {
            *newCallbacks = NULL;
            return NOERROR;
        }

        if (mCallbacks == NULL) {
            *newCallbacks = NULL;
            return NOERROR;
        }

        AutoPtr<ILauncherModelCallbacks> _callbacks;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&_callbacks);
        if (TO_IINTERFACE(_callbacks) != TO_IINTERFACE(oldCallbacks)) {
            *newCallbacks = NULL;
            return NOERROR;
        }
        if (_callbacks == NULL) {
            Slogger::W(TAG, "no mCallbacks");
            *newCallbacks = NULL;
            return NOERROR;
        }

        *newCallbacks = _callbacks;
        REFCOUNT_ADD(*newCallbacks);
        return NOERROR;
    }
}

Boolean LauncherModel::LoaderTask::CheckItemPlacement(
    /* [in] */ ArrayOf<ArrayOf<ArrayOf<ItemInfo*> > >* occupied,
    /* [in] */ ItemInfo* item)
{
    Int32 containerIndex = item->mScreen;
    if (item->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        // Return early if we detect that an item is under the hotseat button
        if (mCallbacks == NULL) {
            return FALSE;
        }
        AutoPtr<ILauncherModelCallbacks> _callbacks;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&_callbacks);
        Boolean res;
        _callbacks->IsAllAppsButtonRank(item->mScreen, res);
        if (res) {
            return FALSE;
        }

        // We use the last index to refer to the hotseat and the screen as the rank, so
        // test and update the occupied state accordingly
        if ((*occupied)[ILauncher::SCREEN_COUNT][item->mScreen][0] != NULL) {
            Slogger::E(TAG, "Error loading shortcut into hotseat " + item
                + " into position (" + item.screen + ":" + item.cellX + "," + item.cellY
                + ") occupied by " + occupied[Launcher.SCREEN_COUNT][item.screen][0]);
            return FALSE;
        }
        else {
            occupied->Set(ILauncher::SCREEN_COUNT, item->mScreen, 0, item)
            return TRUE;
        }
    }
    else if (item->mContainer != LauncherSettings::Favorites::CONTAINER_DESKTOP) {
        // Skip further checking if it is not the hotseat or workspace container
        return TRUE;
    }

    // Check if any workspace icons overlap with each other
    for (Int32 x = item->mCellX; x < (item->mCellX+item->mSpanX); x++) {
        for (Int32 y = item->mCellY; y < (item->mCellY+item->mSpanY); y++) {
            if ((*occupied)[containerIndex][x][y] != NULL) {
                Slogger::E(TAG, "Error loading shortcut " + item
                    + " into cell (" + containerIndex + "-" + item.screen + ":"
                    + x + "," + y
                    + ") occupied by "
                    + occupied[containerIndex][x][y]);
                return FALSE;
            }
        }
    }
    for (Int32 x = item->mCellX; x < (item->mCellX+item->mSpanX); x++) {
        for (Int32 y = item->mCellY; y < (item->mCellY+item->mSpanY); y++) {
            occupied->Set(containerIndex, x, y) = item;
        }
    }

    return TRUE;
}

void LauncherModel::LoaderTask::LoadWorkspace()
{
    Int64 t = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;

    AutoPtr<IContext> context = mContext;
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<IPackageManager> manager;
    context->GetPackageManager((IPackageManager**)&manager);
    AutoPtr<IAppWidgetManager> widgets;
    AppWidgetManager->GetInstance(context, (IAppWidgetManager**)&widgets);
    Boolean isSafeMode;
    manager->IsSafeMode(&isSafeMode);

    // Make sure the default workspace is loaded, if needed
    AutoPtr<LauncherProvider> provider;
    mApp->GetLauncherProvider((LauncherProvider**)&provider);
    provider->LoadDefaultFavoritesIfNecessary(0, FALSE);

    synchronized(sBgLock) {
        sBgWorkspaceItems->Clear();
        sBgAppWidgets->Clear();
        sBgFolders->Clear();
        sBgItemsIdMap->Clear();
        sBgDbIconCache->Clear();

        AutoPtr<IArrayList> itemsToRemove;
        CArrayList::New((IArrayList**)&itemsToRemove);

        AutoPtr<ICursor> c;
        contentResolver->Query(
                LauncherSettings::Favorites::CONTENT_URI, NULL, NULL, NULL, NULL);

        // +1 for the hotseat (it can be larger than the workspace)
        // Load workspace in reverse order to ensure that latest items are loaded first (and
        // before any earlier duplicates)
        AutoPtr<ArrayOf<AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > > > occupied =
                ArrayOf<AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > >::Alloc(
                ILauncher::SCREEN_COUNT + 1);

        for (Int32 i = 0; i < ILauncher::SCREEN_COUNT + 1; i++) {
            AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > array2 =
                ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > >::Alloc(mCellCountX + 1);
            (*occupied)[i] = array2;

            for (Int32 j = 0; i < mCellCountX + 1; i++) {

                AutoPtr<ArrayOf<ItemInfo*> > array3 =
                        ArrayOf<ItemInfo*>::Alloc(mCellCountY + 1);
                (*array2)[j] = array3;
            }

        }

        //try {
        Int32 idIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::_ID, &idIndex), FINALLY)
        Int32 intentIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::INTENT, &intentIndex), FINALLY)
        Int32 titleIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::TITLE, &titleIndex), FINALLY)
        Int32 iconTypeIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_TYPE, &iconTypeIndex), FINALLY)
        Int32 iconIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON, &iconIndex), FINALLY)
        Int32 iconPackageIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_PACKAGE, &iconPackageIndex), FINALLY)
        Int32 iconResourceIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_RESOURCE, &iconResourceIndex), FINALLY)
        Int32 containerIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER, &containerIndex), FINALLY)
        Int32 itemTypeIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ITEM_TYPE, &itemTypeIndex), FINALLY)
        Int32 appWidgetIdIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::APPWIDGET_ID, &appWidgetIdIndex), FINALLY)
        Int32 screenIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN, &screenIndex), FINALLY)
        Int32 cellXIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX, &cellXIndex), FINALLY)
        Int32 cellYIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY, &cellYIndex), FINALLY)
        Int32 spanXIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANX, &spanXIndex), FINALLY)
        Int32 spanYIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANY, &spanYIndex), FINALLY)
        Int32 profileIdIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::PROFILE_ID, &profileIdIndex), FINALLY)
        //final int uriIndex = c.getColumnIndexOrThrow(LauncherSettings.Favorites.URI);
        //final int displayModeIndex = c.getColumnIndexOrThrow(
        //        LauncherSettings.Favorites.DISPLAY_MODE);

        AutoPtr<ShortcutInfo> info;
        String intentDescription;
        AutoPtr<LauncherAppWidgetInfo> appWidgetInfo;
        Int32 container;
        Int64 id;
        AutoPtr<IIntent> intent;
        AutoPtr<IUserHandle> user;

        Boolean res;
        while (!mStopped && (c->MoveToNext(&res), res)) {
            //try {
            Int32 itemType;
            FAIL_GOTO(c->GetInt32(itemTypeIndex, &itemType), FAILED)

            switch (itemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
            {
                FAIL_GOTO(c->GetString(intentIndex, &intentDescription), FAILED)
                Int32 serialNumber;
                FAIL_GOTO(c->GetInt32(profileIdIndex, &serialNumber), FAILED)
                FAIL_GOTO(mUserManager->GetUserForSerialNumber(serialNumber, (IUserHandle**)&user), FAILED)
                // If the user doesn't exist anymore, skip.
                if (user == NULL) {
                    Int64 index;
                    FAIL_GOTO(c->GetInt64(idIndex, &index), FAILED)
                    AutoPtr<IInteger64> obj = CoreUtils::Convert(index);
                    FAIL_GOTO(itemsToRemove->Add(TO_IINTERFACE(obj)), FAILED)
                    continue;
                }
                //try {
                if ((ECode)E_URI_SYNTAX_EXCEPTION == Intent::ParseUri(intentDescription, 0,
                        (IIntent**)&intent)) {
                    continue;
                }
                //} catch (URISyntaxException e) {
                    //continue;
                //}

                if (itemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION) {
                        FAIL_GOTO(GetShortcutInfo(manager, intent, user, context, c, iconIndex,
                            titleIndex, mLabelCache, (ShortcutInfo**)&info), FAILED)
                }
                else {
                    FAIL_GOTO(GetShortcutInfo(c, context, iconTypeIndex,
                            iconPackageIndex, iconResourceIndex, iconIndex,
                            titleIndex, (ShortcutInfo**)&info), FAILED)

                    // App shortcuts that used to be automatically added to Launcher
                    // didn't always have the correct intent flags set, so do that
                    // here
                    String action;
                    FAIL_GOTO(intent->GetAction(&action), FAILED)
                    if (!action.IsNull()) {
                        AutoPtr<ArrayOf<String> > categories;
                        FAIL_GOTO(intent->GetCategories((ArrayOf<String>**)&categories), FAILED)
                        Boolean res;
                        if (categories != NULL && (action.Equals(IIntent::ACTION_MAIN, &res), res)) {
                            if (categories->Contains(IIntent::CATEGORY_LAUNCHER, &res), res) {
                                FAIL_GOTO(intent->AddFlags(
                                    Intent::FLAG_ACTIVITY_NEW_TASK |
                                    Intent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED), FAILED)
                            }
                        }
                    }
                }

                if (info != NULL) {
                    info->mIntent = intent;
                    FAIL_GOTO(c->GetInt64(idIndex, &(info->mId)), FAILED)
                    FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
                    info->mContainer = container;
                    FAIL_GOTO(c->GetInt32(screenIndex, &(info->mScreen)), FAILED)
                    FAIL_GOTO(c->GetInt32(cellXIndex, &(info->mCellX)), FAILED)
                    FAIL_GOTO(c->GetInt32(cellYIndex, &(info->mCellY)), FAILED)
                    FAIL_GOTO(info->mIntent->PutExtra(IItemInfo::EXTRA_PROFILE, info->mUser), FAILED)

                    // check & update map of what's occupied
                    Boolean res;
                    FAIL_GOTO(CheckItemPlacement(occupied, info, &res), FAILED)
                    if (!res) {
                        break;
                    }

                    switch (container) {
                    case LauncherSettings::Favorites::CONTAINER_DESKTOP:
                    case LauncherSettings::Favorites::CONTAINER_HOTSEAT:
                        FAIL_GOTO(sBgWorkspaceItems->Add(info), FAILED)
                        break;
                    default:
                        // Item is in a user folder
                        AutoPtr<FolderInfo> folderInfo =
                                FindOrMakeFolder(sBgFolders, container);
                        FAIL_GOTO(folderInfo->Add(info), FAILED)
                        break;
                    }
                    FAIL_GOTO(sBgItemsIdMap->Put(info->mId, info), FAILED)

                    // now that we've loaded everthing re-save it with the
                    // icon in case it disappears somehow.
                    FAIL_GOTO(QueueIconToBeChecked(sBgDbIconCache, info, c, iconIndex), FAILED)
                }
                else {
                    // Failed to load the shortcut, probably because the
                    // activity manager couldn't resolve it (maybe the app
                    // was uninstalled), or the db row was somehow screwed up.
                    // Delete it.
                    FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)
                    Slogger:E(TAG, "Error loading shortcut " + id + ", removing it");
                    FAIL_GOTO(contentResolver->Delete(LauncherSettings::Favorites::getContentUri(
                                id, FALSE), NULL, NULL), FAILED)
                }
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
            {
                FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)
                AutoPtr<FolderInfo> folderInfo = FindOrMakeFolder(sBgFolders, id);

                FAIL_GOTO(c->GetString(titleIndex, &(folderInfo->mTitle)), FAILED)
                folderInfo->mId = id;
                FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
                folderInfo->mContainer = container;
                FAIL_GOTO(c->GetInt32(screenIndex, &(folderInfo->mScreen)), FAILED)
                FAIL_GOTO(c->GetInt32(cellXIndex, &(folderInfo->mCellX)), FAILED)
                FAIL_GOTO(c->GetInt32(cellYIndex, &(folderInfo->mCellY)), FAILED)

                // check & update map of what's occupied
                if (!CheckItemPlacement(occupied, folderInfo)) {
                    break;
                }
                switch (container) {
                    case LauncherSettings::Favorites::CONTAINER_DESKTOP:
                    case LauncherSettings::Favorites::CONTAINER_HOTSEAT:
                        sBgWorkspaceItems->Add(folderInfo);
                        break;
                }

                FAIL_GOTO(sBgItemsIdMap->Put(folderInfo->mId, folderInfo), FAILED)
                FAIL_GOTO(sBgFolders->Put(folderInfo->mId, folderInfo), FAILED)
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
            {
                // Read all Launcher-specific widget details
                Int32 appWidgetId;
                FAIL_GOTO(c->GetInt32(appWidgetIdIndex, &appWidgetId), FAILED)
                FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)

                AutoPtr<IAppWidgetProviderInfo> provider;
                FAIL_GOTO(widgets->GetAppWidgetInfo(appWidgetId,
                        (IAppWidgetProviderInfo**)&provider), FAILED)

                if (!isSafeMode) {
                    if (provider == NULL) {
                        String log = "Deleting widget that isn't installed anymore: id="
                            + id + " appWidgetId=" + appWidgetId;
                        Log.e(TAG, log);
                        Launcher.sDumpLogs.add(log);
                        AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
                        itemsToRemove->Add(TO_IINTERFACE(obj));
                    }
                    else {
                        AutoPtr<IComponentName> name;
                        provider->GetProvider((IComponentName**)&name);
                        if (name == NULL) {
                            String log = "Deleting widget that isn't installed anymore: id="
                                + id + " appWidgetId=" + appWidgetId;
                            Log.e(TAG, log);
                            Launcher.sDumpLogs.add(log);
                            AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
                            itemsToRemove->Add(TO_IINTERFACE(obj));
                        }
                        else {
                            String packageName;
                            name->GetPackageName(&packageName);
                            if (packageName.IsNull()) {
                                String log = "Deleting widget that isn't installed anymore: id="
                                    + id + " appWidgetId=" + appWidgetId;
                                Log.e(TAG, log);
                                Launcher.sDumpLogs.add(log);
                                AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
                                itemsToRemove->Add(TO_IINTERFACE(obj));
                            }
                        }
                    }
                }
                else {
                    AutoPtr<IComponentName> name;
                    FAIL_GOTO(provider->GetProvider((IComponentName**)&name), FAILED)
                    appWidgetInfo = new LauncherAppWidgetInfo(appWidgetId, name);
                    appWidgetInfo->mId = id;
                    FAIL_GOTO(c->GetInt32(screenIndex, &(appWidgetInfo->mScreen)), FAILED)
                    FAIL_GOTO(c->GetInt32(cellXIndex, &(appWidgetInfo->mCellX)), FAILED)
                    FAIL_GOTO(c->GetInt32(cellYIndex, &(appWidgetInfo->mCellY)), FAILED)
                    FAIL_GOTO(c->GetInt32(spanXIndex, &(appWidgetInfo->mSpanX)), FAILED)
                    FAIL_GOTO(c->GetInt32(spanYIndex, &(appWidgetInfo->mSpanY)), FAILED)
                    AutoPtr<ArrayOf<Int32> > minSpan;
                    Launcher::GetMinSpanForWidget(context, provider, (ArrayOf<Int32>**)&minSpan);
                    appWidgetInfo->mMinSpanX = (*minSpan)[0];
                    appWidgetInfo->mMinSpanY = (*minSpan)[1];

                    FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
                    if (container != LauncherSettings::Favorites::CONTAINER_DESKTOP &&
                        container != LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
                        Slogger::E(TAG, "Widget found where container != " +
                            "CONTAINER_DESKTOP nor CONTAINER_HOTSEAT - ignoring!");
                        continue;
                    }
                    FAIL_GOTO(c->GetInt32(containerIndex, &(appWidgetInfo->mContainer)), FAILED)

                    // check & update map of what's occupied
                    if (!CheckItemPlacement(occupied, appWidgetInfo)) {
                        break;
                    }
                    FAIL_GOTO(sBgItemsIdMap->Put(appWidgetInfo->mId, appWidgetInfo), FAILED)
                    FAIL_GOTO(sBgAppWidgets->Add(appWidgetInfo), FAILED)
                }
                break;
            }
            }
            //catch (Exception e) {
FAILED:
            Slogger::W(TAG, "Desktop items loading interrupted:", e);
            //}
        }
        //} finally {
FINALLY:
        c->Close();
        //}
        Int32 size;
        itemsToRemove->GetSize(&size);
        if (size > 0) {
            AutoPtr<IContentProviderClient> client;
            contentResolver->AcquireContentProviderClient(
                    LauncherSettings::Favorites::CONTENT_URI, (IContentProviderClient**)&client);
            // Remove dead items
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                itemsToRemove->Get(i, (IInterface)&obj);
                AutoPtr<IInteger64> value = IInteger64::Probe(obj);
                Int64 id;
                value->GetValue(&id);

                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "Removed id = " + id);
                }
                // Don't notify content observers
                //try {
                if ((ECode)E_REMOTE_EXCEPTION == client->Delete(LauncherSettings::Favorites::
                        GetContentUri(id, FALSE), NULL, NULL)) {
                    Slogger::W(TAG, "Could not remove id = " + id);
                }
                //} catch (RemoteException e) {
                    //Log.w(TAG, "Could not remove id = " + id);
                //}
            }
        }

        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "loaded workspace in " + (SystemClock.uptimeMillis()-t) + "ms");
            Slogger::D(TAG, "workspace layout: ");
            for (Int32 y = 0; y < mCellCountY; y++) {
                StringBuilder line;
                for (Int32 s = 0; s < ILauncher::SCREEN_COUNT; s++) {
                    if (s > 0) {
                        line += " | ";
                    }
                    for (Int32 x = 0; x < mCellCountX; x++) {
                        line += (((*occupied)[s][x][y] != NULL) ? "#" : ".");
                    }
                }
                Slogger::D(TAG, "[ " + line + " ]");
            }
        }
    }
}

void LauncherModel::LoaderTask::FilterCurrentWorkspaceItems(
    /* [in] */ Int32 currentScreen,
    /* [in] */ IArrayList* allWorkspaceItems,
    /* [in] */ IArrayList* currentScreenItems,
    /* [in] */ IArrayList* otherScreenItems)
{
    // Purge any null ItemInfos
    AutoPtr<IIterator> iter;
    allWorkspaceItems->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), &res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IItemInfo> i = IItemInfo::Probe(obj);
        if (i == NULL) {
            iter->Remove();
        }
    }

    // If we aren't filtering on a screen, then the set of items to load is the full set of
    // items given.
    if (currentScreen < 0) {
        currentScreenItems->AddAll(allWorkspaceItems);
    }

    // Order the set of items by their containers first, this allows use to walk through the
    // list sequentially, build up a list of containers that are in the specified screen,
    // as well as all items in those containers.
    AutoPtr<ISet> itemsOnScreen;
    CHashSet::New((ISet**)&itemsOnScreen);
    AutoPtr<IComparator> c = new MyComparator();
    Collections::Sort(allWorkspaceItems, c);

    Int32 size;
    allWorkspaceItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        allWorkspaceItems->Get(i, (IInterface**)&obj);
        ItemInfo* info = (ItemInfo*)(IObject::Probe(obj));

        if (info->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP) {
            if (info->mScreen == currentScreen) {
                currentScreenItems->Add(TO_IINTERFACE(info));
                itemsOnScreen->Add(info->mId);
            }
            else {
                otherScreenItems->Add(TO_IINTERFACE(info));
            }
        }
        else if (info->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            currentScreenItems->Add(TO_IINTERFACE(info));
            itemsOnScreen->Add(info->mId);
        }
        else {
            Boolean res;
            itemsOnScreen->Contains(info->mContainer, &res);
            if (res) {
                currentScreenItems->Add(TO_IINTERFACE(info));
                itemsOnScreen->Add(info->mId);
            }
            else {
                otherScreenItems->Add(TO_IINTERFACE(info));
            }
        }
    }
}

void LauncherModel::LoaderTask::FilterCurrentAppWidgets(
    /* [in] */ Int32 currentScreen,
    /* [in] */ IArrayList* appWidgets,
    /* [in] */ IArrayList* currentScreenWidgets,
    /* [in] */ IArrayList* otherScreenWidgets)
{
    // If we aren't filtering on a screen, then the set of items to load is the full set of
    // widgets given.
    if (currentScreen < 0) {
        currentScreenWidgets->AddAll(appWidgets);
    }

    Int32 size;
    appWidgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        appWidgets->Get(i, (IInterface**)&obj);
        LauncherAppWidgetInfo* widget = (LauncherAppWidgetInfo*)(IObject::Probe(obj));

        if (widget == NULL) continue;
        if (widget->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP &&
                widget->mScreen == currentScreen) {
            currentScreenWidgets->Add(TO_IINTERFACE(widget));
        }
        else {
            otherScreenWidgets->Add(TO_IINTERFACE(widget));
        }
    }
}

void LauncherModel::LoaderTask::FilterCurrentFolders(
    /* [in] */ Int32 currentScreen,
    /* [in] */ IHashMap* itemsIdMap,
    /* [in] */ IHashMap* folders,
    /* [in] */ IHashMap* currentScreenFolders,
    /* [in] */ IHashMap* otherScreenFolders)
{
    // If we aren't filtering on a screen, then the set of items to load is the full set of
    // widgets given.
    if (currentScreen < 0) {
        currentScreenFolders->PutAll(folders);
    }

    AutoPtr<ISet> keySet;
    folders->GetKeySet((ISet**)keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 size;
    array->GetSize(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInteger64> obj = IInteger64::Probe((*array)[i]);
        Int64 id;
        obj->GetValue(&id);

        AutoPtr<IInterface> obj;
        itemsIdMap->Get((*array)[i], (IInterface**)&obj);
        AutoPtr<ItemInfo> info = (ItemInfo*)(IObject::Probe(obj));

        AutoPtr<IInterface> obj2;
        folders->Get((*array)[i], (IInterface**)&obj2);
        AutoPtr<FolderInfo> folder = (FolderInfo*)(IObject::Probe(obj2));

        if (info == NULL || folder == NULL) continue;
        if (info->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP &&
                info->mScreen == currentScreen) {
            currentScreenFolders->Put((*array)[i], TO_IINTERFACE(folder));
        }
        else {
            otherScreenFolders->Put((*array)[i], TO_IINTERFACE(folder));
        }
    }
}

void LauncherModel::LoaderTask::SortWorkspaceItemsSpatially(
    /* [in] */ IArrayList* workspaceItems)
{
    // XXX: review this
    AutoPtr<IComparator> c = new MyComparator2();
    Collections::Sort(workspaceItems, c);
}

void LauncherModel::LoaderTask::BindWorkspaceItems(
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IArrayList* workspaceItems,
    /* [in] */ IArrayList* appWidgets,
    /* [in] */ IHashMap* folders,
    /* [in] */ IArrayList* deferredBindRunnables)
{
    Boolean postOnMainThread = (deferredBindRunnables != NULL);

    // Bind the workspace items
    Int32 N;
    workspaceItems->GetSize(&N);
    for (Int32 i = 0; i < N; i += ITEMS_CHUNK) {
        Int32 start = i;
        Int32 chunkSize = (i+ITEMS_CHUNK <= N) ? ITEMS_CHUNK : (N-i);
        AutoPtr<IRunnable> r = new MyRunnable9(this, oldCallbacks, workspaceItems,
                start, chunkSize);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        }
        else {
            RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
    }

    // Bind the folders
    Boolean res;
    folders->IsEmpty(&res);
    if (!res) {
        AutoPtr<IRunnable> r = new MyRunnable10(this, oldCallbacks, folders);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        } else {
            RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
    }

    // Bind the widgets, one at a time
    appWidgets->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        appWidgets->Get(i, (IInterface**)&obj);
        AutoPtr<ILauncherAppWidgetInfo> widget = ILauncherAppWidgetInfo::Probe(obj);
        AutoPtr<IRunnable> = new MyRunnable11(this, oldCallbacks, widget);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        } else {
            RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
    }
}

void LauncherModel::LoaderTask::BindWorkspace(
    /* [in] */ Int32 synchronizeBindPage)
{
    Int64 t = SystemClock::GetUptimeMillis();
    AutoPtr<IRunnable> r;

    // Don't use these two variables in any of the callback runnables.
    // Otherwise we hold a reference to them.
    AutoPtr<IInterface> obj;
    mCallbacks->Get((IInterface**)&obj);
    AutoPtr<ILauncherModelCallbacks> oldCallbacks = ILauncherModelCallbacks::Probe(obj);
    if (oldCallbacks == NULL) {
        // This launcher has exited and nobody bothered to tell us.  Just bail.
        Slogger::W(TAG, "LoaderTask running with no launcher");
        return;
    }

    Boolean isLoadingSynchronously = (synchronizeBindPage > -1);
    Int32 currentScreen;
    if (isLoadingSynchronously) {
        currentScreen = synchronizeBindPage;
    }
    else {
        oldCallbacks->GetCurrentWorkspaceScreen(&currentScreen);
    }

    // Load all the items that are on the current page first (and in the process, unbind
    // all the existing workspace items before we call startBinding() below.
    UbindWorkspaceItemsOnMainThread();
    AutoPtr<IArrayList> workspaceItems;
    CArrayList::New((IArrayList**)&workspaceItems);
    AutoPtr<IArrayList> appWidgets;
    CArrayList::New((IArrayList**)&appWidgets);
    AutoPtr<IHashMap> folders;
    CHashMap::New((IHashMap**)&folders);
    AutoPtr<IHashMap> itemsIdMap;
    CHashMap::New((IHashMap**)&itemsIdMap);
    synchronized(sBgLock) {
        workspaceItems->AddAll(sBgWorkspaceItems);
        appWidgets->AddAll(sBgAppWidgets);
        folders->PutAll(sBgFolders);
        itemsIdMap->PutAll(sBgItemsIdMap);
    }

    AutoPtr<IArrayList> currentWorkspaceItems;
    CArrayList::New((IArrayList**)&currentWorkspaceItems);
    AutoPtr<IArrayList> otherWorkspaceItems;
    CArrayList::New((IArrayList**)&otherWorkspaceItems);
    AutoPtr<IArrayList> currentAppWidgets;
    CArrayList::New((IArrayList**)&currentAppWidgets);
    AutoPtr<IArrayList> otherAppWidgets;
    CArrayList::New((IArrayList**)&otherAppWidgets);
    AutoPtr<IHashMap> currentFolders;
    CHashMap::New((IHashMap**)&currentFolders);
    AutoPtr<IHashMap> otherFolders;
    CHashMap::New((IHashMap**)&otherFolders);

    // Separate the items that are on the current screen, and all the other remaining items
    FilterCurrentWorkspaceItems(currentScreen, workspaceItems, currentWorkspaceItems,
            otherWorkspaceItems);
    FilterCurrentAppWidgets(currentScreen, appWidgets, currentAppWidgets,
            otherAppWidgets);
    FilterCurrentFolders(currentScreen, itemsIdMap, folders, currentFolders,
            otherFolders);
    SortWorkspaceItemsSpatially(currentWorkspaceItems);
    SortWorkspaceItemsSpatially(otherWorkspaceItems);

    // Tell the workspace that we're about to start binding items
    r = new MyRunnable12(this, oldCallbacks);
    RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);

    // Load items on the current page
    BindWorkspaceItems(oldCallbacks, currentWorkspaceItems, currentAppWidgets,
            currentFolders, NULL);
    if (isLoadingSynchronously) {
        r = new MyRunnable13(this, oldCallbacks, currentScreen);
        RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
    }

    // Load all the remaining pages (if we are loading synchronously, we want to defer this
    // work until after the first render)
    mDeferredBindRunnables->Clear();
    BindWorkspaceItems(oldCallbacks, otherWorkspaceItems, otherAppWidgets, otherFolders,
            (isLoadingSynchronously ? mDeferredBindRunnables : NULL));

    // Tell the workspace that we're done binding items
    r = new MyRunnable14(this, oldCallbacks);
    if (isLoadingSynchronously) {
        mDeferredBindRunnables->Add(r);
    }
    else {
        RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
    }
}

void LauncherModel::LoaderTask::LoadAndBindAllApps()
{
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "loadAndBindAllApps mAllAppsLoaded=" + mAllAppsLoaded);
    }
    if (!mAllAppsLoaded) {
        LoadAllAppsByBatch();
        synchronized(this) {
            if (mStopped) {
                return;
            }
            mAllAppsLoaded = TRUE;
        }
    }
    else {
        OnlyBindAllApps();
    }
}

void LauncherModel::LoaderTask::OnlyBindAllApps()
{
    AutoPtr<IInterface> obj;
    mCallbacks->Get((IInterface**)&obj);
    AutoPtr<ILauncherModelCallbacks> oldCallbacks = ILauncherModelCallbacks::Probe(obj);
    if (oldCallbacks == NULL) {
        // This launcher has exited and nobody bothered to tell us.  Just bail.
        Slogger::W(TAG, "LoaderTask running with no launcher (onlyBindAllApps)");
        return;
    }

    // shallow copy
    //@SuppressWarnings("unchecked")
    AutoPtr<IInterface> obj;
    ICloneable::Probe(mBgAllAppsList->mDdata)->Clone((IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    AutoPtr<IRunnable> r = new MyRunnable15(this, oldCallbacks, list) ;
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    Int32 myId;
    Process::MyTid(&myId);
    Boolean isRunningOnMainThread = !(threadId == myId);
    Boolean res;
    oldCallbacks->IsAllAppsVisible(&res);
    if (res && isRunningOnMainThread) {
        r->Run();
    }
    else {
        mHandler->Post(r, &res);
    }
}

void LauncherModel::LoaderTask::LoadAllAppsByBatch()
{
    Int64 t = DEBUG_LOADERS ? SystemClock::uptimeMillis() : 0;

    // Don't use these two variables in any of the callback runnables.
    // Otherwise we hold a reference to them.
    AutoPtr<IInterface> obj;
    mCallbacks->Get((IInterface**)&obj);
    AutoPtr<ILauncherModelCallbacks> oldCallbacks = ILauncherModelCallbacks::Probe(obj);
    if (oldCallbacks == NULL) {
        // This launcher has exited and nobody bothered to tell us.  Just bail.
        Slogger::W(TAG, "LoaderTask running with no launcher (loadAllAppsByBatch)");
        return;
    }

    AutoPtr<IIntent> mainIntent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mainIntent);
    mainIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);

    AutoPtr<IList> profiles;
    mUserManager->GetUserProfiles((IList**)&profiles);

    mBgAllAppsList->Clear();
    Int32 profileCount;
    profiles->GetSize(&profileCount);
    for (Int32 p = 0; p < profileCount; p++) {
        AutoPtr<IInterface> obj;
        profiles->Get(p, (IInterface**)&obj);
        AutoPtr<IUserHandle> user = IUserHandle::Probe(obj);
        AutoPtr<IList> apps;
        Int32 N = Integer.MAX_VALUE;

        Int32 startIndex;
        Int32 i = 0;
        Int32 batchSize = -1;
        while (i < N && !mStopped) {
            if (i == 0) {
                Int64 qiaTime = DEBUG_LOADERS ? SystemClock::UptimeMillis() : 0;
                mLauncherApps->GetActivityList(NULL, user, (IList**)&apps);
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "queryIntentActivities took "
                            + (SystemClock.uptimeMillis()-qiaTime) + "ms");
                }
                if (apps == NULL) {
                    return;
                }
                apps->GetSize(&N);
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "queryIntentActivities got " + N + " apps");
                }
                if (N == 0) {
                    // There are no apps?!?
                    return;
                }
                if (mBatchSize == 0) {
                    batchSize = N;
                }
                else {
                    batchSize = mBatchSize;
                }

                Int64 sortTime = DEBUG_LOADERS ? SystemClock::UptimeMillis() : 0;
                AutoPtr<IComparator> c = new ShortcutNameComparator(mLabelCache);
                Collections::Sort(apps, c);
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "sort took "
                            + (SystemClock.uptimeMillis()-sortTime) + "ms");
                }
            }

            Int64 t2 = DEBUG_LOADERS ? SystemClock::UptimeMillis() : 0;

            startIndex = i;
            for (Int32 j=0; i<N && j<batchSize; j++) {
                // This builds the icon bitmaps.
                AutoPtr<IInterface> obj;
                apps->Get(i, (IInterface**)&obj);
                AutoPtr<LauncherActivityInfo> _info = LauncherActivityInfo::Probe(obj);
                AutoPtr<ApplicationInfo> info = new ApplicationInfo(_info, user,
                        mIconCache, mLabelCache);
                mBgAllAppsList->Add(info);
                i++;
            }

            AutoPtr<ILauncherModelCallbacks> callbacks = TryGetCallbacks(oldCallbacks);
            AutoPtr<IArrayList> added = mBgAllAppsList->mAdded;
            Boolean firstProfile = p == 0;
            CArrayList::New((ArrayList**)&(mBgAllAppsList->mAdded));
            AutoPtr<IRunnable> r = new MyRunnable16(callbacks, firstProfile, added);
            Boolean res;
            mHandler->Post(r, &res);

            if (DEBUG_LOADERS) {
                Slogger::D(TAG, "batch of " + (i-startIndex) + " icons processed in "
                        + (SystemClock.uptimeMillis()-t2) + "ms");
            }

            if (mAllAppsLoadDelay > 0 && i < N) {
                //try {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "sleeping for " + mAllAppsLoadDelay + "ms");
                }
                Thread::Sleep(mAllAppsLoadDelay);
                //} catch (InterruptedException exc) { }
            }
        }

        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "cached all " + N + " apps in "
                    + (SystemClock.uptimeMillis()-t) + "ms"
                    + (mAllAppsLoadDelay > 0 ? " (including delay)" : ""));
        }
    }
}

ECode LauncherModel::LoaderTask::DumpState()
{
    synchronized(sBgLock) {
        Slogger::D(TAG, "mLoaderTask.mContext=" + mContext);
        Slogger::D(TAG, "mLoaderTask.mIsLaunching=" + mIsLaunching);
        Slogger::D(TAG, "mLoaderTask.mStopped=" + mStopped);
        Slogger::D(TAG, "mLoaderTask.mLoadAndBindStepFinished=" + mLoadAndBindStepFinished);
        Slogger::D(TAG, "mItems size=" + sBgWorkspaceItems.size());
    }
    return NOERROR;
}

const Int32 LauncherModel::PackageUpdatedTask::OP_NONE = 0;
const Int32 LauncherModel::PackageUpdatedTask::OP_ADD = 1;
const Int32 LauncherModel::PackageUpdatedTask::OP_UPDATE = 2;
const Int32 LauncherModel::PackageUpdatedTask::OP_REMOVE = 3; // uninstlled
const Int32 LauncherModel::PackageUpdatedTask::OP_UNAVAILABLE = 4; // external media unmounted

CAR_INTERFACE_IMPL(LauncherModel::PackageUpdatedTask, Object, IRunnable);

LauncherModel::PackageUpdatedTask::PackageUpdatedTask(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ IUserHandle* user)
    : mOp(op)
    , mPackages(packages)
    , mUser(user)
{
}

ECode LauncherModel::PackageUpdatedTask::Run()
{
    AutoPtr<IContext> context = mApp;

    AutoPtr<ArrayOf<String> > packages = mPackages;
    Int32 N = packages->GetLength();
    switch (mOp) {
        case OP_ADD:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "mAllAppsList.addPackage " + packages[i]);
                }
                mBgAllAppsList->AddPackage(context, (*packages)[i], mUser);
            }
            break;
        }
        case OP_UPDATE:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "mAllAppsList.updatePackage " + packages[i]);
                }
                mBgAllAppsList->UpdatePackage(context, (*packages)[i], mUser);
                AutoPtr<IContext> ctx;
                context->GetApplicationContext((IContext**)&ctx);
                AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
                AutoPtr<WidgetPreviewLoader::CacheDb> db;
                app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db);
                WidgetPreviewLoader->RemoveFromDb(db, (*packages)[i]);
            }
            break;
        }
        case OP_REMOVE:
        case OP_UNAVAILABLE:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "mAllAppsList.removePackage " + packages[i]);
                }
                mBgAllAppsList->RemovePackage((*packages)[i], mUser);
                AutoPtr<IContext> ctx;
                context->GetApplicationContext((IContext**)&ctx);
                AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
                AutoPtr<WidgetPreviewLoader::CacheDb> db;
                app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db);
                WidgetPreviewLoader->RemoveFromDb(db, (*packages)[i]);
            }
            break;
        }
    }

    AutoPtr<IArrayList> added;
    AutoPtr<IArrayList> modified;
    AutoPtr<IArrayList> removedApps;
    CArrayList::New((IArrayList**)&removedApps);

    Boolean size;
    mBgAllAppsList->mAdded->GetSize(&size);
    if (size > 0) {
        CArrayList::New(mBgAllAppsList->mAdded, (IArrayList**)&added);
        mBgAllAppsList->mAdded->Clear();
    }
    mBgAllAppsList->mModified->GetSize(&size);
    if (size > 0) {
        CArrayList::New(mBgAllAppsList->mModified, (IArrayList**)&modified);
        mBgAllAppsList->mModified->Clear();
    }
    mBgAllAppsList->mRemoved->GetSize(&size);
    if (size > 0) {
        removedApps->AddAll(mBgAllAppsList->mRemoved);
        mBgAllAppsList->mRemoved->Clear();
    }

    AutoPtr<ILauncherModelCallbacks> callbacks;
    if (mCallbacks != NULL) {
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                    (ILauncherModelCallbacks**)&callbacks);
    }
    else {
        callbacks = NULL;
    }
    if (callbacks == NULL) {
        Slogger::W(TAG, "Nobody to tell about the new app.  Launcher is probably loading.");
        return;
    }

    if (added != NULL) {
        AutoPtr<IArrayList> addedFinal = added;
        AutoPtr<IRunnable> r = new MyRunnable17(mCallbacks, callbacks, addedFinal);
        Boolean res;
        mHandler->Post(r, &res);
    }
    if (modified != NULL) {
        AutoPtr<IArrayList> modifiedFinal = modified;
        AutoPtr<IRunnable> r = new MyRunnable18(mCallbacks, callbacks, modifiedFinal);
        Boolean res;
        mHandler->Post(r,&res);
    }
    // If a package has been removed, or an app has been removed as a result of
    // an update (for example), make the removed callback.
    Boolean res;
    removedApps->IsEmpty(&res);
    if (mOp == OP_REMOVE || !res) {
        Boolean permanent = (mOp == OP_REMOVE);
        AutoPtr<IArrayList> removedPackageNames;
        CArrayList::New(Arrays::AsList(packages), (IArrayList**)&removedPackageNames);

        AutoPtr<IRunnable> r = new MyRunnable19(mCallbacks, callbacks, removedPackageNames,
                removedApps, permanent, mUser);
        Boolean res;
        mHandler->Post(r, &res);
    }

    AutoPtr<IArrayList> widgetsAndShortcuts;
    GetSortedWidgetsAndShortcuts(context, (IArrayList**)&widgetsAndShortcuts);
    AutoPtr<IRunnable> r = new MyRunnable20(mCallbacks, callbacks, widgetsAndShortcuts);
    Boolean res;
    return mHandler->Post(r, &res);
}

CAR_INTERFACE_IMPL(LauncherModel::MyComparator3, Object, IComparator);

LauncherModel::MyComparator3::MyComparator3(
    /* [in] */ ICollator* collator)
    : mCollator(collator)
{
}

CARAPI LauncherModel::MyComparator3::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* outresult)
{
    VALIDATE_NOT_NULL(outresult);

    AutoPtr<ApplicationInfo> _a = (ApplicationInfo*)IObject::Probe(a);
    AutoPtr<ApplicationInfo> _b = (ApplicationInfo*)IObject::Probe(b);
    Boolean res;
    a->mUser.Equals(b->mUser, &res);
    if (res) {
        String astr;
        a->mTitle->ToString(&astr);
        String bstr;
        b->mTitle->ToString(&bstr);
        Int32 result;
        mCollator->Compare(astr, bstr, &result);
        if (result == 0) {
            a->mComponentName->CompareTo(b->mComponentName, &result);
        }
        *outresult = result;
        return NOERROR;
    }
    else {
        // TODO: Order this based on profile type rather than string compares.
        String astr;
        a->mUser->ToString(&astr);
        String bstr;
        b->mUser->ToString(&bstr);
        return astr.CompareTo(bstr, outresult);
    }
}

CAR_INTERFACE_IMPL(LauncherModel::MyComparator4, Object, IComparator);

LauncherModel::MyComparator4::MyComparator4(
    /* [in] */ ICollator* collator)
    : mCollator(collator)
{
}

CARAPI LauncherModel::MyComparator4::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String astr;
    a->mLabel->ToString(&astr);
    String bstr;
    b->mLabel->ToString(&bstr);
    return mCollator->Compare(astr, bstr, result);
}

CAR_INTERFACE_IMPL(LauncherModel::MyComparator5, Object, IComparator);

ECode LauncherModel::MyComparator5::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ApplicationInfo> _a = (ApplicationInfo*)IObject::Probe(a);
    AutoPtr<ApplicationInfo> _b = (ApplicationInfo*)IObject::Probe(b);
    if (_a->mFirstInstallTime < _b->mFirstInstallTime) {
        *result = 1;
        return NOERROR;
    }
    if (_a->mFirstInstallTime > _b->mFirstInstallTime) {
        *result = -1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

static AutoPtr<IComparator> initAPP_INSTALL_TIME_COMPARATOR()
{
    AutoPtr<IComparator> c = new MyComparator5();
    return c;
}

AutoPtr<IComparator> LauncherModel::APP_INSTALL_TIME_COMPARATOR
        = initAPP_INSTALL_TIME_COMPARATOR();

const Boolean LauncherModel::DEBUG_LOADERS = FALSE;
const String LauncherModel::TAG("Launcher.Model");

const Int32 LauncherModel::ITEMS_CHUNK = 6;

// Specific runnable types that are run on the main thread deferred handler, this allows us to
// clear all queued binding runnables when the Launcher activity is destroyed.
const Int32 LauncherModel::MAIN_THREAD_NORMAL_RUNNABLE = 0;
const Int32 LauncherModel::MAIN_THREAD_BINDING_RUNNABLE = 1;

Boolean LauncherModel::InitStaticBlock()
{
    AutoPtr<IHandlerThread> sWorkerThread;
    CHandlerThread::New(String("launcher-loader"), (IHandlerThread**)&sWorkerThread);
    sWorkerThread->Start();

    AutoPtr<ILooper> looper;
    sWorkerThread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> sWorker;
    CHandler::New(looper, (IHandler**)&sWorker);

    CArrayList::New((IArrayList**)&mDeferredBindRunnables);

    CHashMap::New((IHashMap**)&sBgItemsIdMap);
    CHashMap::New((IHashMap**)&sBgWorkspaceItems);
    CHashMap::New((IHashMap**)&sBgAppWidgets);
    CHashMap::New((IHashMap**)&sBgFolders);
    CHashMap::New((IHashMap**)&sBgDbIconCache);
    return TRUE;
}

Boolean LauncherModel::initStaticBlock = InitStaticBlock();

Object LauncherModel::sBgLock;

Int32 LauncherModel::mCellCountX;
Int32 LauncherModel::mCellCountY;

LauncherModel::LauncherModel()
    : mPreviousConfigMcc(0)
    , mBatchSize(0)
    , mAllAppsLoadDelay(0)
    , mAppsCanBeOnRemoveableStorage(FALSE)
    , mIsLoaderTaskRunning(FALSE)
    , mFlushingWorkerThread(FALSE)
    , mWorkspaceLoaded(FALSE)
    , mAllAppsLoaded(FALSE)
{
    CDeferredHandler::New((IDeferredHandler**)&mHandler);
}

ECode LauncherModel::constructor(
    /* [in] */ ILauncherApplication* app,
    /* [in] */ IconCache* iconCache)
{
    Environment::IsExternalStorageRemovable(&mAppsCanBeOnRemoveableStorage);
    mApp = app;
    mBgAllAppsList = new AllAppsList(iconCache);
    mIconCache = iconCache;


    Utilities::CreateIconBitmap(mIconCache->GetFullResDefaultActivityIcon(),
            app, (IBitmap**)&mDefaultIcon);
    AutoPtr<IResources> res;
    app->GetResources((IResources**)&res);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_allAppsBatchLoadDelay,
            &mAllAppsLoadDelay);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_allAppsBatchSize,
            &mBatchSize);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    config->GetMcc(&mPreviousConfigMcc);
    AutoPtr<IInterface> obj;
    app->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    mLauncherApps = ILauncherApps::Probe(obj);
    AutoPtr<IInterface> obj2;
    app->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj2);
    mUserManager = IUserManager::Probe(obj2);
    mLauncherAppsCallback = new LauncherModel::LauncherAppsCallback();
    return NOERROR;
}

void LauncherModel::RunOnMainThread(
    /* [in] */ IRunnable* r)
{
    RunOnMainThread(r, 0);
}

void LauncherModel::RunOnMainThread(
    /* [in] */ IRunnable* r,
    /* [in] */ Int32 type)
{
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    Int32 tid;
    Process::MyTid(&tid);
    if (threadId == tid) {
        // If we are on the worker thread, post onto the main handler
        Boolean res;
        mHandler->Post(r, &res);
    }
    else {
        r->Run();
    }
}

void LauncherModel::RunOnWorkerThread(
    /* [in] */ IRunnable* r)
{
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    Int32 tid;
    Process::MyTid(&tid);
    if (threadId == tid) {
        r->Run();
    }
    else {
        // If we are not on the worker thread, then post to the worker handler
        Boolean res;
        sWorker->Post(r, &res);
    }
}

ECode LauncherModel::GetFallbackIcon(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    return Bitmap::CreateBitmap(mDefaultIcon, bitmap);
}

ECode LauncherModel::UnbindItemInfosAndClearQueuedBindRunnables()
{
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    Int32 tid;
    Process::MyTid(&tid);
    if (threadId == tid) {
        // throw new RuntimeException("Expected unbindLauncherItemInfos() to be called from the " +
        //         "main thread");
        Slogger::E(TAG, "Expected unbindLauncherItemInfos() to be called from the " +
                "main thread");
        return E_RUNTIME_EXCEPTION;
    }

    // Clear any deferred bind runnables
    mDeferredBindRunnables->Clear();
    // Remove any queued bind runnables
    mHandler->CancelAllRunnablesOfType(MAIN_THREAD_BINDING_RUNNABLE);
    // Unbind all the workspace items
    return UnbindWorkspaceItemsOnMainThread();
}

ECode LauncherModel::UnbindWorkspaceItemsOnMainThread()
{
    // Ensure that we don't use the same workspace items data structure on the main thread
    // by making a copy of workspace items first.
    AutoPtr<IArrayList> tmpWorkspaceItems;
    CArrayList::New((IArrayList**)&tmpWorkspaceItems);
    AutoPtr<IArrayList> tmpAppWidgets;
    CArrayList::New((IArrayList**)&tmpAppWidgets);
    synchronized(sBgLock) {
        tmpWorkspaceItems->AddAll(sBgWorkspaceItems);
        tmpAppWidgets->AddAll(sBgAppWidgets);
    }
    AutoPtr<IRunnable> r = new MyRunnable(tmpWorkspaceItems, tmpAppWidgets);
    return RunOnMainThread(r);
}

ECode LauncherModel::AddOrMoveItemInDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    if (item.container == IItemInfo::NO_ID) {
        // From all apps
        return AddItemToDatabase(context, item, container, screen, cellX, cellY, FALSE);
    }
    else {
        // From somewhere else
        return MoveItemInDatabase(context, item, container, screen, cellX, cellY);
    }
    return NOERROR;
}

ECode LauncherModel::CheckItemInfoLocked(
    /* [in] */ Int64 itemId,
    /* [in] */ IItemInfo* item)
{
    AutoPtr<IInterface> obj;
    sBgItemsIdMap->Get(itemId, (IInterface**)&obj);
    AutoPtr<IItemInfo> modelItem = IItemInfo::Probe(obj);
    if (modelItem != NULL && TO_IINTERFACE(item) != TO_IINTERFACE(modelItem)) {
        // check all the data is consistent
        if (IShortcutInfo::Probe(modelItem) ! = NULL &&
                IShortcutInfo::Probe(item) ! = NULL) {
            AutoPtr<IShortcutInfo> modelShortcut = IShortcutInfo::Probe(modelItem);
            ShortcutInfo* _modelShortcut = (ShortcutInfo*)modelShortcut;
            AutoPtr<IShortcutInfo> shortcut = IShortcutInfo::Probe(item);
            ShortcutInfo* _shortcut = (ShortcutInfo*)shortcut;

            String str;
            _modelShortcut->mTitle->ToString(&str);
            String str2;
            _shortcut->mTitle->ToString(&str2);
            Boolean res;
            str.Equals(str2, &res);
            Boolean res2;
            _modelShortcut->mIntent->FilterEquals(_shortcut->mIntent, &res2);
            if (res && res2 &&
                    _modelShortcut->mId == _shortcut->mId &&
                    _modelShortcut->mItemType == _shortcut->mItemType &&
                    _modelShortcut->mContainer == _shortcut->mContainer &&
                    _modelShortcut->mScreen == _shortcut->mScreen &&
                    _modelShortcut->mCellX == _shortcut->mCellX &&
                    _modelShortcut->mCellY == _shortcut->mCellY &&
                    _modelShortcut->mSpanX == _shortcut->mSpanX &&
                    _modelShortcut->mSpanY == _shortcut->mSpanY &&
                    ((_modelShortcut->mDropPos == NULL && _shortcut->mDropPos == NULL) ||
                    (_modelShortcut->mDropPos != NULL &&
                            _shortcut->mDropPos != NULL &&
                            (*(_modelShortcut->mDropPos))[0] == (*(_shortcut->mDropPos))[0] &&
                    (*(_modelShortcut->mDropPos))[1] == (*(_shortcut->mDropPos))[1]))) {
                // For all intents and purposes, this is the same object
                return NOERROR;
            }
        }

        // the modelItem needs to match up perfectly with item if our model is
        // to be consistent with the database-- for now, just require
        // modelItem == item or the equality check above
        String msg = "item: " + ((item != NULL) ? item.toString() : "null") +
                "modelItem: " +
                ((modelItem != null) ? modelItem.toString() : "null") +
                "Error: ItemInfo passed to checkItemInfo doesn't match original";
        // RuntimeException e = new RuntimeException(msg);
        // if (stackTrace != null) {
        //     e.setStackTrace(stackTrace);
        // }
        // throw e;
        Slogger::E(TAG, msg);
        return E_RUNTIME_EXCEPTION;
    }
}

ECode LauncherModel::CheckItemInfo(
    /* [in] */ ItemInfo* item)
{
    Int64 itemId = item->mId;
    AutoPtr<IRunnable> r = new MyRunnable2(this, itemId, item);
    return RunOnWorkerThread(r);
}

ECode LauncherModel::UpdateItemInDatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values,
    /* [in] */ ItemInfo* item,
    /* [in] */ const String& callingFunction)
{
    Int64 itemId = item->mId;
    AutoPtr<IUri> uri;
    LauncherSettings::Favorites::GetContentUri(itemId, FALSE, (IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IRunnable> r = new MyRunnable3(this, cr, uri, values, itemId, item);
    return RunOnWorkerThread(r);
}

ECode LauncherModel::FlushWorkerThread()
{
    mFlushingWorkerThread = TRUE;
    AutoPtr<IRunnable> waiter = new MyRunnable4(this);

    synchronized(waiter) {
        RunOnWorkerThread(waiter);
        if (mLoaderTask != NULL) {
            synchronized(mLoaderTask) {
                mLoaderTask->Notify();
            }
        }
        Boolean success = FALSE;
        while (!success) {
            //try {
            ECode ec = waiter->Wait();
            if (SUCCEEDED(ec)) {
                success = TRUE;
            }
            //} catch (InterruptedException e) {
            //}
        }
    }
    return NOERROR;
}

ECode LauncherModel::MoveItemInDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    String transaction = "DbDebug    Modify item (" + item.title + ") in db, id: " + item.id +
            " (" + item.container + ", " + item.screen + ", " + item.cellX + ", " + item.cellY +
            ") --> " + "(" + container + ", " + screen + ", " + cellX + ", " + cellY + ")";
    Launcher::sDumpLogs->Add(transaction);
    Slogger::D(TAG, transaction);
    item->mContainer = container;
    item->mCellX = cellX;
    item->mCellY = cellY;

    // We store hotseat items in canonical form which is this orientation invariant position
    // in the hotseat
    if (ILauncher::Probe(context) != NULL && screen < 0 &&
            container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        AutoPtr<IHotseat> hotseat;
        ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
        hotseat->GetOrderInHotseat(cellX, cellY, &(item->mScreen));
    }
    else {
        item->mScreen = screen;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(LauncherSettings::Favorites::CONTAINER, item->mContainer);
    values->Put(LauncherSettings::Favorites::CELLX, item->mCellX);
    values->Put(LauncherSettings::Favorites::CELLY, item->mCellY);
    values->Put(LauncherSettings::Favorites::SCREEN, item->mScreen);

    return UpdateItemInDatabaseHelper(context, values, item, String("moveItemInDatabase"));
}

ECode LauncherModel::ModifyItemInDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
{
    String transaction = "DbDebug    Modify item (" + item.title + ") in db, id: " + item.id +
            " (" + item.container + ", " + item.screen + ", " + item.cellX + ", " + item.cellY +
            ") --> " + "(" + container + ", " + screen + ", " + cellX + ", " + cellY + ")";
    Launcher::sDumpLogs->Add(transaction);
    Slogger::D(TAG, transaction);
    item->mCellX = cellX;
    item->mCellY = cellY;
    item->mSpanX = spanX;
    item->mSpanY = spanY;

    // We store hotseat items in canonical form which is this orientation invariant position
    // in the hotseat
    if (ILauncher::Probe(context) != NULL && screen < 0 &&
            container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        AutoPtr<IHotseat> hotseat;
        ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
        hotseat->GetOrderInHotseat(cellX, cellY, &(item->mScreen));
    }
    else {
        item->mScreen = screen;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(LauncherSettings::Favorites::CONTAINER, item->mContainer);
    values->Put(LauncherSettings::Favorites::CELLX, item->mCellX);
    values->Put(LauncherSettings::Favorites::CELLY, item->mCellY);
    values->Put(LauncherSettings::Favorites::SPANX, item->mSpanX);
    values->Put(LauncherSettings::Favorites::SPANY, item->mSpanY);
    values->Put(LauncherSettings::Favorites::SCREEN, item->mScreen);

    return UpdateItemInDatabaseHelper(context, values, item, String("modifyItemInDatabase"));
}

ECode LauncherModel::UpdateItemInDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ItemInfo* item)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    item->OnAddToDatabase(context, values);
    item->UpdateValuesWithCoordinates(values, item->mCellX, item->mCellY);
    return UpdateItemInDatabaseHelper(context, values, item, String("updateItemInDatabase"));
}

ECode LauncherModel::ShortcutExists(
    /* [in] */ IContext* context,
    /* [in] */ const String& title,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > args1 = ArrayOf<String>::Alloc(2);
    (*args1[0]) = String("title");
    (*args1[1]) = String("intent");
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > args2 = ArrayOf<String>::Alloc(2);
    (*args2[0]) = title;
    String str;
    intent->ToUri(0, &str);
    (*args2[1]) = str;
    cr->Query(LauncherSettings::Favorites::CONTENT_URI,
        args1, String("title=? and intent=?"),
        args2, NULL);
    //try {
    c->MoveToFirst(result);
    //} finally {
    c->Close();
    //}
    return NOERROR;
}

ECode LauncherModel::GetItemsInLocalCoordinates(
    /* [in] */ IContext* context,
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(8);
    (*args)[0] = LauncherSettings::Favorites::ITEM_TYPE;
    (*args)[1] = LauncherSettings::Favorites::CONTAINER;
    (*args)[2] = LauncherSettings::Favorites::SCREEN;
    (*args)[3] = LauncherSettings::Favorites::CELLX;
    (*args)[4] = LauncherSettings::Favorites::CELLY;
    (*args)[5] = LauncherSettings::Favorites::SPANX;
    (*args)[6] = LauncherSettings::Favorites::SPANY;
    (*args)[7] = LauncherSettings::Favorites::PROFILE_ID;
    AutoPtr<ICursor> c;
    cr->Query(LauncherSettings::Favorites::CONTENT_URI, args,
            NULL, NULL, NULL, (ICursor**)&c);

    Int32 itemTypeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ITEM_TYPE, &itemTypeIndex);
    Int32 containerIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER, &containerIndex);
    Int32 screenIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN, &screenIndex);
    Int32 cellXIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX, &cellXIndex);
    Int32 cellYIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY, &cellYIndex);
    Int32 spanXIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANX, &spanXIndex);
    Int32 spanYIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANY, &spanYIndex);
    Int32 profileIdIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::PROFILE_ID, &profileIdIndex);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    //try {
    Boolean res;
    while (c->MoveToNext(&res), res) {
        AutoPtr<IItemInfo> item = new ItemInfo();
        ItemInfo* _item = (ItemInfo*)item;
        FAIL_GOTO(c->GetInt32(cellXIndex, &(_item->mCellX)), EXIT)
        FAIL_GOTO(c->GetInt32(cellYIndex, &(_item->mCellY)), EXIT)
        FAIL_GOTO(c->GetInt32(spanXIndex, &(_item->mSpanX)), EXIT)
        FAIL_GOTO(c->GetInt32(spanYIndex, &(_item->mSpanY)), EXIT)
        FAIL_GOTO(c->GetInt32(containerIndex, &(_item->mContainer)), EXIT)
        FAIL_GOTO(c->GetInt32(itemTypeIndex, &(_item->mItemType)), EXIT)
        FAIL_GOTO(c->GetInt32(screenIndex, &(_item->mScreen)), EXIT)
        FAIL_GOTO(c->GetInt32(profileIdIndex, &serialNumber), EXIT)
        FAIL_GOTO(um->GetUserForSerialNumber(serialNumber,
                (IUserHandle**)&(item->mUser)), EXIT)
        // If the user no longer exists, skip this item
        if (item->mUser != NULL) {
            FAIL_GOTO(items->Add(item), EXIT)
        }
    }
    //} catch (Exception e) {
EXIT:
    items->Clear();
    //} finally {
    c->Close();
    //}

    *list = items;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode LauncherModel::GetFolderById(
    /* [in] */ IContext* context,
    /* [in] */ IHashMap* folderList,
    /* [in] */ Int64 id,
    /* [out] */ IFolderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = String.valueOf(id);
    (*args)[1] = String.valueOf(LauncherSettings::Favorites::ITEM_TYPE_FOLDER);
    AutoPtr<ICursor> c;
    cr->Query(LauncherSettings::Favorites::CONTENT_URI, NULL,
            String("_id=? and (itemType=? or itemType=?)"),
            args, NULL, (ICursor**)&c);

    //try {
    Boolean res;
    if (c->MoveToFirst(&res), res) {
        Int32 itemTypeIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ITEM_TYPE,
                &itemTypeIndex), EXIT)
        Int32 titleIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::TITLE,
                &titleIndex), EXIT)
        Int32 containerIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER,
                &containerIndex), EXIT)
        Int32 screenIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN,
                &screenIndex), EXIT)
        Int32 cellXIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX,
                &cellXIndex), EXIT)
        Int32 cellYIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY,
                &cellYIndex), EXIT)

        AutoPtr<IFolderInfo> folderInfo;
        Int32 index;
        FAIL_GOTO(c->GetInt32(itemTypeIndex, &index), EXIT)
        switch (index) {
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
                FAIL_GOTO(FindOrMakeFolder(folderList, id, (IFolderInfo**)&folderInfo), EXIT)
                break;
        }

        FolderInfo* _folderInfo = (FolderInfo*)folderInfo;
        _folderInfo->mTitle;
        FAIL_GOTO(c->GetString(titleIndex, &(_folderInfo->mTitle)), EXIT)
        _folderInfo->mId = id;
        FAIL_GOTO(c->GetInt32(containerIndex, &(_folderInfo->mContainer)), EXIT)
        FAIL_GOTO(c->GetInt32(screenIndex, &(_folderInfo->mScreen)), EXIT)
        FAIL_GOTO(c->GetInt32(cellXIndex, &(_folderInfo->mCellX)), EXIT)
        FAIL_GOTO(c->GetInt32(cellYIndex, &(_folderInfo->mCellY)), EXIT)

        *info = folderInfo;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    //} finally {
EXIT:
    c->Close();
    //}

    *info = NULL;
    return NOERROR;
}

ECode LauncherModel::AddItemToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Boolean notify)
{
    item->mContainer = container;
    item->mCellX = cellX;
    item->mCellY = cellY;
    // We store hotseat items in canonical form which is this orientation invariant position
    // in the hotseat
    if (ILauncher::Probe(context) != NULL && screen < 0 &&
            container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        AutoPtr<IHotseat> hotseat;
        ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
        hotseat->GetOrderInHotseat(cellX, cellY, &(item->mScreen));
    }
    else {
        item->mScreen = screen;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    item->OnAddToDatabase(context, values);

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    AutoPtr<ILauncherProvider> provider;
    app->GetLauncherProvider((ILauncherProvider**)&provider);
    provider->GenerateNewId(&(item->mId));
    values->Put(LauncherSettings::Favorites::_ID, item->mId);
    item->UpdateValuesWithCoordinates(values, item->mCellX, item->mCellY);

    //final StackTraceElement[] stackTrace = new Throwable().getStackTrace();
    AutoPtr<IRunnable> r = new MyRunnable5(this, cr, notify, values, item);
    return RunOnWorkerThread(r);
}

ECode LauncherModel::GetCellLayoutChildId(
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 localCellX,
    /* [in] */ Int32 localCellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [out] */ Int32* id);
{
    VALIDATE_NOT_NULL(id);

    *id = (((Int32)container & 0xFF) << 24)
            | (screen & 0xFF) << 16 | (localCellX & 0xFF) << 8 | (localCellY & 0xFF);
    return NOERROR;
}

ECode LauncherModel::GetCellCountX(
    /* [out] */ Int32* x);
{
    VALIDATE_NOT_NULL(x);

    *x = mCellCountX;
    return NOERROR;
}

ECode LauncherModel::GetCellCountY(
    /* [out] */ Int32* y);
{
    VALIDATE_NOT_NULL(y);

    *y = mCellCountY;
    return NOERROR;
}

ECode LauncherModel::UpdateWorkspaceLayoutCells(
    /* [in] */ Int32 shortAxisCellCount,
    /* [in] */ Int32 longAxisCellCount)
{
    mCellCountX = shortAxisCellCount;
    mCellCountY = longAxisCellCount;
    return NOERROR;
}

ECode LauncherModel::DeleteItemFromDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ItemInfo* item)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uriToDelete = LauncherSettings::Favorites::GetContentUri(item->mId, FALSE);

    AutoPtr<IRunnable> r = new MyRunnable6(this, cr, uriToDelete, values, item);
    return RunOnWorkerThread(r);
}

ECode LauncherModel::DeleteFolderContentsFromDatabase(
    /* [in] */ IContext* context,
    /* [in] */ FolderInfo* info)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IRunnable> r = new MyRunnable7(this, cr, info)
    return RunOnWorkerThread(r);
}

ECode LauncherModel::Initialize(
    /* [in] */ ILauncherModelCallback* _callbacks)
{
    synchronized (mLock) {
        mCallbacks = IWeakReference::Probe(_callbacks);
    }
    return NOERROR;
}

ECode LauncherModel::GetLauncherAppsCallback(
    /* [out] */ ILauncherAppsCallback** _callbacks)
{
    VALIDATE_NOT_NULL(_callbacks);

    *_callbacks = mLauncherAppsCallback;
    REFCOUNT_ADD(*_callbacks);
    return NOERROR;
}

ECode LauncherModel::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (DEBUG_LOADERS) {
        Slogger::D(TAG, "onReceive intent=" + intent);
    }

    String action;
    intent->GetAction(&action);
    Boolean res, res2;
    IIntent::ACTION_LOCALE_CHANGED.Equals(action, &res);
    if (Ires) {
        // If we have changed locale we need to clear out the labels in all apps/workspace.
        ForceReload();
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action, &res), res) {
         // Check if configuration change was an mcc/mnc change which would affect app resources
         // and we would need to clear out the labels in all apps/workspace. Same handling as
         // above for ACTION_LOCALE_CHANGED
         Configuration currentConfig = context.getResources().getConfiguration();
         if (mPreviousConfigMcc != currentConfig.mcc) {
               Log.d(TAG, "Reload apps on config change. curr_mcc:"
                   + currentConfig.mcc + " prevmcc:" + mPreviousConfigMcc);
               forceReload();
         }
         // Update previousConfig
         mPreviousConfigMcc = currentConfig.mcc;
    } else if ((ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED.Equals(action, &res), res)
                || (ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED.Equals(action, &res2), res2)) {
        if (mCallbacks != NULL) {
            AutoPtr<ILauncherModelCallbacks> _callbacks;
            mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                    (ILauncherModelCallbacks**)&_callbacks);
            if (_callbacks != NULL) {
                _callbacks->BindSearchablesChanged();
            }
        }
    }
    return NOERROR;
}

ECode LauncherModel::ForceReload()
{
    ResetLoadedState(true, true);

    // Do this here because if the launcher activity is running it will be restarted.
    // If it's not running startLoaderFromBackground will merely tell it that it needs
    // to reload.
    return StartLoaderFromBackground();
}

ECode LauncherModel::ResetLoadedState(
    /* [in] */ Boolean resetAllAppsLoaded,
    /* [in] */ Boolean resetWorkspaceLoaded)
{
    synchronized(mLock) {
        // Stop any existing loaders first, so they don't set mAllAppsLoaded or
        // mWorkspaceLoaded to true later
        StopLoaderLocked();
        if (resetAllAppsLoaded) mAllAppsLoaded = FALSE;
        if (resetWorkspaceLoaded) mWorkspaceLoaded = FALSE;
    }
    return NOERROR;
}

ECode LauncherModel::StartLoaderFromBackground()
{
    Boolean runLoader = FALSE;
    if (mCallbacks != NULL) {
        AutoPtr<ILauncherModelCallbacks> _callbacks;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&_callbacks);
        if (_callbacks != NULL) {
            // Only actually run the loader if they're not paused.
            Boolean res;
            _callbacks->SetLoadOnResume(&res);
            if (!res) {
                runLoader = TRUE;
            }
        }
    }
    if (runLoader) {
        return StartLoader(FALSE, -1);
    }
    return NOERROR;
}

Boolean LauncherModel::StopLoaderLocked()
{
    Boolean isLaunching = FALSE;
    AutoPtr<LoaderTask> oldTask = mLoaderTask;
    if (oldTask != NULL) {
        Boolean res;
        oldTask->IsLaunching(&res);
        if (res) {
            isLaunching = TRUE;
        }
        oldTask->StopLocked();
    }
    return isLaunching;
}

ECode LauncherModel::StartLoader(
    /* [in] */ Boolean isLaunching,
    /* [in] */ Int32 synchronousBindPage)
{
    synchronized(mLock) {
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "startLoader isLaunching=" + isLaunching);
        }

        // Clear any deferred bind-runnables from the synchronized load process
        // We must do this before any loading/binding is scheduled below.
        mDeferredBindRunnables->Clear();

        // Don't bother to start the thread if we know it's not going to do anything
        AutoPtr<ILauncherModelCallbacks> _callbacks;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (ILauncherModelCallbacks**)&_callbacks);
        if (mCallbacks != NULL && _callbacks != NULL) {
            // If there is already one running, tell it to stop.
            // also, don't downgrade isLaunching if we're already running
            if (!isLaunching) {
                StopLoaderLocked(&tmp);
            }
            mLoaderTask = new LoaderTask(mApp, isLaunching);
            if (synchronousBindPage > -1 && mAllAppsLoaded && mWorkspaceLoaded) {
                mLoaderTask->RunBindSynchronousPage(synchronousBindPage);
            }
            else {
                sWorkerThread->SetPriority(IThread::NORM_PRIORITY);
                sWorker->Post(mLoaderTask);
            }
        }
    }
    return NOERROR;
}

ECode LauncherModel::BindRemainingSynchronousPages()
{
    // Post the remaining side pages to be loaded
    Boolean res;
    mDeferredBindRunnables->IsEmpty(&res);
    if (!res) {
        Int32 size;
        mDeferredBindRunnables->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mDeferredBindRunnables->Get(i, (IInterface**)&obj);
            AutoPtr<IRunnable> r = IRunnable::Probe(obj);
            mHandler->Post(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
        mDeferredBindRunnables->Clear();
    }
    return NOERROR;
}

ECode LauncherModel::StopLoader()
{
    synchronized(mLock) {
        if (mLoaderTask != NULL) {
            mLoaderTask->StopLocked();
        }
    }
    return NOERROR;
}

ECode LauncherModel::IsAllAppsLoaded(
    /* [out] */ Boolean* result)

{
    VALIDATE_NOT_NULL(result);

    *result = mAllAppsLoaded;
    return NOERROR;
}

ECode LauncherModel::IsLoadingWorkspace(
    /* [out] */ Boolean* result)

{
    VALIDATE_NOT_NULL(result);

    synchronized(mLock) {
        if (mLoaderTask != NULL) {
            return mLoaderTask->IsLoadingWorkspace(&result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LauncherModel::EnqueuePackageUpdated(
    /* [in] */ PackageUpdatedTask* task)
{
    return sWorker->Post(task);
}

ECode LauncherModel::GetSortedWidgetsAndShortcuts(
    /* [in] */ IContext* context,
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IArrayList> widgetsAndShortcuts;
    CArrayList::New((IArrayList**)&widgetsAndShortcuts);

    // Get all user profiles.
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::APPWIDGET_SERVICE, (IInterface**)&obj);
    AutoPtr<IAppWidgetManager> widgetManager = IAppWidgetManager::Probe(obj);

    AutoPtr<IInterface> obj2;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj2);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj2);

    AutoPtr<IList> profiles;
    userManager->GetUserProfiles((IList**)&profiles);

    // Add the widgets for the managed profiles next.
    Int32 profileCount;
    profiles->GetSize(&profileCount);
    for (Int32 i = 0; i < profileCount; i++) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        AutoPtr<IUserHandle> profile = IUserHandle::Probe(obj);
        // Add the widget providers for the profile.
        AutoPtr<IList> providers;
        widgetManager->GetInstalledProvidersForProfile(profile, (IList**)&providers);
        widgetsAndShortcuts->AddAll(providers);
    }

    // Add all shortcuts for the user.
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IIntent> shortcutsIntent;
    CIntent::New(IIntent::ACTION_CREATE_SHORTCUT, (IIntent**)&shortcutsIntent);

    AutoPtr<IList> activities;
    packageManager->QueryIntentActivities(shortcutsIntent, 0, (IList**)&activities);
    widgetsAndShortcuts->AddAll(activities);

    AutoPtr<IComparator> c = new WidgetAndShortcutNameComparator(packageManager);
    Collections::Sort(widgetsAndShortcuts, c);

    *list = widgetsAndShortcuts;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode LauncherModel::GetShortcutInfo(
    /* [in] */ IPackageManager* manager,
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IContext* context,
    /* [out] */ ShortcutInfo** sinfo)
{
    VALIDATE_NOT_NULL(sinfo);

    return GetShortcutInfo(manager, intent, user, context, NULL, -1, -1, NULL, sinfo);
}

ECode LauncherModel::GetShortcutInfo(
    /* [in] */ IPackageManager* manager,
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 iconIndex,
    /* [in] */ Int32 titleIndex,
    /* [in] */ IHashMap* labelCache,
    /* [out] */ ShortcutInfo** sinfo)
{
    VALIDATE_NOT_NULL(sinfo);

    AutoPtr<IBitmap> icon;
    AutoPtr<ShortcutInfo> info = new ShortcutInfo();
    info->mUser = user;

    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    if (componentName == NULL) {
        *sinfo = NULL;
        return NOERROR;
    }

    AutoPtr<ILauncherActivityInfo> lai;
    mLauncherApps->ResolveActivity(intent, user, (ILauncherActivityInfo**)&lai);
    if (lai == NULL) {
        *sinfo = NULL;
        return NOERROR;
    }

    mIconCache->GetIcon(componentName, lai, labelCache, (IBitmap**)&icon);
    // the db
    if (icon == NULL) {
        if (c != NULL) {
            GetIconFromCursor(c, iconIndex, context, (IBitmap**)&icon);
        }
    }
    // the fallback icon
    if (icon == NULL) {
        GetFallbackIcon((IBitmap**)&icon);
        info->mUsingFallbackIcon = TRUE;
    }
    info->SetIcon(icon);

    // from the resource
    AutoPtr<IComponentName> key;
    lai->GetComponentName((IComponentName**)&key);
    Boolean res;
    if (labelCache != NULL && (labelCache->ContainsKey(key, &res), res)) {
        AutoPtr<IInterface> obj;
        labelCache->Get(key, (IInterface**)&obj);
        info->mTitle = ICharSequence::Probe(obj);
    }
    else {
        lai->GetLabel((ICharSequence**)&(info->mTitle));
        if (labelCache != NULL) {
            labelCache->Put(key, info->mTitle);
        }
    }
    // from the db
    if (info->mTitle == NULL) {
        if (c != NULL) {
            String str;
            c->GetString(titleIndex, &str);
            info->mTitle = CoreUtils::Convert(str);
        }
    }
    // fall back to the class name of the activity
    if (info->mTitle == NULL) {
        String name;
        componentName->GetClassName(&name);
        info->mTitle = CoreUtils::Convert(name);
    }

    AutoPtr<IPackageManager> manager;
    mApp->GetPackageManager((IPackageManager**)&manager);
    manager->GetUserBadgedLabel(info->mTitle, user, (ICharSequence**)&(info->mContentDescription));
    info->mItemType = LauncherSettings::Favorites::ITEM_TYPE_APPLICATION;
    *sinfo = info;
    REFCOUNT_ADD(*sinfo);
    return NOERROR;
}

ECode LauncherModel::GetWorkspaceShortcutItemInfosWithIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    synchronized(sBgLock) {
        Int32 size;
        sBgWorkspaceItems->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            sBgWorkspaceItems->Get(i, (IInterface**)&obj);

            AutoPtr<IItemInfo> info = IItemInfo::Probe(obj);
            if (IShortcutInfo::Probe(info) != NULL) {
                AutoPtr<ShortcutInfo> shortcut = (ShortcutInfo*)IObject::Probe(info);
                String uri;
                shortcut->mIntent->TtoUri(0, &uri);
                String uri2;
                intent->TtoUri(0, &uri2);
                Boolean res;
                uri.Equals(uri2, &res);
                if (res) {
                    items->Add(TO_IINTERFACE(shortcut));
                }
            }
        }
    }
    *list = items;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

AutoPtr<ShortcutInfo> LauncherModel::GetShortcutInfo(
    /* [in] */ ICursor* c,
    /* [in] */ IContext* context,
    /* [in] */ Int32 iconTypeIndex,
    /* [in] */ Int32 iconPackageIndex,
    /* [in] */ Int32 iconResourceIndex,
    /* [in] */ Int32 iconIndex,
    /* [in] */ Int32 titleIndex)
{
    AutoPtr<IBitmap> icon;
    AutoPtr<ShortcutInfo> info = new ShortcutInfo();
    info->mItemType = LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT;

    // TODO: If there's an explicit component and we can't install that, delete it.
    String str;
    c->GetString(titleIndex, &str);
    info->mTitle = CoreUtils::Convert(str);

    AutoPtr<IPackageManager> manager;
    mApp->GetPackageManager((IPackageManager**)&manager);
    manager->GetUserBadgedLabel(info->mTitle, info->mUser,
            (ICharSequence**)&(info->mContentDescription));

    Int32 iconType;
    c->GetInt32(iconTypeIndex, &iconType);
    switch (iconType) {
        case LauncherSettings::Favorites::ICON_TYPE_RESOURCE:
        {
            String packageName;
            c->GetString(iconPackageIndex, &packageName);
            String resourceName;
            c->GetString(iconResourceIndex, &resourceName);
            AutoPtr<IPackageManager> packageManager;
            context->GetPackageManager((IPackageManager**)&packageManager);
            info->mCustomIcon = FALSE;
                // the resource
                //try {
                AutoPtr<IResources> resources;
                packageManager->GetResourcesForApplication(packageName, (IResources**)&resources);
                if (resources != NULL) {
                    Int32 id;
                    resources->GetIdentifier(resourceName, NULL, NULL, &id);
                    AutoPtr<IUserHandle> userH;
                    Process::MyUserHandle((IUserHandle**)&userH);
                    AutoPtr<IDrawable> drawable;
                    mIconCache->GetFullResIcon(resources, id, userH, (IDrawable**)&drawable);
                    Utilities::CreateIconBitmap(drawable, context, (IBitmap**)&icon);
                }
                //} catch (Exception e) {
                    // drop this. we have other places to look for icons
                //}
            // the db
            if (icon == NULL) {
                GetIconFromCursor(c, iconIndex, context, (IBitmap**)&icon);
            }
            // the fallback icon
            if (icon == NULL) {
                GetFallbackIcon((IBitmap**)&icon);
                info->mUsingFallbackIcon = TRUE;
            }
            break;
        }
        case LauncherSettings::Favorites::ICON_TYPE_BITMAP:
        {
            GetIconFromCursor(c, iconIndex, context, (IBitmap**)&icon);
            if (icon == NULL) {
                GetFallbackIcon((IBitmap**)&icon);
                info->mCustomIcon = FALSE;
                info->mUsingFallbackIcon = TRUE;
            }
            else {
                info->mCustomIcon = TRUE;
            }
            break;
        }
        default:
        {
            GetFallbackIcon((IBitmap**)&icon);
            info->mUsingFallbackIcon = TRUE;
            info->mCustomIcon = FALSE;
            break;
        }
    }
    info->SetIcon(icon);
    return info;
}

ECode LauncherModel::GetIconFromCursor(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 iconIndex,
    /* [in] */ IContext* context,
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = NULL;

    //@SuppressWarnings("all") // suppress dead code warning
    // Boolean debug = FALSE;
    // if (debug) {
    //     Log.d(TAG, "getIconFromCursor app="
    //             + c.getString(c.getColumnIndexOrThrow(LauncherSettings.Favorites.TITLE)));
    // }
    AutoPtr<ArrayOf<Byte> > data;
    c->GetBlob(iconIndex, (ArrayOf<Byte>**)&data);
    //try {
    AutoPtr<IBitmap> bitmap;
    BitmapFactory::DecodeByteArray(data, 0, data->GetLength(), (IBitmap**)&bitmap);
    if (FAILED(Utilities::CreateIconBitmap(bitmap, context, map))){
        *map = NULL
        return NOERROR;
    }
    //} catch (Exception e) {
        //return null;
    //}
    return NOERROR;
}

ECode LauncherModel::AddShortcut(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Boolean notify,
    /* [out] */ ShortcutInfo** sinfo)
{
    VALIDATE_NOT_NULL(sinfo);

    AutoPtr<ShortcutInfo> info;
    InfoFromShortcutIntent(context, data, NULL, (ShortcutInfo**)&info);
    if (info == NULL) {
        *sinfo = NULL;
        return NOERROR;
    }
    AddItemToDatabase(context, info, container, screen, cellX, cellY, notify);

    *sinfo = info;
    REFCOUNT_ADD(*sinfo);
    return NOERROR;
}

ECode LauncherModel::FindAppWidgetProviderInfoWithComponent(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* component,
    /* [out] */ IAppWidgetProviderInfo** pinfo)
{
    VALIDATE_NOT_NULL(pinfo);

    AutoPtr<IAppWidgetManager> manager = AppWidgetManager::GetInstance(context);
    AutoPtr<IList> widgets;
    manager->GetInstalledProviders((IList**)&widgets);

    Int32 size;
    widgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        widgets->Get(i, (IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);

        AutoPtr<IComponentName> name;
        info->GetProvider((IComponentName**)&name);
        Boolean res;
        name->Equals(component, &res);
        if (res) {
            *pinfo = info;
            REFCOUNT_ADD(*pinfo);
            return NOERROR;
        }

    }

    *pinfo = NULL;
    return NOERROR;
}

ECode LauncherModel::ResolveWidgetsForMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& mimeType,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IList> supportedConfigurationActivities;
    CArrayList::New((IList**)&supportedConfigurationActivities);

    AutoPtr<IIntent> supportsIntent;
    CIntent::New(IInstallWidgetReceiver::ACTION_SUPPORTS_CLIPDATA_MIMETYPE,
            (IIntent**)&supportsIntent);
    supportsIntent->SetType(mimeType);

    // Create a set of widget configuration components that we can test against
    AutoPtr<IAppWidgetManager> manager = AppWidgetManager::GetInstance(context);
    AutoPtr<IList> widgets;
    manager->GetInstalledProviders((IList**)&widgets);
    AutoPtr<IHashMap> configurationComponentToWidget;
    CHashMap::New((IHashMap**)&configurationComponentToWidget);

    Int32 size;
    widgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        widgets->Get((IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);

        configurationComponentToWidget->Put(info.configure, TO_IINTERFACE(info));
    }

    // Run through each of the intents that can handle this type of clip data, and cross
    // reference them with the components that are actual configuration components
    AutoPtr<IList> activities;
    packageManager->QueryIntentActivities(supportsIntent,
            IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&activities);

    activities->GetSize(&size);
    for (Int32 i = 0; i < size ; i++) {
        AutoPtr<IInterface> obj;
        activities->Get(i, (IInterface**)&obj);
        AutoPtr<ResolveInfo> info = (ResolveInfo*)IObject::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo = info->mActivityInfo;
        String pname;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(activityInfo)-GetName(&name);
        AutoPtr<IComponentName> infoComponent;
        CComponentName::New(pname, name, (IComponentName**)&infoComponent);
        Boolean res;
        configurationComponentToWidget->ContainsKey(infoComponent, &res);
        if (res) {
            AutoPtr<IInterface> obj;
            configurationComponentToWidget->Get(TO_IINTERFACE(infoComponent), (IInterface**)&obj);
            AutoPtr<IAppWidgetProviderInfo> winfo = IAppWidgetProviderInfo::Probe(obj);
            AutoPtr<WidgetMimeTypeHandlerData> = data;
                    new InstallWidgetReceiver::WidgetMimeTypeHandlerData(info, winfo);
            supportedConfigurationActivities->Add(TO_IINTERFACE(data));
        }
    }

    *list = supportedConfigurationActivities;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode LauncherModel::InfoFromShortcutIntent(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data,
    /* [in] */ IBitmap* fallbackIcon,
    /* [out] */ ShortcutInfo** sinfo)
{
    VALIDATE_NOT_NULL(sinfo);

    AutoPtr<IParcelable> p;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_INTENT, (IParcelable**)&p);
    AutoPtr<IIntent> intent = IIntent::Probe(p);

    String name;
    data->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &name);

    AutoPtr<IParcelable> bitmap;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_ICON, (IParcelable**)&bitmap);

    AutoPtr<IParcelable> p2;
    data->GetParcelableExtra(IItemInfo::EXTRA_PROFILE, (IParcelable**)&p2);
    AutoPtr<IUserHandle> user = IUserHandle::Probe(p2);
    if (user == NULL) {
        Process::MyUserHandle((IUserHandle**)&user);
    }

    if (intent == NULL) {
        // If the intent is null, we can't construct a valid ShortcutInfo, so we return null
        Slogger::E(TAG, "Can't construct ShorcutInfo with null intent");
        *sinfo = NULL;
        return NOERROR;
    }

    AutoPtr<IBitmap> icon;
    Boolean customIcon = FALSE;
    AutoPtr<IShortcutIconResource> iconResource;

    if (bitmap != NULL && IBitmap::Probe(bitmap) != NULL) {
        AutoPtr<IDrawable> drawable = new FastBitmapDrawable(IBitmap::Probe(bitmap));
        Utilities::CreateIconBitmap(drawable, context, (IBitmap**)&icon);
        customIcon = TRUE;
    }
    else {
        AutoPtr<IParcelable> extra;
        data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_ICON_RESOURCE, (IParcelable**)&extra);
        if (extra != NULL && IShortcutIconResource::Probe(extra) != NULL) {
            //try {
            iconResource = IShortcutIconResource::Probe(extra);
            AutoPtr<IPackageManager> packageManager;
            FAIL_GOTO(context->GetPackageManager((IPackageManager**)&packageManager), ERROR)
            AutoPtr<IResources> resources;
            FAIL_GOTO(packageManager->GetResourcesForApplication(iconResource.packageName,
                    (IResources**)&resources), ERROR)
            Int32 id;
            FAIL_GOTO(resources->GetIdentifier(iconResource.resourceName, NULL, NULL, &id), ERROR)
            AutoPtr<IDrawable> drawable;
            FAIL_GOTO(mIconCache->GetFullResIcon(resources, id, user, (IDrawable**)&drawable), ERROR)
            FAIL_GOTO(Utilities::CreateIconBitmap(drawable, context, (IBitmap**)&icon), ERROR)
            //} catch (Exception e) {
    ERROR:
                Slogger::W(TAG, "Could not load shortcut icon: " + extra);
            //}
        }
    }

    AutoPtr<ShortcutInfo> info = new ShortcutInfo();

    if (icon == NULL) {
        if (fallbackIcon != NULL) {
            icon = fallbackIcon;
        }
        else {
            GetFallbackIcon((IBitmap**)&icon));
            info->mUsingFallbackIcon = TRUE;
        }
    }
    info->SetIcon(icon);

    info->mTitle = name;
    AutoPtr<IPackageManager> packageManager;
    mApp.getPackageManager((IPackageManager**)&packageManager);
    packageManager->GetUserBadgedLabel(name, info->mUser, (ICharSequence**)&mContentDescription);
    info->mIntent = intent;
    info->mCustomIcon = customIcon;
    info->mIconResource = iconResource;

    *sinfo = info;
    REFCOUNT_ADD(*sinfo);
    return NOERROR;
}

ECode LauncherModel::QueueIconToBeChecked(
    /* [in] */ IHashMap* cache,
    /* [in] */ ShortcutInfo* info,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 iconIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // If apps can't be on SD, don't even bother.
    if (!mAppsCanBeOnRemoveableStorage) {
        *result = FALSE;
        return NOERROR;
    }
    // If this icon doesn't have a custom icon, check to see
    // what's stored in the DB, and if it doesn't match what
    // we're going to show, store what we are going to show back
    // into the DB.  We do this so when we're loading, if the
    // package manager can't find an icon (for example because
    // the app is on SD) then we can use that instead.
    if (!info->mCustomIcon && !info->mUsingFallbackIcon) {
        AutoPtr<ArrayOf<Byte> > blob;
        c->GetBlob(iconIndex, (ArrayOf<Byte>**)&blob);
        AutoPtr<IArrayOf> array = CoreUtils::Convert(blob);
        cache->Put(TO_IINTERFACE(info), TO_IINTERFACE(array));
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LauncherModel::UpdateSavedIcon(
    /* [in] */ IContext* context,
    /* [in] */ ShortcutInfo* info,
    /* [in] */ ArrayOf<Byte>* data)
{
    Boolean needSave = FALSE;
    //try {
    if (data != NULL) {
        AutoPtr<IBitmap> saved;
        FAIL_GOTO(BitmapFactory::DecodeByteArray(data, 0, data->GetLength(), (IBitmap**)&saved), ERROR)
        AutoPtr<IBitmap> loaded;
        FAIL_GOTO(info->GetIcon(mIconCache, (IBitmap**)&loaded), ERROR)
        FAIL_GOTO(saved->SameAs(loaded, &needSave), ERROR)
        needSave = !needSave;
    }
    else {
        needSave = TRUE;
    }
    //} catch (Exception e) {
ERROR:
        needSave = TRUE;
    //}
    if (needSave) {
        Slogger::D(TAG, "going to save icon bitmap for info=" + info);
        // This is slower than is ideal, but this only happens once
        // or when the app is updated with a new icon.
        UpdateItemInDatabase(context, info);
    }
    return NOERROR;
}

AutoPtr<FolderInfo> LauncherModel::FindOrMakeFolder(
    /* [in] */ IHashMap* folders,
    /* [in] */ Int64 id)
{
    // See if a placeholder was created for us already
    AutoPtr<IInterface> obj;
    folders->Get(id, (IInterface**)&obj);
    AutoPtr<FolderInfo> folderInfo = (FolderInfo*)IObject::Probe(obj);
    if (folderInfo == NULL) {
        // No placeholder -- create a new instance
        folderInfo = new FolderInfo();
        AutoPtr<IInteger64> key = CoreUtils::Convert(id);
        folders->Put(TO_IINTERFACE(key), TO_IINTERFACE(folderInfo));
    }
    return folderInfo;
}

ECode LauncherModel::GetAppNameComparator(
    /* [out] */ IComparator** comparator)
{
    VALIDATE_NOT_NULL(comparator);

    AutoPtr<ICollator> collator;
    Collator::GetInstance((ICollator**)&collator);
    AutoPtr<IComparator> c = new MyComparator3(collator);
    *comparator = c;
    REFCOUNT_ADD(*comparator);
    return NOERROR;
}

ECode LauncherModel::GetWidgetNameComparator(
    /* [out] */ IComparator** comparator)
{
    VALIDATE_NOT_NULL(comparator);

    AutoPtr<ICollator> collator;
    Collator::GetInstance((ICollator**)&collator);
    AutoPtr<IComparator> c = new MyComparator4(collator);
    *comparator = c;
    REFCOUNT_ADD(*comparator);
    return NOERROR;
}

ECode LauncherModel::GetComponentNameFromResolveInfo(
    /* [in] */ IResolveInfo* info,
    /* [out] */ IComponentName** outname)
{
    VALIDATE_NOT_NULL(outname);

    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    if (activityInfo != NULL) {
        String pname;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(activityInfo)-GetName(&name);
        return CComponentName::New(pname, name, (IComponentName**)&outname);
    }
    else {
        AutoPtr<IServiceInfo> serviceInfo;
        info->GetServiceInfo((IServiceInfo**)serviceInfo);
        String pname;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(serviceInfo)-GetName(&name);
        return CComponentName::New(packageName, name, (IComponentName**)&outname);
    }
}

ECode LauncherModel::DumpState()
{
    Slogger::D(TAG, "mCallbacks=" + mCallbacks);
    ApplicationInfo::DumpApplicationInfoList(TAG, "mAllAppsList.data", mBgAllAppsList.data);
    ApplicationInfo::DumpApplicationInfoList(TAG, "mAllAppsList.added", mBgAllAppsList.added);
    ApplicationInfo::DumpApplicationInfoList(TAG, "mAllAppsList.removed", mBgAllAppsList.removed);
    ApplicationInfo::DumpApplicationInfoList(TAG, "mAllAppsList.modified", mBgAllAppsList.modified);
    if (mLoaderTask != NULL) {
        mLoaderTask->DumpState();
    }
    else {
        Slogger::D(TAG, "mLoaderTask=null");
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos