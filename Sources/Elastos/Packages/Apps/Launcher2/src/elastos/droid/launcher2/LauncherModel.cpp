
#include "elastos/droid/launcher2/LauncherModel.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "elastos/droid/launcher2/InstallWidgetReceiver.h"
#include "elastos/droid/launcher2/FastBitmapDrawable.h"
#include "elastos/droid/launcher2/Utilities.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Arrays.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::Content::Pm::EIID_ILauncherAppsCallback;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentShortcutIconResource;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICloneable;
using Elastos::Core::Thread;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IMap;
using Elastos::Utility::Arrays;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherModel::ShortcutNameComparator, Object,
        IComparator);

LauncherModel::ShortcutNameComparator::ShortcutNameComparator()
{
    CHashMap::New((IHashMap**)&mLabelCache);
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
}

LauncherModel::ShortcutNameComparator::ShortcutNameComparator(
    /* [in] */ IHashMap* labelCache)
    : mLabelCache(labelCache)
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
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
    _a->GetComponentName((IComponentName**)&keyA);
    AutoPtr<IComponentName> keyB;
    _b->GetComponentName((IComponentName**)&keyB);

    Boolean res;
    mLabelCache->ContainsKey(TO_IINTERFACE(keyA), &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(TO_IINTERFACE(keyA), (IInterface**)&obj);
        labelA = ICharSequence::Probe(obj);
    }
    else {
        _a->GetLabel((ICharSequence**)&labelA);
        mLabelCache->Put(TO_IINTERFACE(keyA), TO_IINTERFACE(labelA));
    }

    mLabelCache->ContainsKey(TO_IINTERFACE(keyB), &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(TO_IINTERFACE(keyB), (IInterface**)&obj);
        labelB = ICharSequence::Probe(obj);
    }
    else {
        _b->GetLabel((ICharSequence**)&labelB);

        mLabelCache->Put(TO_IINTERFACE(keyB), TO_IINTERFACE(labelB));
    }
    return mCollator->Compare(TO_IINTERFACE(labelA), TO_IINTERFACE(labelB), result);
}

CAR_INTERFACE_IMPL(LauncherModel::WidgetAndShortcutNameComparator, Object,
        IComparator);

LauncherModel::WidgetAndShortcutNameComparator::WidgetAndShortcutNameComparator(
    /* [in] */ IPackageManager* pm)
    : mPackageManager(pm)
{
    CHashMap::New((IHashMap**)&mLabelCache);
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
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
        mLabelCache->Get(TO_IINTERFACE(a), (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&labelA);
    }
    else {
        AutoPtr<ICharSequence> cchar;
        if (IAppWidgetProviderInfo::Probe(a) != NULL) {
            IAppWidgetProviderInfo::Probe(a)->LoadLabel(mPackageManager, &labelA);
            cchar = CoreUtils::Convert(labelA);
        }
        else {
            IResolveInfo::Probe(a)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(&labelA);
        }
        mLabelCache->Put(TO_IINTERFACE(a), TO_IINTERFACE(cchar));
    }
    mLabelCache->ContainsKey(TO_IINTERFACE(b), &res);
    if (res) {
        AutoPtr<IInterface> obj;
        mLabelCache->Get(TO_IINTERFACE(b), (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&labelB);
    }
    else {
        AutoPtr<ICharSequence> cchar;
        if (IAppWidgetProviderInfo::Probe(b) != NULL) {
            IAppWidgetProviderInfo::Probe(b)->LoadLabel(mPackageManager, &labelB);
            cchar = CoreUtils::Convert(labelB);
        }
        else {
            IResolveInfo::Probe(b)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
            cchar->ToString(&labelB);
        }
        mLabelCache->Put(TO_IINTERFACE(b), TO_IINTERFACE(cchar));
    }
    Int32 compareResult;
    AutoPtr<ICharSequence> ccharA = CoreUtils::Convert(labelA);
    AutoPtr<ICharSequence> ccharB = CoreUtils::Convert(labelB);
    mCollator->Compare(TO_IINTERFACE(ccharA), TO_IINTERFACE(ccharB), &compareResult);
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
        AutoPtr<ItemInfo> item = (ItemInfo*)IItemInfo::Probe(obj);
        item->Unbind();
    }

    mTmpAppWidgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTmpAppWidgets->Get(i, (IInterface**)&obj);
        AutoPtr<ItemInfo> item = (ItemInfo*)IItemInfo::Probe(obj);
        item->Unbind();
    }
    return NOERROR;
}

LauncherModel::MyRunnable2::MyRunnable2(
    /* [in] */ Int64 itemId,
    /* [in] */ ItemInfo* item)
    : mItemId(itemId)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable2::Run()
{
    synchronized(LauncherModel::sBgLock) {
        LauncherModel::CheckItemInfoLocked(mItemId, mItem);
    }
    return NOERROR;
}

LauncherModel::MyRunnable3::MyRunnable3(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ Int64 itemId,
    /* [in] */ ItemInfo* item)
    : mCr(cr)
    , mUri(uri)
    , mValues(values)
    , mItemId(itemId)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable3::Run()
{
    Int32 tmp;
    mCr->Update(mUri, mValues, String(NULL), NULL, &tmp);

    // Lock on mBgLock *after* the db operation
    synchronized(LauncherModel::sBgLock) {
        CheckItemInfoLocked(mItemId, mItem);

        if (mItem->mContainer != LauncherSettings::Favorites::CONTAINER_DESKTOP &&
                mItem->mContainer != LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            // Item is in a folder, make sure this folder exists
            Boolean res;
            AutoPtr<IInteger64> value = CoreUtils::Convert(mItem->mContainer);
            sBgFolders->ContainsKey(TO_IINTERFACE(value), &res);
            if (!res) {
                // An items container is being set to a that of an item which is not in
                // the list of Folders.
                StringBuilder sb;
                sb += "item: ";
                sb += TO_STR(mItem);
                sb += " container being set to: ";
                sb += mItem->mContainer;
                sb += ", not in the list of folders";
                String msg = sb.ToString();
                Slogger::E(TAG, msg);
                assert(0);
                //Launcher::DumpDebugLogsToConsole();
            }
        }

        // Items are added/removed from the corresponding FolderInfo elsewhere, such
        // as in Workspace.onDrop. Here, we just add/remove them from the list of items
        // that are on the desktop, as appropriate
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger64> id = CoreUtils::Convert(mItemId);
        sBgItemsIdMap->Get(TO_IINTERFACE(id), (IInterface**)&obj);
        AutoPtr<IItemInfo> modelItem = IItemInfo::Probe(obj);
        ItemInfo* _modelItem = (ItemInfo*)modelItem.Get();
        if (_modelItem->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP ||
                _modelItem->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            switch (_modelItem->mItemType) {
                case ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_APPLICATION:
                case ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT:
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
    /* [in] */ IContentResolver* cr,
    /* [in] */ Boolean notify,
    /* [in] */ IContentValues* values,
    /* [in] */ ItemInfo* item)
    : mCr(cr)
    , mNotify(notify)
    , mValues(values)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable5::Run()
{
    assert(0);
    // String transaction = "DbDebug    Add item (" + item.title + ") to db, id: "
    //         + item.id + " (" + container + ", " + screen + ", " + cellX + ", "
    //         + cellY + ")";
    // Launcher::sDumpLogs->Add(transaction);
    // Slogger::D(TAG, transaction);

    AutoPtr<IUri> tmp;
    mCr->Insert(mNotify ? LauncherSettings::Favorites::CONTENT_URI :
            LauncherSettings::Favorites::CONTENT_URI_NO_NOTIFICATION, mValues, (IUri**)&tmp);

    // Lock on mBgLock *after* the db operation
    synchronized(LauncherModel::sBgLock) {
        CheckItemInfoLocked(mItem->mId, mItem/*, stackTrace*/);
        AutoPtr<IInteger64> value = CoreUtils::Convert(mItem->mId);
        LauncherModel::sBgItemsIdMap->Put(TO_IINTERFACE(value), TO_IINTERFACE(mItem));
        switch (mItem->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
            {
                AutoPtr<IInteger64> value = CoreUtils::Convert(mItem->mId);
                sBgFolders->Put(TO_IINTERFACE(value), TO_IINTERFACE(mItem));
                // Fall through
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT:
            {
                if (mItem->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP ||
                        mItem->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
                    LauncherModel::sBgWorkspaceItems->Add(TO_IINTERFACE(mItem));
                }
                else {
                    Boolean res;
                    AutoPtr<IInteger64> value = CoreUtils::Convert(mItem->mContainer);
                    LauncherModel::sBgFolders->ContainsKey(TO_IINTERFACE(value), &res);
                    if (!res) {
                        // Adding an item to a folder that doesn't exist.
                        assert(0);
                        // String msg = "adding item: " + item + " to a folder that " +
                        //         " doesn't exist";
                        // Slogger::E(TAG, msg);
                        // Launcher::DumpDebugLogsToConsole();
                    }
                }
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                LauncherModel::sBgAppWidgets->Add(TO_IINTERFACE(mItem)); //(LauncherAppWidgetInfo) item
                break;
        }
    }
    return NOERROR;
}

LauncherModel::MyRunnable6::MyRunnable6(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uriToDelete,
    /* [in] */ ItemInfo* item)
    : mCr(cr)
    , mUriToDelete(uriToDelete)
    , mItem(item)
{
}

ECode LauncherModel::MyRunnable6::Run()
{
    assert(0);
    // String transaction = "DbDebug    Delete item (" + item.title + ") from db, id: "
    //         + item.id + " (" + item.container + ", " + item.screen + ", " + item.cellX +
    //         ", " + item.cellY + ")";
    // Launcher::sDumpLogs->Add(transaction);
    // Slogger::D(TAG, transaction);

    Int32 tmp;
    mCr->Delete(mUriToDelete, String(NULL), NULL, &tmp);

    // Lock on mBgLock *after* the db operation
    synchronized(LauncherModel::sBgLock) {
        switch (mItem->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
            {
                AutoPtr<IInteger64> obj = CoreUtils::Convert(mItem->mId);
                LauncherModel::sBgFolders->Remove(TO_IINTERFACE(obj));
                AutoPtr<ICollection> value;
                sBgItemsIdMap->GetValues((ICollection**)&value);
                AutoPtr<ArrayOf<IInterface*> > array;
                value->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); i++) {
                    AutoPtr<IItemInfo> info = IItemInfo::Probe((*array)[i]);

                    if (mItem->mContainer == mItem->mId) {
                        // We are deleting a folder which still contains items that
                        // think they are contained by that folder.
                        assert(0);
                        // String msg = "deleting a folder (" + item + ") which still " +
                        //         "contains items (" + info + ")";
                        // Slogger::E(TAG, msg);
                        // Launcher::DumpDebugLogsToConsole();
                    }
                }
                LauncherModel::sBgWorkspaceItems->Remove(TO_IINTERFACE(mItem));
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
                LauncherModel::sBgWorkspaceItems->Remove(TO_IINTERFACE(mItem));
                break;
            case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                LauncherModel::sBgAppWidgets->Remove(TO_IINTERFACE(mItem));
                break;
        }
        AutoPtr<IInteger64> value = CoreUtils::Convert(mItem->mId);
        sBgItemsIdMap->Remove(TO_IINTERFACE(value));
        sBgDbIconCache->Remove(TO_IINTERFACE(mItem));
    }
    return NOERROR;
}

LauncherModel::MyRunnable7::MyRunnable7(
    /* [in] */ IContentResolver* cr,
    /* [in] */ FolderInfo* info)
    : mCr(cr)
    , mInfo(info)
{
}

ECode LauncherModel::MyRunnable7::Run()
{
    Int32 tmp;
    AutoPtr<IUri> uri;
    LauncherSettings::Favorites::GetContentUri(mInfo->mId, FALSE, (IUri**)&uri);
    mCr->Delete(uri, String(NULL), NULL, &tmp);
    // Lock on mBgLock *after* the db operation
    synchronized(LauncherModel::sBgLock) {
        AutoPtr<IInteger64> value = CoreUtils::Convert(mInfo->mId);
        LauncherModel::sBgItemsIdMap->Remove(TO_IINTERFACE(value));
        LauncherModel::sBgFolders->Remove(TO_IINTERFACE(value));
        LauncherModel::sBgDbIconCache->Remove(TO_IINTERFACE(mInfo));
        LauncherModel::sBgWorkspaceItems->Remove(TO_IINTERFACE(mInfo));
    }

    StringBuilder sb;
    sb += LauncherSettings::Favorites::CONTAINER;
    sb += "=";
    sb += mInfo->mId;
    mCr->Delete(LauncherSettings::Favorites::CONTENT_URI_NO_NOTIFICATION,
            sb.ToString(), NULL, &tmp);
    // Lock on mBgLock *after* the db operation
    synchronized(LauncherModel::sBgLock) {
        Int32 size;
        mInfo->mContents->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mInfo->mContents->Get(i, (IInterface**)&obj);
            AutoPtr<ItemInfo> childInfo = (ItemInfo*)IItemInfo::Probe(obj);

            AutoPtr<IInteger64> value = CoreUtils::Convert(childInfo->mId);
            sBgItemsIdMap->Remove(TO_IINTERFACE(value));
            sBgDbIconCache->Remove(TO_IINTERFACE(childInfo));
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
            mHost, PackageUpdatedTask::OP_UPDATE, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
            mHost, PackageUpdatedTask::OP_REMOVE, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
            mHost, PackageUpdatedTask::OP_ADD, array, user);
    return mHost->EnqueuePackageUpdated(task);
}

ECode LauncherModel::LauncherAppsCallback::OnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    if (!replacing) {
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                mHost, PackageUpdatedTask::OP_ADD, packageNames, user);
        mHost->EnqueuePackageUpdated(task);
        if (mHost->mAppsCanBeOnRemoveableStorage) {
            // Only rebind if we support removable storage. It catches the
            // case where apps on the external sd card need to be reloaded.
            mHost->StartLoaderFromBackground();
        }
    }
    else {
        // If we are replacing then just update the packages in the list
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                mHost, PackageUpdatedTask::OP_UPDATE, packageNames, user);
        mHost->EnqueuePackageUpdated(task);
    }
    return NOERROR;
}

ECode LauncherModel::LauncherAppsCallback::OnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    if (!replacing) {
        AutoPtr<PackageUpdatedTask> task = new PackageUpdatedTask(
                mHost, PackageUpdatedTask::OP_UNAVAILABLE, packageNames, user);
        return mHost->EnqueuePackageUpdated(task);
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable8::MyRunnable8(
    /* [in] */ LoaderTask* host)
    : mHost(host)
{
}

ECode LauncherModel::LoaderTask::MyRunnable8::Run()
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

CAR_INTERFACE_IMPL(LauncherModel::LoaderTask::MyComparator, Object,
        IComparator);

ECode LauncherModel::LoaderTask::MyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    ItemInfo* lhsinfo = (ItemInfo*)(IObject::Probe(lhs));
    ItemInfo* rhsinfo = (ItemInfo*)(IObject::Probe(rhs));
    *result = (Int32)(lhsinfo->mContainer - rhsinfo->mContainer);
    return NOERROR;
}

CAR_INTERFACE_IMPL(LauncherModel::LoaderTask::MyComparator2, Object,
        IComparator);

ECode LauncherModel::LoaderTask::MyComparator2::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 cellCountX;
    LauncherModel::GetCellCountX(&cellCountX);
    Int32 cellCountY;
    LauncherModel::GetCellCountY(&cellCountY);
    Int32 screenOffset = cellCountX * cellCountY;
    Int32 containerOffset = screenOffset * (ILauncher::SCREEN_COUNT + 1); // +1 hotseat
    ItemInfo* lhsinfo = (ItemInfo*)(IObject::Probe(lhs));
    ItemInfo* rhsinfo = (ItemInfo*)(IObject::Probe(rhs));
    Int64 lr = (lhsinfo->mContainer * containerOffset + lhsinfo->mScreen * screenOffset +
            lhsinfo->mCellY * cellCountX + lhsinfo->mCellX);
    Int64 rr = (rhsinfo->mContainer * containerOffset + rhsinfo->mScreen * screenOffset +
            rhsinfo->mCellY * cellCountX + rhsinfo->mCellX);
    *result = (Int32)(lr - rr);
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable9::MyRunnable9(
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

ECode LauncherModel::LoaderTask::MyRunnable9::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        return callbacks->BindItems(mWorkspaceItems, mStart, mStart + mChunkSize);
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable10::MyRunnable10(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IHashMap* folders)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mFolders(folders)
{
}

ECode LauncherModel::LoaderTask::MyRunnable10::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        return callbacks->BindFolders(mFolders);
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable11::MyRunnable11(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ ILauncherAppWidgetInfo* widget)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mWidget(widget)
{
}

ECode LauncherModel::LoaderTask::MyRunnable11::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        return callbacks->BindAppWidget(mWidget);
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable12::MyRunnable12(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
{
}

ECode LauncherModel::LoaderTask::MyRunnable12::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        return callbacks->StartBinding();
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable13::MyRunnable13(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ Int32 currentScreen)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mCurrentScreen(currentScreen)
{
}

ECode LauncherModel::LoaderTask::MyRunnable13::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        callbacks->OnPageBoundSynchronously(mCurrentScreen);
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable14::MyRunnable14(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
{
}

ECode LauncherModel::LoaderTask::MyRunnable14::Run()
{
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        callbacks->FinishBindingItems();
    }

    // If we're profiling, ensure this is the last thing in the queue.
    if (DEBUG_LOADERS) {
        assert(0);
        // Slogger::D(TAG, "bound workspace in "
        //     + (SystemClock.uptimeMillis()-t) + "ms");
    }

    mHost->mIsLoadingAndBindingWorkspace = FALSE;
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable15::MyRunnable15(
    /* [in] */ LoaderTask* host,
    /* [in] */ ILauncherModelCallbacks* oldCallbacks,
    /* [in] */ IArrayList* list)
    : mHost(host)
    , mOldCallbacks(oldCallbacks)
    , mList(list)
{
}

ECode LauncherModel::LoaderTask::MyRunnable15::Run()
{
    Int64 t = SystemClock::GetUptimeMillis();
    AutoPtr<ILauncherModelCallbacks> callbacks;
    mHost->TryGetCallbacks(mOldCallbacks, (ILauncherModelCallbacks**)&callbacks);
    if (callbacks != NULL) {
        callbacks->BindAllApplications(mList);
    }
    if (DEBUG_LOADERS) {
        assert(0);
        // Slogger::D(TAG, "bound all " + list.size() + " apps from cache in "
        //         + (SystemClock.GetUptimeMillis()-t) + "ms");
    }
    return NOERROR;
}

LauncherModel::LoaderTask::MyRunnable16::MyRunnable16(
    /* [in] */ ILauncherModelCallbacks* _callbacks,
    /* [in] */ Boolean firstProfile,
    /* [in] */ IArrayList* added)
    : mCallbacks(_callbacks)
    , mFirstProfile(firstProfile)
    , mAdded(added)
{
}

ECode LauncherModel::LoaderTask::MyRunnable16::Run()
{
    Int64 t = SystemClock::GetUptimeMillis();
    if (mCallbacks != NULL) {
        if (mFirstProfile) {
            mCallbacks->BindAllApplications(mAdded);
        } else {
            mCallbacks->BindAppsAdded(mAdded);
        }
        if (DEBUG_LOADERS) {
            assert(0);
            // Slogger::D(TAG, "bound " + mAdded.size() + " apps in "
            //     + (SystemClock.uptimeMillis() - t) + "ms");
        }
    }
    else {
        Slogger::I(TAG, "not binding apps: no Launcher activity");
    }
    return NOERROR;
}

LauncherModel::MyRunnable17::MyRunnable17(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks,
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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        cb = ILauncherModelCallbacks::Probe(obj);
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
    /* [in] */ ILauncherModelCallbacks* modelCallbacks,
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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        cb = ILauncherModelCallbacks::Probe(obj);
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
    /* [in] */ ILauncherModelCallbacks* modelCallbacks,
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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        cb = ILauncherModelCallbacks::Probe(obj);
    }
    else {
        cb = NULL;
    }
    if (TO_IINTERFACE(mModelCallbacks) == TO_IINTERFACE(cb) && cb != NULL) {
        mModelCallbacks->BindComponentsRemoved(mRemovedPackageNames,
                mRemovedApps, mPermanent, mUser);
    }
    return NOERROR;
}

LauncherModel::MyRunnable20::MyRunnable20(
    /* [in] */ IWeakReference* _callbacks,
    /* [in] */ ILauncherModelCallbacks* modelCallbacks,
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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        cb = ILauncherModelCallbacks::Probe(obj);
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
    /* [in] */ LauncherModel* host,
    /* [in] */ IContext* context,
    /* [in] */ Boolean isLaunching)
    : mHost(host)
    , mContext(context)
    , mIsLaunching(isLaunching)
    , mIsLoadingAndBindingWorkspace(FALSE)
    , mStopped(FALSE)
    , mLoadAndBindStepFinished(FALSE)
{
    CHashMap::New((IHashMap**)&mLabelCache);
}

ECode LauncherModel::LoaderTask::IsLaunching(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsLaunching;
    return NOERROR;
}

ECode LauncherModel::LoaderTask::IsLoadingWorkspace(
    /* [out] */ Boolean* result)
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
        assert(0);
        //Slogger::D(TAG, "loadAndBindWorkspace mWorkspaceLoaded=" + mWorkspaceLoaded);
    }

    if (!mHost->mWorkspaceLoaded) {
        LoadWorkspace();
        synchronized(this) {
            if (mStopped) {
                return;
            }
            mHost->mWorkspaceLoaded = TRUE;
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
        mHost->mHandler->PostIdle(run);

        while (!mStopped && !mLoadAndBindStepFinished && !mHost->mFlushingWorkerThread) {
            //try {
            // Just in case mFlushingWorkerThread changes but we aren't woken up,
            // wait no longer than 1sec at a time
            Wait(1000);
            //} catch (InterruptedException ex) {
                // Ignore
            //}
        }
        if (DEBUG_LOADERS) {
            assert(0);
            // Slogger::D(TAG, "waited "
            //         + (SystemClock.uptimeMillis()-workspaceWaitTime)
            //         + "ms for previous step to finish binding");
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
        Slogger::E("LauncherModel::LoaderTask", "Should not call runBindSynchronousPage() without "
                "valid page index");
        return E_RUNTIME_EXCEPTION;
    }
    if (!mHost->mAllAppsLoaded || !mHost->mWorkspaceLoaded) {
        // Ensure that we don't try and bind a specified page when the pages have not been
        // loaded already (we should load everything asynchronously in that case)
        //throw new RuntimeException("Expecting AllApps and Workspace to be loaded");
        Slogger::E("LauncherModel::LoaderTask", "Expecting AllApps and Workspace to be loaded");
        return E_RUNTIME_EXCEPTION;
    }
    synchronized(mHost->mLock) {
        if (mHost->mIsLoaderTaskRunning) {
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
    mHost->mHandler->Flush();

    // Divide the set of loaded items into those that we are binding synchronously, and
    // everything else that is to be bound normally (asynchronously).
    BindWorkspace(synchronousBindPage);
    // XXX: For now, continue posting the binding of AllApps as there are other issues that
    //      arise from that.
    OnlyBindAllApps();
    return NOERROR;
}

ECode LauncherModel::LoaderTask::Run()
{
    synchronized(mHost->mLock) {
        mHost->mIsLoaderTaskRunning = TRUE;
    }

    keep_running: {
        // Elevate priority when Home launches for the first time to avoid
        // starving at boot time. Staring at a blank home is not cool.
        synchronized(mHost->mLock) {
            if (DEBUG_LOADERS) {
                assert(0);
                // Slogger::D(TAG, "Setting thread priority to " +
                //     (mIsLaunching ? "DEFAULT" : "BACKGROUND"));
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
            assert(0);
            //break keep_running;
        }

        // Whew! Hard work done.  Slow us down, and wait until the UI thread has
        // settled down.
        synchronized(mHost->mLock) {
            if (mIsLaunching) {
                if (DEBUG_LOADERS) {
                    Slogger::D(TAG, "Setting thread priority to BACKGROUND");
                }
                Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
            }
        }
        WaitForIdle();

        // Second step. Load all apps.
        if (DEBUG_LOADERS) {
            Slogger::D(TAG, "step 2: loading all apps");
        }
        LoadAndBindAllApps();

        // Restore the default thread priority after we are done loading items
        synchronized(mHost->mLock) {
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
            fit->GetNext((IInterface**)&key);
            AutoPtr<IInterface> obj;
            sBgDbIconCache->Get(key, (IInterface**)&obj);
            assert(0);
            // UpdateSavedIcon(mContext, (ShortcutInfo*)IShortcutInfo::Probe(key),
            //         IArrayOf::Probe(obj));
        }
        sBgDbIconCache->Clear();
    }

    // Clear out this reference, otherwise we end up holding it until all of the
    // callback runnables are done.
    mContext = NULL;

    synchronized(mHost->mLock) {
        // If we are still the last one to be scheduled, remove ourselves.
        if (TO_IINTERFACE(mHost->mLoaderTask) == TO_IINTERFACE(this)) {
            mHost->mLoaderTask = NULL;
        }
        mHost->mIsLoaderTaskRunning = FALSE;
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

    synchronized(mHost->mLock) {
        if (mStopped) {
            *newCallbacks = NULL;
            return NOERROR;
        }

        if (mHost->mCallbacks == NULL) {
            *newCallbacks = NULL;
            return NOERROR;
        }


        AutoPtr<IInterface> obj;
        mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        AutoPtr<ILauncherModelCallbacks> _callbacks =
                ILauncherModelCallbacks::Probe(obj);
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
        if (mHost->mCallbacks == NULL) {
            return FALSE;
        }

        AutoPtr<IInterface> obj;
        mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks,
                (IInterface**)&obj);
        AutoPtr<ILauncherModelCallbacks> _callbacks =
                ILauncherModelCallbacks::Probe(obj);
        Boolean res;
        _callbacks->IsAllAppsButtonRank(item->mScreen, &res);
        if (res) {
            return FALSE;
        }

        // We use the last index to refer to the hotseat and the screen as the rank, so
        // test and update the occupied state accordingly
        if ((*occupied)[ILauncher::SCREEN_COUNT][item->mScreen][0] != NULL) {
            assert(0);
            // Slogger::E(TAG, "Error loading shortcut into hotseat " + item
            //     + " into position (" + item.screen + ":" + item.cellX + "," + item.cellY
            //     + ") occupied by " + occupied[Launcher.SCREEN_COUNT][item.screen][0]);
            return FALSE;
        }
        else {
            assert(0);
            //(*((*occupied)[ILauncher::SCREEN_COUNT]))[item->mScreen]->Set(0, item);
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
                assert(0);
                // Slogger::E(TAG, "Error loading shortcut " + item
                //     + " into cell (" + containerIndex + "-" + item.screen + ":"
                //     + x + "," + y
                //     + ") occupied by "
                //     + occupied[containerIndex][x][y]);
                return FALSE;
            }
        }
    }
    for (Int32 x = item->mCellX; x < (item->mCellX+item->mSpanX); x++) {
        for (Int32 y = item->mCellY; y < (item->mCellY+item->mSpanY); y++) {
            assert(0);
            //occupied->Set(containerIndex, x, y) = item;
        }
    }

    return TRUE;
}

void LauncherModel::LoaderTask::LoadWorkspace()
{
//     Int64 t = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;

//     AutoPtr<IContext> context = mContext;
//     AutoPtr<IContentResolver> contentResolver;
//     context->GetContentResolver((IContentResolver**)&contentResolver);
//     AutoPtr<IPackageManager> manager;
//     context->GetPackageManager((IPackageManager**)&manager);

//     AutoPtr<IAppWidgetManagerHelper> helper;
//     CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
//     AutoPtr<IAppWidgetManager> widgets;
//     helper->GetInstance(context, (IAppWidgetManager**)&widgets);

//     Boolean isSafeMode;
//     manager->IsSafeMode(&isSafeMode);

//     // Make sure the default workspace is loaded, if needed
//     AutoPtr<ILauncherProvider> provider;
//     assert(0 && "need class LauncherApplication");
//     //mHost->mApp->GetLauncherProvider((ILauncherProvider**)&provider);
//     provider->LoadDefaultFavoritesIfNecessary(0, FALSE);

//     synchronized(sBgLock) {
//         sBgWorkspaceItems->Clear();
//         sBgAppWidgets->Clear();
//         sBgFolders->Clear();
//         sBgItemsIdMap->Clear();
//         sBgDbIconCache->Clear();

//         AutoPtr<IArrayList> itemsToRemove;
//         CArrayList::New((IArrayList**)&itemsToRemove);

//         AutoPtr<ICursor> c;
//         contentResolver->Query(
//                 LauncherSettings::Favorites::CONTENT_URI, NULL,
//                 String(NULL), NULL, String(NULL), (ICursor**)&c);

//         // +1 for the hotseat (it can be larger than the workspace)
//         // Load workspace in reverse order to ensure that latest items are loaded first (and
//         // before any earlier duplicates)
//         AutoPtr<ArrayOf<AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > > > occupied =
//                 ArrayOf<AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > >::Alloc(
//                 ILauncher::SCREEN_COUNT + 1);

//         for (Int32 i = 0; i < ILauncher::SCREEN_COUNT + 1; i++) {
//             AutoPtr<ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > > > array2 =
//                 ArrayOf<AutoPtr<ArrayOf<ItemInfo*> > >::Alloc(mCellCountX + 1);
//             (*occupied)[i] = array2;

//             for (Int32 j = 0; i < mCellCountX + 1; i++) {

//                 AutoPtr<ArrayOf<ItemInfo*> > array3 =
//                         ArrayOf<ItemInfo*>::Alloc(mCellCountY + 1);
//                 (*array2)[j] = array3;
//             }

//         }

//         //try {
//         Int32 idIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(IBaseColumns::ID, &idIndex), FINALLY)
//         Int32 intentIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::INTENT, &intentIndex), FINALLY)
//         Int32 titleIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::TITLE, &titleIndex), FINALLY)
//         Int32 iconTypeIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ICON_TYPE, &iconTypeIndex), FINALLY)
//         Int32 iconIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ICON, &iconIndex), FINALLY)
//         Int32 iconPackageIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ICON_PACKAGE, &iconPackageIndex), FINALLY)
//         Int32 iconResourceIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ICON_RESOURCE, &iconResourceIndex), FINALLY)
//         Int32 containerIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER, &containerIndex), FINALLY)
//         Int32 itemTypeIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE, &itemTypeIndex), FINALLY)
//         Int32 appWidgetIdIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::APPWIDGET_ID, &appWidgetIdIndex), FINALLY)
//         Int32 screenIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN, &screenIndex), FINALLY)
//         Int32 cellXIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX, &cellXIndex), FINALLY)
//         Int32 cellYIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY, &cellYIndex), FINALLY)
//         Int32 spanXIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANX, &spanXIndex), FINALLY)
//         Int32 spanYIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SPANY, &spanYIndex), FINALLY)
//         Int32 profileIdIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(LauncherSettings::Favorites::PROFILE_ID, &profileIdIndex), FINALLY)
//         //final int uriIndex = c.getColumnIndexOrThrow(LauncherSettings.Favorites.URI);
//         //final int displayModeIndex = c.getColumnIndexOrThrow(
//         //        LauncherSettings.Favorites.DISPLAY_MODE);

//         AutoPtr<ShortcutInfo> info;
//         String intentDescription;
//         AutoPtr<LauncherAppWidgetInfo> appWidgetInfo;
//         Int32 container;
//         Int64 id;
//         AutoPtr<IIntent> intent;
//         AutoPtr<IUserHandle> user;

//         Boolean res;
//         while (!mStopped && (c->MoveToNext(&res), res)) {
//             //try {
//             Int32 itemType;
//             FAIL_GOTO(c->GetInt32(itemTypeIndex, &itemType), FAILED)

//             switch (itemType) {
//             case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
//             case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
//             {
//                 FAIL_GOTO(c->GetString(intentIndex, &intentDescription), FAILED)
//                 Int32 serialNumber;
//                 FAIL_GOTO(c->GetInt32(profileIdIndex, &serialNumber), FAILED)
//                 FAIL_GOTO(mHost->mUserManager->GetUserForSerialNumber(serialNumber,
//                         (IUserHandle**)&user), FAILED)
//                 // If the user doesn't exist anymore, skip.
//                 if (user == NULL) {
//                     Int64 index;
//                     FAIL_GOTO(c->GetInt64(idIndex, &index), FAILED)
//                     AutoPtr<IInteger64> obj = CoreUtils::Convert(index);
//                     FAIL_GOTO(itemsToRemove->Add(TO_IINTERFACE(obj)), FAILED)
//                     continue;
//                 }
//                 //try {
//                 AutoPtr<IIntentHelper> helper;
//                 CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
//                 if ((ECode)E_URI_SYNTAX_EXCEPTION == helper->ParseUri(intentDescription,
//                         0, (IIntent**)&intent)) {
//                     continue;
//                 }
//                 //} catch (URISyntaxException e) {
//                     //continue;
//                 //}

//                 if (itemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION) {
//                         FAIL_GOTO(mHost->GetShortcutInfo(manager, intent, user, context, c, iconIndex,
//                             titleIndex, mLabelCache, (ShortcutInfo**)&info), FAILED)
//                 }
//                 else {
//                     info = mHost->GetShortcutInfo(c, context, iconTypeIndex,
//                             iconPackageIndex, iconResourceIndex, iconIndex, titleIndex);

//                     // App shortcuts that used to be automatically added to Launcher
//                     // didn't always have the correct intent flags set, so do that
//                     // here
//                     String action;
//                     FAIL_GOTO(intent->GetAction(&action), FAILED)
//                     if (!action.IsNull()) {
//                         AutoPtr<ArrayOf<String> > categories;
//                         FAIL_GOTO(intent->GetCategories((ArrayOf<String>**)&categories), FAILED)
//                         Boolean res;
//                         if (categories != NULL && (action.Equals(IIntent::ACTION_MAIN))) {
//                             assert(0);
//                             // if (categories->Contains(IIntent::CATEGORY_LAUNCHER)) {
//                             //     FAIL_GOTO(intent->AddFlags(
//                             //         Intent::FLAG_ACTIVITY_NEW_TASK |
//                             //         Intent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED), FAILED)
//                             // }
//                         }
//                     }
//                 }

//                 if (info != NULL) {
//                     info->mIntent = intent;
//                     FAIL_GOTO(c->GetInt64(idIndex, &(info->mId)), FAILED)
//                     FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
//                     info->mContainer = container;
//                     FAIL_GOTO(c->GetInt32(screenIndex, &(info->mScreen)), FAILED)
//                     FAIL_GOTO(c->GetInt32(cellXIndex, &(info->mCellX)), FAILED)
//                     FAIL_GOTO(c->GetInt32(cellYIndex, &(info->mCellY)), FAILED)
//                     FAIL_GOTO(info->mIntent->PutExtra(ItemInfo::EXTRA_PROFILE,
//                             IParcelable::Probe(info->mUser)), FAILED)

//                     // check & update map of what's occupied
//                     assert(0);
//                     // if (!CheckItemPlacement(occupied, (ItemInfo*)IItemInfo::Probe(info))) {
//                     //     break;
//                     // }

//                     switch (container) {
//                         case LauncherSettings::Favorites::CONTAINER_DESKTOP:
//                         case LauncherSettings::Favorites::CONTAINER_HOTSEAT:
//                             FAIL_GOTO(sBgWorkspaceItems->Add(TO_IINTERFACE(info)), FAILED)
//                             break;
//                         default:
//                         {
//                             // Item is in a user folder
//                             AutoPtr<FolderInfo> folderInfo =
//                                     FindOrMakeFolder(sBgFolders, container);
//                             FAIL_GOTO(folderInfo->Add(info), FAILED)
//                             break;
//                         }
//                     }
//                     AutoPtr<IInteger64> obj = CoreUtils::Convert(info->mId);
//                     FAIL_GOTO(sBgItemsIdMap->Put(TO_IINTERFACE(obj),
//                             TO_IINTERFACE(info)), FAILED)

//                     // now that we've loaded everthing re-save it with the
//                     // icon in case it disappears somehow.
//                     Boolean res;
//                     FAIL_GOTO(mHost->QueueIconToBeChecked(sBgDbIconCache, info, c,
//                             iconIndex, &res), FAILED)
//                 }
//                 else {
//                     // Failed to load the shortcut, probably because the
//                     // activity manager couldn't resolve it (maybe the app
//                     // was uninstalled), or the db row was somehow screwed up.
//                     // Delete it.
//                     FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)
//                     assert(0);
//                     //Slogger:E(TAG, "Error loading shortcut " + id + ", removing it");
//                     AutoPtr<IUri> uri;
//                     LauncherSettings::Favorites::GetContentUri(id, FALSE, (IUri**)&uri);
//                     Int32 tmp;
//                     FAIL_GOTO(contentResolver->Delete(uri, String(NULL), NULL, &tmp), FAILED)
//                 }
//                 break;
//             }
//             case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
//             {
//                 FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)
//                 AutoPtr<FolderInfo> folderInfo = FindOrMakeFolder(sBgFolders, id);

//                 String str;
//                 FAIL_GOTO(c->GetString(titleIndex, &str), FAILED)
//                 folderInfo->mTitle = CoreUtils::Convert(str);
//                 folderInfo->mId = id;
//                 FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
//                 folderInfo->mContainer = container;
//                 FAIL_GOTO(c->GetInt32(screenIndex, &(folderInfo->mScreen)), FAILED)
//                 FAIL_GOTO(c->GetInt32(cellXIndex, &(folderInfo->mCellX)), FAILED)
//                 FAIL_GOTO(c->GetInt32(cellYIndex, &(folderInfo->mCellY)), FAILED)

//                 // check & update map of what's occupied
//                 assert(0);
//                 // if (!CheckItemPlacement(occupied, folderInfo)) {
//                 //     break;
//                 // }
//                 switch (container) {
//                     case LauncherSettings::Favorites::CONTAINER_DESKTOP:
//                     case LauncherSettings::Favorites::CONTAINER_HOTSEAT:
//                         sBgWorkspaceItems->Add(TO_IINTERFACE(folderInfo));
//                         break;
//                 }

//                 AutoPtr<IInteger64> obj = CoreUtils::Convert(folderInfo->mId);
//                 FAIL_GOTO(sBgItemsIdMap->Put(TO_IINTERFACE(obj),
//                         TO_IINTERFACE(folderInfo)), FAILED)

//                 FAIL_GOTO(sBgFolders->Put(TO_IINTERFACE(obj),
//                         TO_IINTERFACE(folderInfo)), FAILED)
//                 break;
//             }
//             case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
//             {
//                 // Read all Launcher-specific widget details
//                 Int32 appWidgetId;
//                 FAIL_GOTO(c->GetInt32(appWidgetIdIndex, &appWidgetId), FAILED)
//                 FAIL_GOTO(c->GetInt64(idIndex, &id), FAILED)

//                 AutoPtr<IAppWidgetProviderInfo> provider;
//                 FAIL_GOTO(widgets->GetAppWidgetInfo(appWidgetId,
//                         (IAppWidgetProviderInfo**)&provider), FAILED)

//                 if (!isSafeMode) {
//                     if (provider == NULL) {
//                         assert(0);
//                         // String log = "Deleting widget that isn't installed anymore: id="
//                         //     + id + " appWidgetId=" + appWidgetId;
//                         // Slogger::E(TAG, log);
//                         assert(0);
//                         //Launcher.sDumpLogs.add(log);
//                         AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
//                         itemsToRemove->Add(TO_IINTERFACE(obj));
//                     }
//                     else {
//                         AutoPtr<IComponentName> name;
//                         provider->GetProvider((IComponentName**)&name);
//                         if (name == NULL) {
//                             assert(0);
//                             // String log = "Deleting widget that isn't installed anymore: id="
//                             //     + id + " appWidgetId=" + appWidgetId;
//                             // Log.e(TAG, log);
//                             assert(0);
//                             //Launcher.sDumpLogs.add(log);
//                             AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
//                             itemsToRemove->Add(TO_IINTERFACE(obj));
//                         }
//                         else {
//                             String packageName;
//                             name->GetPackageName(&packageName);
//                             if (packageName.IsNull()) {
//                                 assert(0);
//                                 // String log = "Deleting widget that isn't installed anymore: id="
//                                 //     + id + " appWidgetId=" + appWidgetId;
//                                 // Log.e(TAG, log);
//                                 assert(0);
//                                 //Launcher.sDumpLogs.add(log);
//                                 AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
//                                 itemsToRemove->Add(TO_IINTERFACE(obj));
//                             }
//                         }
//                     }
//                 }
//                 else {
//                     AutoPtr<IComponentName> name;
//                     FAIL_GOTO(provider->GetProvider((IComponentName**)&name), FAILED)
//                     appWidgetInfo = new LauncherAppWidgetInfo();
//                     appWidgetInfo->constructor(appWidgetId, name);
//                     appWidgetInfo->mId = id;
//                     FAIL_GOTO(c->GetInt32(screenIndex, &(appWidgetInfo->mScreen)), FAILED)
//                     FAIL_GOTO(c->GetInt32(cellXIndex, &(appWidgetInfo->mCellX)), FAILED)
//                     FAIL_GOTO(c->GetInt32(cellYIndex, &(appWidgetInfo->mCellY)), FAILED)
//                     FAIL_GOTO(c->GetInt32(spanXIndex, &(appWidgetInfo->mSpanX)), FAILED)
//                     FAIL_GOTO(c->GetInt32(spanYIndex, &(appWidgetInfo->mSpanY)), FAILED)
//                     AutoPtr<ArrayOf<Int32> > minSpan;
//                     assert(0);
//                     //Launcher::GetMinSpanForWidget(context, provider, (ArrayOf<Int32>**)&minSpan);
//                     appWidgetInfo->mMinSpanX = (*minSpan)[0];
//                     appWidgetInfo->mMinSpanY = (*minSpan)[1];

//                     FAIL_GOTO(c->GetInt32(containerIndex, &container), FAILED)
//                     if (container != LauncherSettings::Favorites::CONTAINER_DESKTOP &&
//                         container != LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
//                         Slogger::E(TAG, "Widget found where container != "
//                             "CONTAINER_DESKTOP nor CONTAINER_HOTSEAT - ignoring!");
//                         continue;
//                     }
//                     Int32 tmp;
//                     FAIL_GOTO(c->GetInt32(containerIndex, &tmp), FAILED)
//                     appWidgetInfo->mContainer = tmp;

//                     // check & update map of what's occupied
//                     assert(0);
//                     // if (!CheckItemPlacement(occupied, appWidgetInfo)) {
//                     //     break;
//                     // }
//                     AutoPtr<IInteger64> obj = CoreUtils::Convert(appWidgetInfo->mId);
//                     FAIL_GOTO(sBgItemsIdMap->Put(TO_IINTERFACE(obj), TO_IINTERFACE(appWidgetInfo)), FAILED)
//                     FAIL_GOTO(sBgAppWidgets->Add(TO_IINTERFACE(appWidgetInfo)), FAILED)
//                 }
//                 break;
//             }
//             }
//             //catch (Exception e) {
// FAILED:
//             Slogger::W(TAG, "Desktop items loading interrupted:");
//             //}
//         }
//         //} finally {
// FINALLY:
//         c->Close();
//         //}
//         Int32 size;
//         itemsToRemove->GetSize(&size);
//         if (size > 0) {
//             AutoPtr<IContentProviderClient> client;
//             contentResolver->AcquireContentProviderClient(
//                     LauncherSettings::Favorites::CONTENT_URI, (IContentProviderClient**)&client);
//             // Remove dead items
//             for (Int32 i = 0; i < size; i++) {
//                 AutoPtr<IInterface> obj;
//                 itemsToRemove->Get(i, (IInterface)&obj);
//                 AutoPtr<IInteger64> value = IInteger64::Probe(obj);
//                 Int64 id;
//                 value->GetValue(&id);

//                 if (DEBUG_LOADERS) {
//                     StringBuilder sb;
//                     sb += "Removed id = ";
//                     sb += id;
//                     Slogger::D(TAG, sb.ToString());
//                 }
//                 // Don't notify content observers
//                 //try {
//                 AutoPtr<IUri> uri;
//                 LauncherSettings::Favorites::GetContentUri(id, FALSE, (IUri**)&uri);
//                 Int32 tmp;
//                 if ((ECode)E_REMOTE_EXCEPTION == client->Delete(uri, NULL, NULL, &tmp)) {
//                     StringBuilder sb;
//                     sb += "Could not remove id = ";
//                     sb += id;
//                     Slogger::W(TAG, sb.ToString());
//                 }
//                 //} catch (RemoteException e) {
//                     //Log.w(TAG, "Could not remove id = " + id);
//                 //}
//             }
//         }

//         if (DEBUG_LOADERS) {
//             SystemClock::GetUptimeMillis()
//             StringBuilder sb;
//             sb += "loaded workspace in ";
//             sb += SystemClock::GetUptimeMillis() - t;
//             sb += "ms";
//             Slogger::D(TAG, sb.ToString());
//             Slogger::D(TAG, "workspace layout: ");
//             for (Int32 y = 0; y < mCellCountY; y++) {
//                 StringBuilder line;
//                 line += "[ ";
//                 for (Int32 s = 0; s < ILauncher::SCREEN_COUNT; s++) {
//                     if (s > 0) {
//                         line += " | ";
//                     }
//                     for (Int32 x = 0; x < mCellCountX; x++) {
//                         line += (((*occupied)[s][x][y] != NULL) ? "#" : ".");
//                     }
//                 }
//                 line += " ]";
//                 Slogger::D(TAG, line.ToString());
//             }
//         }
//     }
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
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IItemInfo> i = IItemInfo::Probe(obj);
        if (i == NULL) {
            iter->Remove();
        }
    }

    // If we aren't filtering on a screen, then the set of items to load is the full set of
    // items given.
    if (currentScreen < 0) {
        currentScreenItems->AddAll(ICollection::Probe(allWorkspaceItems));
    }

    // Order the set of items by their containers first, this allows use to walk through the
    // list sequentially, build up a list of containers that are in the specified screen,
    // as well as all items in those containers.
    AutoPtr<ISet> itemsOnScreen;
    CHashSet::New((ISet**)&itemsOnScreen);
    AutoPtr<IComparator> c = new MyComparator();
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(allWorkspaceItems), c);

    Int32 size;
    allWorkspaceItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        allWorkspaceItems->Get(i, (IInterface**)&obj);
        ItemInfo* info = (ItemInfo*)(IObject::Probe(obj));

        if (info->mContainer == LauncherSettings::Favorites::CONTAINER_DESKTOP) {
            if (info->mScreen == currentScreen) {
                currentScreenItems->Add(TO_IINTERFACE(info));
                AutoPtr<IInteger64> obj = CoreUtils::Convert(info->mId);
                itemsOnScreen->Add(TO_IINTERFACE(obj));
            }
            else {
                otherScreenItems->Add(TO_IINTERFACE(info));
            }
        }
        else if (info->mContainer == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
            currentScreenItems->Add(TO_IINTERFACE(info));
            AutoPtr<IInteger64> obj = CoreUtils::Convert(info->mId);
            itemsOnScreen->Add(TO_IINTERFACE(obj));
        }
        else {
            Boolean res;
            AutoPtr<IInteger64> obj = CoreUtils::Convert(info->mContainer);
            itemsOnScreen->Contains(TO_IINTERFACE(obj), &res);
            if (res) {
                currentScreenItems->Add(TO_IINTERFACE(info));
                AutoPtr<IInteger64> obj = CoreUtils::Convert(info->mId);
                itemsOnScreen->Add(TO_IINTERFACE(obj));
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
        currentScreenWidgets->AddAll(ICollection::Probe(appWidgets));
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
        currentScreenFolders->PutAll(IMap::Probe(folders));
    }

    AutoPtr<ISet> keySet;
    folders->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInteger64> obj = IInteger64::Probe((*array)[i]);
        Int64 id;
        obj->GetValue(&id);

        AutoPtr<IInterface> value;
        itemsIdMap->Get((*array)[i], (IInterface**)&value);
        AutoPtr<ItemInfo> info = (ItemInfo*)(IObject::Probe(value));

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
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(workspaceItems), c);
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
        Int32 chunkSize = (i + ITEMS_CHUNK <= N) ? ITEMS_CHUNK : (N-i);
        AutoPtr<IRunnable> r = new MyRunnable9(this, oldCallbacks, workspaceItems,
                start, chunkSize);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        }
        else {
            mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
    }

    // Bind the folders
    Boolean res;
    folders->IsEmpty(&res);
    if (!res) {
        AutoPtr<IRunnable> r = new MyRunnable10(this, oldCallbacks, folders);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        }
        else {
            mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
        }
    }

    // Bind the widgets, one at a time
    appWidgets->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        appWidgets->Get(i, (IInterface**)&obj);
        AutoPtr<ILauncherAppWidgetInfo> widget = ILauncherAppWidgetInfo::Probe(obj);
        AutoPtr<IRunnable> r = new MyRunnable11(this, oldCallbacks, widget);
        if (postOnMainThread) {
            deferredBindRunnables->Add(TO_IINTERFACE(r));
        }
        else {
            mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
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
    mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
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
    mHost->UnbindWorkspaceItemsOnMainThread();
    AutoPtr<IArrayList> workspaceItems;
    CArrayList::New((IArrayList**)&workspaceItems);
    AutoPtr<IArrayList> appWidgets;
    CArrayList::New((IArrayList**)&appWidgets);
    AutoPtr<IHashMap> folders;
    CHashMap::New((IHashMap**)&folders);
    AutoPtr<IHashMap> itemsIdMap;
    CHashMap::New((IHashMap**)&itemsIdMap);
    synchronized(sBgLock) {
        workspaceItems->AddAll(ICollection::Probe(sBgWorkspaceItems));
        appWidgets->AddAll(ICollection::Probe(sBgAppWidgets));
        folders->PutAll(IMap::Probe(sBgFolders));
        itemsIdMap->PutAll(IMap::Probe(sBgItemsIdMap));
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
    mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);

    // Load items on the current page
    BindWorkspaceItems(oldCallbacks, currentWorkspaceItems, currentAppWidgets,
            currentFolders, NULL);
    if (isLoadingSynchronously) {
        r = new MyRunnable13(this, oldCallbacks, currentScreen);
        mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
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
        mHost->RunOnMainThread(r, MAIN_THREAD_BINDING_RUNNABLE);
    }
}

void LauncherModel::LoaderTask::LoadAndBindAllApps()
{
    if (DEBUG_LOADERS) {
        assert(0);
        //Slogger::D(TAG, "loadAndBindAllApps mAllAppsLoaded=" + mHost->mAllAppsLoaded);
    }
    if (!mHost->mAllAppsLoaded) {
        LoadAllAppsByBatch();
        synchronized(this) {
            if (mStopped) {
                return;
            }
            mHost->mAllAppsLoaded = TRUE;
        }
    }
    else {
        OnlyBindAllApps();
    }
}

void LauncherModel::LoaderTask::OnlyBindAllApps()
{
    AutoPtr<IInterface> obj;
    mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
    AutoPtr<ILauncherModelCallbacks> oldCallbacks = ILauncherModelCallbacks::Probe(obj);
    if (oldCallbacks == NULL) {
        // This launcher has exited and nobody bothered to tell us.  Just bail.
        Slogger::W(TAG, "LoaderTask running with no launcher (onlyBindAllApps)");
        return;
    }

    // shallow copy
    //@SuppressWarnings("unchecked")
    AutoPtr<IInterface> obj2;
    ICloneable::Probe(mHost->mBgAllAppsList->mData)->Clone((IInterface**)&obj2);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj2);
    AutoPtr<IRunnable> r = new MyRunnable15(this, oldCallbacks, list) ;
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    Boolean isRunningOnMainThread = !(threadId == Process::MyTid());
    Boolean res;
    oldCallbacks->IsAllAppsVisible(&res);
    if (res && isRunningOnMainThread) {
        r->Run();
    }
    else {
        mHost->mHandler->Post(r);
    }
}

void LauncherModel::LoaderTask::LoadAllAppsByBatch()
{
    Int64 t = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;

    // Don't use these two variables in any of the callback runnables.
    // Otherwise we hold a reference to them.
    AutoPtr<IInterface> obj;
    mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
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
    mHost->mUserManager->GetUserProfiles((IList**)&profiles);

    mHost->mBgAllAppsList->Clear();
    Int32 profileCount;
    profiles->GetSize(&profileCount);
    for (Int32 p = 0; p < profileCount; p++) {
        AutoPtr<IInterface> obj;
        profiles->Get(p, (IInterface**)&obj);
        AutoPtr<IUserHandle> user = IUserHandle::Probe(obj);
        AutoPtr<IList> apps;
        Int32 N = Elastos::Core::Math::INT32_MAX_VALUE;

        Int32 startIndex;
        Int32 i = 0;
        Int32 batchSize = -1;
        while (i < N && !mStopped) {
            if (i == 0) {
                Int64 qiaTime = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;
                mHost->mLauncherApps->GetActivityList(String(NULL), user, (IList**)&apps);
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "queryIntentActivities took ";
                    sb += SystemClock::GetUptimeMillis() - qiaTime;
                    sb += "ms";
                    Slogger::D(TAG, sb.ToString());
                }
                if (apps == NULL) {
                    return;
                }
                apps->GetSize(&N);
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "queryIntentActivities got ";
                    sb += N;
                    sb += " apps";
                    Slogger::D(TAG, sb.ToString());
                }
                if (N == 0) {
                    // There are no apps?!?
                    return;
                }
                if (mHost->mBatchSize == 0) {
                    batchSize = N;
                }
                else {
                    batchSize = mHost->mBatchSize;
                }

                Int64 sortTime = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;
                AutoPtr<IComparator> c = new ShortcutNameComparator(mLabelCache);
                AutoPtr<ICollections> collections;
                CCollections::AcquireSingleton((ICollections**)&collections);
                collections->Sort(apps, c);
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "sort took ";
                    sb += SystemClock::GetUptimeMillis() - sortTime;
                    sb += "ms";
                    Slogger::D(TAG, sb.ToString());
                }
            }

            Int64 t2 = DEBUG_LOADERS ? SystemClock::GetUptimeMillis() : 0;

            startIndex = i;
            for (Int32 j=0; i<N && j<batchSize; j++) {
                // This builds the icon bitmaps.
                AutoPtr<IInterface> obj;
                apps->Get(i, (IInterface**)&obj);
                AutoPtr<ILauncherActivityInfo> _info = ILauncherActivityInfo::Probe(obj);
                AutoPtr<ApplicationInfo> info = new ApplicationInfo();
                info->constructor(_info, user, mHost->mIconCache, mLabelCache);
                mHost->mBgAllAppsList->Add(info);
                i++;
            }

            AutoPtr<ILauncherModelCallbacks> callbacks;
            TryGetCallbacks(oldCallbacks, (ILauncherModelCallbacks**)&callbacks);
            AutoPtr<IArrayList> added = mHost->mBgAllAppsList->mAdded;
            Boolean firstProfile = p == 0;
            CArrayList::New((IArrayList**)&(mHost->mBgAllAppsList->mAdded));
            AutoPtr<IRunnable> r = new MyRunnable16(callbacks, firstProfile, added);
            mHost->mHandler->Post(r);

            if (DEBUG_LOADERS) {
                StringBuilder sb;
                sb += "batch of ";
                sb += i - startIndex;
                sb += " icons processed in ";
                sb += SystemClock::GetUptimeMillis() - t2;
                sb += "ms";
                Slogger::D(TAG, sb.ToString());
            }

            if (mHost->mAllAppsLoadDelay > 0 && i < N) {
                //try {
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "sleeping for ";
                    sb += mHost->mAllAppsLoadDelay;
                    sb += " icons processed in ";
                    Slogger::D(TAG, sb.ToString());
                }
                Thread::Sleep(mHost->mAllAppsLoadDelay);
                //} catch (InterruptedException exc) { }
            }
        }

        if (DEBUG_LOADERS) {
            StringBuilder sb;
            sb += "cached all ";
            sb += N;
            sb += " apps in ";
            sb += SystemClock::GetUptimeMillis() - t;
            sb += "ms";
            sb += mHost->mAllAppsLoadDelay > 0 ? " (including delay)" : "";
            Slogger::D(TAG, sb.ToString());
        }
    }
}

ECode LauncherModel::LoaderTask::DumpState()
{
    assert(0);
    // synchronized(sBgLock) {
    //     Slogger::D(TAG, "mLoaderTask.mContext=" + mContext);
    //     Slogger::D(TAG, "mLoaderTask.mIsLaunching=" + mIsLaunching);
    //     Slogger::D(TAG, "mLoaderTask.mStopped=" + mStopped);
    //     Slogger::D(TAG, "mLoaderTask.mLoadAndBindStepFinished=" + mLoadAndBindStepFinished);
    //     Slogger::D(TAG, "mItems size=" + sBgWorkspaceItems.size());
    // }
    return NOERROR;
}

const Int32 LauncherModel::PackageUpdatedTask::OP_NONE = 0;
const Int32 LauncherModel::PackageUpdatedTask::OP_ADD = 1;
const Int32 LauncherModel::PackageUpdatedTask::OP_UPDATE = 2;
const Int32 LauncherModel::PackageUpdatedTask::OP_REMOVE = 3; // uninstlled
const Int32 LauncherModel::PackageUpdatedTask::OP_UNAVAILABLE = 4; // external media unmounted

CAR_INTERFACE_IMPL(LauncherModel::PackageUpdatedTask, Object, IRunnable);

LauncherModel::PackageUpdatedTask::PackageUpdatedTask(
    /* [in] */ LauncherModel* host,
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ IUserHandle* user)
    : mHost(host)
    , mOp(op)
    , mPackages(packages)
    , mUser(user)
{
}

ECode LauncherModel::PackageUpdatedTask::Run()
{
    AutoPtr<IContext> context = IContext::Probe(mHost->mApp);

    AutoPtr<ArrayOf<String> > packages = mPackages;
    Int32 N = packages->GetLength();
    switch (mOp) {
        case OP_ADD:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "mAllAppsList.addPackage ";
                    sb += (*packages)[i];
                    Slogger::D(TAG, sb.ToString());
                }
                mHost->mBgAllAppsList->AddPackage(context, (*packages)[i], mUser);
            }
            break;
        }
        case OP_UPDATE:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "mAllAppsList.updatePackage ";
                    sb += (*packages)[i];
                    Slogger::D(TAG, sb.ToString());
                }
                mHost->mBgAllAppsList->UpdatePackage(context, (*packages)[i], mUser);
                AutoPtr<IContext> ctx;
                context->GetApplicationContext((IContext**)&ctx);
                AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
                assert(0);
                // AutoPtr<WidgetPreviewLoader::CacheDb> db;
                // app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db);
                // WidgetPreviewLoader->RemoveFromDb(db, (*packages)[i]);
            }
            break;
        }
        case OP_REMOVE:
        case OP_UNAVAILABLE:
        {
            for (Int32 i = 0; i < N; i++) {
                if (DEBUG_LOADERS) {
                    StringBuilder sb;
                    sb += "mAllAppsList.removePackage ";
                    sb += (*packages)[i];
                    Slogger::D(TAG, sb.ToString());
                }
                mHost->mBgAllAppsList->RemovePackage((*packages)[i], mUser);
                AutoPtr<IContext> ctx;
                context->GetApplicationContext((IContext**)&ctx);
                AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
                assert(0);
                // AutoPtr<WidgetPreviewLoader::CacheDb> db;
                // app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db);
                // WidgetPreviewLoader->RemoveFromDb(db, (*packages)[i]);
            }
            break;
        }
    }

    AutoPtr<IArrayList> added;
    AutoPtr<IArrayList> modified;
    AutoPtr<IArrayList> removedApps;
    CArrayList::New((IArrayList**)&removedApps);

    Int32 size;
    mHost->mBgAllAppsList->mAdded->GetSize(&size);
    if (size > 0) {
        CArrayList::New(ICollection::Probe(mHost->mBgAllAppsList->mAdded), (IArrayList**)&added);
        mHost->mBgAllAppsList->mAdded->Clear();
    }
    mHost->mBgAllAppsList->mModified->GetSize(&size);
    if (size > 0) {
        CArrayList::New(ICollection::Probe(mHost->mBgAllAppsList->mModified), (IArrayList**)&modified);
        mHost->mBgAllAppsList->mModified->Clear();
    }
    mHost->mBgAllAppsList->mRemoved->GetSize(&size);
    if (size > 0) {
        removedApps->AddAll(ICollection::Probe(mHost->mBgAllAppsList->mRemoved));
        mHost->mBgAllAppsList->mRemoved->Clear();
    }

    AutoPtr<ILauncherModelCallbacks> callbacks;
    if (mHost->mCallbacks != NULL) {
        AutoPtr<IInterface> obj;
        mHost->mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
        callbacks = ILauncherModelCallbacks::Probe(obj);
    }
    else {
        callbacks = NULL;
    }
    if (callbacks == NULL) {
        Slogger::W(TAG, "Nobody to tell about the new app.  Launcher is probably loading.");
        return NOERROR;
    }

    if (added != NULL) {
        AutoPtr<IArrayList> addedFinal = added;
        AutoPtr<IRunnable> r = new MyRunnable17(mHost->mCallbacks, callbacks, addedFinal);
        mHost->mHandler->Post(r);
    }
    if (modified != NULL) {
        AutoPtr<IArrayList> modifiedFinal = modified;
        AutoPtr<IRunnable> r = new MyRunnable18(mHost->mCallbacks, callbacks, modifiedFinal);
        mHost->mHandler->Post(r);
    }
    // If a package has been removed, or an app has been removed as a result of
    // an update (for example), make the removed callback.
    Boolean res;
    removedApps->IsEmpty(&res);
    if (mOp == OP_REMOVE || !res) {
        Boolean permanent = (mOp == OP_REMOVE);
        AutoPtr<IArrayList> removedPackageNames;
        assert(0);
        //CArrayList::New(Arrays::AsList(packages), (IArrayList**)&removedPackageNames);

        AutoPtr<IRunnable> r = new MyRunnable19(mHost->mCallbacks, callbacks, removedPackageNames,
                removedApps, permanent, mUser);
        mHost->mHandler->Post(r);
    }

    AutoPtr<IArrayList> widgetsAndShortcuts;
    GetSortedWidgetsAndShortcuts(context, (IArrayList**)&widgetsAndShortcuts);
    AutoPtr<IRunnable> r = new MyRunnable20(mHost->mCallbacks, callbacks, widgetsAndShortcuts);
    return mHost->mHandler->Post(r);
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
    (IObject::Probe(_a->mUser))->Equals(_b->mUser, &res);
    if (res) {
        String astr;
        _a->mTitle->ToString(&astr);
        String bstr;
        _b->mTitle->ToString(&bstr);
        Int32 result;
        mCollator->Compare(astr, bstr, &result);
        if (result == 0) {
            (IComparable::Probe(_a->mComponentName))->CompareTo(_b->mComponentName, &result);
        }
        *outresult = result;
        return NOERROR;
    }
    else {
        // TODO: Order this based on profile type rather than string compares.
        String astr;
        _a->mUser->ToString(&astr);
        String bstr;
        _b->mUser->ToString(&bstr);
        *outresult = astr.Compare(bstr);
        return NOERROR;
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

    AutoPtr<IAppWidgetProviderInfo> _a = IAppWidgetProviderInfo::Probe(a);
    AutoPtr<IAppWidgetProviderInfo> _b = IAppWidgetProviderInfo::Probe(b);

    String astr;
    _a->GetLabel(&astr);
    AutoPtr<ICharSequence> objAstr = CoreUtils::Convert(astr);
    String bstr;
    _b->GetLabel(&bstr);
    AutoPtr<ICharSequence> objBstr = CoreUtils::Convert(bstr);
    return mCollator->Compare(TO_IINTERFACE(objAstr), TO_IINTERFACE(objBstr), result);
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
    AutoPtr<IComparator> c = new LauncherModel::MyComparator5();
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

AutoPtr<IHandlerThread> LauncherModel::sWorkerThread;
AutoPtr<IHandler> LauncherModel::sWorker;

AutoPtr<IArrayList> LauncherModel::mDeferredBindRunnables;

AutoPtr<IHashMap> LauncherModel::sBgItemsIdMap;
AutoPtr<IArrayList> LauncherModel::sBgWorkspaceItems;
AutoPtr<IArrayList> LauncherModel::sBgAppWidgets;
AutoPtr<IHashMap> LauncherModel::sBgFolders;
AutoPtr<IHashMap> LauncherModel::sBgDbIconCache;

Boolean LauncherModel::InitStaticBlock()
{
    AutoPtr<IHandlerThread> sWorkerThread;
    CHandlerThread::New(String("launcher-loader"), (IHandlerThread**)&sWorkerThread);
    IThread::Probe(sWorkerThread)->Start();

    AutoPtr<ILooper> looper;
    sWorkerThread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> sWorker;
    CHandler::New(looper, (IHandler**)&sWorker);

    CArrayList::New((IArrayList**)&mDeferredBindRunnables);

    CHashMap::New((IHashMap**)&sBgItemsIdMap);
    CArrayList::New((IArrayList**)&sBgWorkspaceItems);
    CArrayList::New((IArrayList**)&sBgAppWidgets);
    CHashMap::New((IHashMap**)&sBgFolders);
    CHashMap::New((IHashMap**)&sBgDbIconCache);
    return TRUE;
}

Boolean LauncherModel::initStaticBlock = InitStaticBlock();

Object LauncherModel::sBgLock;

Int32 LauncherModel::mCellCountX;
Int32 LauncherModel::mCellCountY;

CAR_INTERFACE_IMPL(LauncherModel, BroadcastReceiver, ILauncherModel);

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
    mHandler = new DeferredHandler();
}

ECode LauncherModel::constructor()
{
    return NOERROR;
}

ECode LauncherModel::constructor(
    /* [in] */ ILauncherApplication* app,
    /* [in] */ IconCache* iconCache)
{
    AutoPtr<IEnvironment> en;
    CEnvironment::AcquireSingleton((IEnvironment**)&en);
    en->IsExternalStorageRemovable(&mAppsCanBeOnRemoveableStorage);

    mApp = app;
    mBgAllAppsList = new AllAppsList(iconCache);
    mIconCache = iconCache;

    AutoPtr<IBitmap> icon;
    assert(0);
    //mIconCache->GetFullResDefaultActivityIcon();
    mDefaultIcon = Utilities::CreateIconBitmap(icon, IContext::Probe(app));
    AutoPtr<IResources> res;
    IContext::Probe(app)->GetResources((IResources**)&res);
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
    IContext::Probe(app)->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    mLauncherApps = ILauncherApps::Probe(obj);
    AutoPtr<IInterface> obj2;
    IContext::Probe(app)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj2);
    mUserManager = IUserManager::Probe(obj2);
    mLauncherAppsCallback = new LauncherModel::LauncherAppsCallback(this);
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
    if (threadId == Process::MyTid()) {
        // If we are on the worker thread, post onto the main handler
        mHandler->Post(r);
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
    if (threadId == Process::MyTid()) {
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

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    return helper->CreateBitmap(mDefaultIcon, bitmap);
}

ECode LauncherModel::UnbindItemInfosAndClearQueuedBindRunnables()
{
    Int32 threadId;
    sWorkerThread->GetThreadId(&threadId);
    if (threadId == Process::MyTid()) {
        // throw new RuntimeException("Expected unbindLauncherItemInfos() to be called from the " +
        //         "main thread");
        Slogger::E(TAG, "Expected unbindLauncherItemInfos() to be called from the "
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
        tmpWorkspaceItems->AddAll(ICollection::Probe(sBgWorkspaceItems));
        tmpAppWidgets->AddAll(ICollection::Probe(sBgAppWidgets));
    }
    AutoPtr<IRunnable> r = new MyRunnable(tmpWorkspaceItems, tmpAppWidgets);
    RunOnMainThread(r);
    return NOERROR;
}

ECode LauncherModel::AddOrMoveItemInDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    ItemInfo* _item = (ItemInfo*)item;
    if (_item->mContainer == IItemInfo::NO_ID) {
        // From all apps
        return AddItemToDatabase(context, _item, container, screen, cellX, cellY, FALSE);
    }
    else {
        // From somewhere else
        return MoveItemInDatabase(context, _item, container, screen, cellX, cellY);
    }
    return NOERROR;
}

ECode LauncherModel::CheckItemInfoLocked(
    /* [in] */ Int64 itemId,
    /* [in] */ IItemInfo* item)
{
    AutoPtr<IInterface> obj;
    AutoPtr<IInteger64> objId = CoreUtils::Convert(itemId);
    sBgItemsIdMap->Get(TO_IINTERFACE(objId), (IInterface**)&obj);
    AutoPtr<IItemInfo> modelItem = IItemInfo::Probe(obj);
    if (modelItem != NULL && TO_IINTERFACE(item) != TO_IINTERFACE(modelItem)) {
        // check all the data is consistent
        if (IShortcutInfo::Probe(modelItem) != NULL &&
                IShortcutInfo::Probe(item) != NULL) {
            AutoPtr<IShortcutInfo> modelShortcut = IShortcutInfo::Probe(modelItem);
            ShortcutInfo* _modelShortcut = (ShortcutInfo*)modelShortcut.Get();
            AutoPtr<IShortcutInfo> shortcut = IShortcutInfo::Probe(item);
            ShortcutInfo* _shortcut = (ShortcutInfo*)shortcut.Get();

            String str;
            _modelShortcut->mTitle->ToString(&str);
            String str2;
            _shortcut->mTitle->ToString(&str2);
            Boolean res;
            _modelShortcut->mIntent->FilterEquals(_shortcut->mIntent, &res);
            if (str.Equals(str2) && res &&
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
        StringBuilder msg;
        msg += "item: ";
        msg += item != NULL ? TO_STR(item) : "null";
        msg += "modelItem: ";
        msg += modelItem != NULL ? TO_STR(modelItem) : "null";
        msg += "Error: ItemInfo passed to checkItemInfo doesn't match original";
        // RuntimeException e = new RuntimeException(msg);
        // if (stackTrace != null) {
        //     e.setStackTrace(stackTrace);
        // }
        // throw e;
        Slogger::E(TAG, msg.ToString());
        return E_RUNTIME_EXCEPTION;
    }
}

ECode LauncherModel::CheckItemInfo(
    /* [in] */ ItemInfo* item)
{
    Int64 itemId = item->mId;
    AutoPtr<IRunnable> r = new MyRunnable2(itemId, item);
    RunOnWorkerThread(r);
    return NOERROR;
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

    AutoPtr<IRunnable> r = new MyRunnable3(cr, uri, values, itemId, item);
    RunOnWorkerThread(r);
    return NOERROR;
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
            assert(0);
            ECode ec;// = waiter->Wait();
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
    assert(0);
    // String transaction = "DbDebug    Modify item (" + item.title + ") in db, id: " + item.id +
    //         " (" + item.container + ", " + item.screen + ", " + item.cellX + ", " + item.cellY +
    //         ") --> " + "(" + container + ", " + screen + ", " + cellX + ", " + cellY + ")";
    // Launcher::sDumpLogs->Add(transaction);
    // Slogger::D(TAG, transaction);
    item->mContainer = container;
    item->mCellX = cellX;
    item->mCellY = cellY;

    // We store hotseat items in canonical form which is this orientation invariant position
    // in the hotseat
    assert(0);
    // if (ILauncher::Probe(context) != NULL && screen < 0 &&
    //         container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
    //     AutoPtr<IHotseat> hotseat;
    //     ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
    //     hotseat->GetOrderInHotseat(cellX, cellY, &(item->mScreen));
    // }
    // else {
    //     item->mScreen = screen;
    // }

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
    assert(0);
    // String transaction = "DbDebug    Modify item (" + item.title + ") in db, id: " + item.id +
    //         " (" + item.container + ", " + item.screen + ", " + item.cellX + ", " + item.cellY +
    //         ") --> " + "(" + container + ", " + screen + ", " + cellX + ", " + cellY + ")";
    // Launcher::sDumpLogs->Add(transaction);
    // Slogger::D(TAG, transaction);
    item->mCellX = cellX;
    item->mCellY = cellY;
    item->mSpanX = spanX;
    item->mSpanY = spanY;

    // We store hotseat items in canonical form which is this orientation invariant position
    // in the hotseat
    assert(0);
    // if (ILauncher::Probe(context) != NULL && screen < 0 &&
    //         container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
    //     AutoPtr<IHotseat> hotseat;
    //     ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
    //     hotseat->GetOrderInHotseat(cellX, cellY, &(item->mScreen));
    // }
    // else {
    //     item->mScreen = screen;
    // }

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
    (*args1)[0] = String("title");
    (*args1)[1] = String("intent");
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > args2 = ArrayOf<String>::Alloc(2);
    (*args2)[0] = title;
    String str;
    intent->ToUri(0, &str);
    (*args2)[1] = str;
    cr->Query(LauncherSettings::Favorites::CONTENT_URI,
        args1, String("title=? and intent=?"),
        args2, String(NULL), (ICursor**)&c);
    //try {
    c->MoveToFirst(result);
    //} finally {
    ICloseable::Probe(c)->Close();
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
            String(NULL), NULL, String(NULL), (ICursor**)&c);

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
        ItemInfo* _item = (ItemInfo*)item.Get();
        FAIL_GOTO(c->GetInt32(cellXIndex, &(_item->mCellX)), EXIT)
        FAIL_GOTO(c->GetInt32(cellYIndex, &(_item->mCellY)), EXIT)
        FAIL_GOTO(c->GetInt32(spanXIndex, &(_item->mSpanX)), EXIT)
        FAIL_GOTO(c->GetInt32(spanYIndex, &(_item->mSpanY)), EXIT)
        Int32 tmp;
        FAIL_GOTO(c->GetInt32(containerIndex, &tmp), EXIT)
        _item->mContainer = tmp;
        FAIL_GOTO(c->GetInt32(itemTypeIndex, &(_item->mItemType)), EXIT)
        FAIL_GOTO(c->GetInt32(screenIndex, &(_item->mScreen)), EXIT)
        Int32 serialNumber;
        FAIL_GOTO(c->GetInt32(profileIdIndex, &serialNumber), EXIT)
        FAIL_GOTO(um->GetUserForSerialNumber(serialNumber,
                (IUserHandle**)&(_item->mUser)), EXIT)
        // If the user no longer exists, skip this item
        if (_item->mUser != NULL) {
            FAIL_GOTO(items->Add(TO_IINTERFACE(item)), EXIT)
        }
    }
    //} catch (Exception e) {
EXIT:
    items->Clear();
    //} finally {
    ICloseable::Probe(c)->Close();
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
    (*args)[0] = StringUtils::ToString(id);
    (*args)[1] = StringUtils::ToString(LauncherSettings::Favorites::ITEM_TYPE_FOLDER);
    AutoPtr<ICursor> c;
    cr->Query(LauncherSettings::Favorites::CONTENT_URI, NULL,
            String("_id=? and (itemType=? or itemType=?)"),
            args, String(NULL), (ICursor**)&c);

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
                folderInfo = FindOrMakeFolder(folderList, id);
                break;
        }

        FolderInfo* _folderInfo = (FolderInfo*)folderInfo.Get();
        String str;
        FAIL_GOTO(c->GetString(titleIndex, &str), EXIT)
        _folderInfo->mTitle = CoreUtils::Convert(str);
        _folderInfo->mId = id;
        Int32 tmp;
        FAIL_GOTO(c->GetInt32(containerIndex, &tmp), EXIT)
        _folderInfo->mContainer = tmp;
        FAIL_GOTO(c->GetInt32(screenIndex, &(_folderInfo->mScreen)), EXIT)
        FAIL_GOTO(c->GetInt32(cellXIndex, &(_folderInfo->mCellX)), EXIT)
        FAIL_GOTO(c->GetInt32(cellYIndex, &(_folderInfo->mCellY)), EXIT)

        *info = folderInfo;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    //} finally {
EXIT:
    ICloseable::Probe(c)->Close();
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
        assert(0);
        //ILauncher::Probe(context)->GetHotseat((IHotseat**)&hotseat);
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
    assert(0);
    //app->GetLauncherProvider((ILauncherProvider**)&provider);
    provider->GenerateNewId(&(item->mId));
    values->Put(IBaseColumns::ID, item->mId);
    item->UpdateValuesWithCoordinates(values, item->mCellX, item->mCellY);

    //final StackTraceElement[] stackTrace = new Throwable().getStackTrace();
    AutoPtr<IRunnable> r = new MyRunnable5(cr, notify, values, item);
    RunOnWorkerThread(r);
    return NOERROR;
}

ECode LauncherModel::GetCellLayoutChildId(
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 localCellX,
    /* [in] */ Int32 localCellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = (((Int32)container & 0xFF) << 24)
            | (screen & 0xFF) << 16 | (localCellX & 0xFF) << 8 | (localCellY & 0xFF);
    return NOERROR;
}

ECode LauncherModel::GetCellCountX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);

    *x = mCellCountX;
    return NOERROR;
}

ECode LauncherModel::GetCellCountY(
    /* [out] */ Int32* y)
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
    AutoPtr<IUri> uriToDelete;
    LauncherSettings::Favorites::GetContentUri(item->mId, FALSE, (IUri**)&uriToDelete);

    AutoPtr<IRunnable> r = new MyRunnable6(cr, uriToDelete, item);
    RunOnWorkerThread(r);
    return NOERROR;
}

ECode LauncherModel::DeleteFolderContentsFromDatabase(
    /* [in] */ IContext* context,
    /* [in] */ FolderInfo* info)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IRunnable> r = new MyRunnable7(cr, info);
    RunOnWorkerThread(r);
    return NOERROR;
}

// ECode LauncherModel::Initialize(
//     /* [in] */ ILauncherModelCallback* _callbacks)
// {
//     assert(0);
//     // synchronized(mLock) {
//     //     mCallbacks = IWeakReference::Probe(_callbacks);
//     // }
//     return NOERROR;
// }

ECode LauncherModel::GetLauncherAppsCallback(
    /* [out] */ ILauncherAppsCallback** _callbacks)
{
    assert(0 && "LauncherModel::Initialize");
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
        StringBuilder sb;
        sb += "onReceiTO_STR(intent)ve intent=";
        sb += TO_STR(intent);
        Slogger::D(TAG, sb.ToString());
    }

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_LOCALE_CHANGED.Equals(action)) {
        // If we have changed locale we need to clear out the labels in all apps/workspace.
        ForceReload();
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        // Check if configuration change was an mcc/mnc change which would affect app resources
        // and we would need to clear out the labels in all apps/workspace. Same handling as
        // above for ACTION_LOCALE_CHANGED
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> currentConfig;
        resources->GetConfiguration((IConfiguration**)&currentConfig);
        Int32 mcc;
        currentConfig->GetMcc(&mcc);
        if (mPreviousConfigMcc != mcc) {
            StringBuilder sb;
            sb += "Reload apps on config change. curr_mcc:";
            sb += mcc;
            sb += " prevmcc:";
            sb += mPreviousConfigMcc;
            Slogger::D(TAG, sb.ToString());
            ForceReload();
        }
        // Update previousConfig
        mPreviousConfigMcc = mcc;
    }
    else if ((ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED.Equals(action))
                || (ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED.Equals(action))) {
        if (mCallbacks != NULL) {
            AutoPtr<IInterface> obj;
            mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
            AutoPtr<ILauncherModelCallbacks> _callbacks = ILauncherModelCallbacks::Probe(obj);

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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
        AutoPtr<ILauncherModelCallbacks> _callbacks = ILauncherModelCallbacks::Probe(obj);
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
        AutoPtr<IInterface> obj;
        mCallbacks->Resolve(EIID_ILauncherModelCallbacks, (IInterface**)&obj);
        AutoPtr<ILauncherModelCallbacks> _callbacks = ILauncherModelCallbacks::Probe(obj);
        if (mCallbacks != NULL && _callbacks != NULL) {
            // If there is already one running, tell it to stop.
            // also, don't downgrade isLaunching if we're already running
            isLaunching = isLaunching || StopLoaderLocked();
            mLoaderTask = new LoaderTask(this, IContext::Probe(mApp), isLaunching);
            if (synchronousBindPage > -1 && mAllAppsLoaded && mWorkspaceLoaded) {
                mLoaderTask->RunBindSynchronousPage(synchronousBindPage);
            }
            else {
                IThread::Probe(sWorkerThread)->SetPriority(IThread::NORM_PRIORITY);
                Boolean res;
                sWorker->Post(mLoaderTask, &res);
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
            return mLoaderTask->IsLoadingWorkspace(result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LauncherModel::EnqueuePackageUpdated(
    /* [in] */ PackageUpdatedTask* task)
{
    Boolean res;
    return sWorker->Post(task, &res);
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
        widgetsAndShortcuts->AddAll(ICollection::Probe(providers));
    }

    // Add all shortcuts for the user.
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IIntent> shortcutsIntent;
    CIntent::New(IIntent::ACTION_CREATE_SHORTCUT, (IIntent**)&shortcutsIntent);

    AutoPtr<IList> activities;
    packageManager->QueryIntentActivities(shortcutsIntent, 0, (IList**)&activities);
    widgetsAndShortcuts->AddAll(ICollection::Probe(activities));

    AutoPtr<IComparator> c = new WidgetAndShortcutNameComparator(packageManager);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(widgetsAndShortcuts), c);

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

    icon = mIconCache->GetIcon(componentName, lai, labelCache);
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

    AutoPtr<IPackageManager> _manager;
    assert(0);
    //mApp->GetPackageManager((IPackageManager**)&_manager);
    _manager->GetUserBadgedLabel(info->mTitle, user, (ICharSequence**)&(info->mContentDescription));
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
                shortcut->mIntent->ToUri(0, &uri);
                String uri2;
                intent->ToUri(0, &uri2);
                if (uri.Equals(uri2)) {
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
    assert(0);
    //mApp->GetPackageManager((IPackageManager**)&manager);
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
                    resources->GetIdentifier(resourceName, String(NULL), String(NULL), &id);
                    AutoPtr<IUserHandle> userH;
                    Process::MyUserHandle((IUserHandle**)&userH);
                    AutoPtr<IDrawable> drawable = mIconCache->GetFullResIcon(resources,
                            id, userH);
                    icon = Utilities::CreateIconBitmap(drawable, context);
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
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bitmap;
    if(FAILED(factory->DecodeByteArray(data, 0, data->GetLength(), (IBitmap**)&bitmap))) {
        *map = NULL;
        return NOERROR;
    }

    AutoPtr<IBitmap> _map = Utilities::CreateIconBitmap(bitmap, context);
    *map = _map;
    REFCOUNT_ADD(*map);
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

    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> manager;
    helper->GetInstance(context, (IAppWidgetManager**)&manager);

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
        IObject::Probe(name)->Equals(component, &res);
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
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> manager;
    helper->GetInstance(context, (IAppWidgetManager**)&manager);

    AutoPtr<IList> widgets;
    manager->GetInstalledProviders((IList**)&widgets);
    AutoPtr<IHashMap> configurationComponentToWidget;
    CHashMap::New((IHashMap**)&configurationComponentToWidget);

    Int32 size;
    widgets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        widgets->Get(i, (IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);

        AutoPtr<IComponentName> name;
        info->GetConfigure((IComponentName**)&name);
        configurationComponentToWidget->Put(TO_IINTERFACE(name), TO_IINTERFACE(info));
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
        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        String pname;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        AutoPtr<IComponentName> infoComponent;
        CComponentName::New(pname, name, (IComponentName**)&infoComponent);
        Boolean res;
        configurationComponentToWidget->ContainsKey(infoComponent, &res);
        if (res) {
            AutoPtr<IInterface> obj;
            configurationComponentToWidget->Get(TO_IINTERFACE(infoComponent), (IInterface**)&obj);
            AutoPtr<IAppWidgetProviderInfo> winfo = IAppWidgetProviderInfo::Probe(obj);
            AutoPtr<InstallWidgetReceiver::WidgetMimeTypeHandlerData> data =
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
    AutoPtr<IIntentShortcutIconResource> iconResource;

    if (bitmap != NULL && IBitmap::Probe(bitmap) != NULL) {
        AutoPtr<FastBitmapDrawable> tmp = new FastBitmapDrawable();
        tmp->constructor(IBitmap::Probe(bitmap));
        AutoPtr<IDrawable> drawable = IDrawable::Probe(tmp);

        icon = Utilities::CreateIconBitmap(drawable, context);
        customIcon = TRUE;
    }
    else {
        AutoPtr<IParcelable> extra;
        data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_ICON_RESOURCE, (IParcelable**)&extra);
        assert(0);
    //     if (extra != NULL && IIntentShortcutIconResource::Probe(extra) != NULL) {
    //         //try {
    //         iconResource = IIntentShortcutIconResource::Probe(extra);
    //         AutoPtr<IPackageManager> packageManager;
    //         FAIL_GOTO(context->GetPackageManager((IPackageManager**)&packageManager), ERROR)
    //         String packageName;
    //         iconResource->GetPackageName(&packageName);
    //         AutoPtr<IResources> resources;
    //         FAIL_GOTO(packageManager->GetResourcesForApplication(packageName,
    //                 (IResources**)&resources), ERROR)

    //         String resourceName;
    //         iconResource->GetResourceName(&resourceName);
    //         Int32 id;
    //         resources->GetIdentifier(resourceName, String(NULL), String(NULL), &id);
    //         AutoPtr<IDrawable> drawable;
    //         drawable = mIconCache->GetFullResIcon(resources, id, user);
    //         icon = Utilities::CreateIconBitmap(drawable, context);
    //         //} catch (Exception e) {
    // ERROR:
    //             Slogger::W(TAG, "Could not load shortcut icon: " + extra);
    //         //}
    //     }
    }

    AutoPtr<ShortcutInfo> info = new ShortcutInfo();

    if (icon == NULL) {
        if (fallbackIcon != NULL) {
            icon = fallbackIcon;
        }
        else {
            GetFallbackIcon((IBitmap**)&icon);
            info->mUsingFallbackIcon = TRUE;
        }
    }
    info->SetIcon(icon);

    info->mTitle = CoreUtils::Convert(name);
    AutoPtr<IPackageManager> packageManager;
    assert(0);
    //mApp->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICharSequence> tmp = CoreUtils::Convert(name);
    packageManager->GetUserBadgedLabel(tmp, info->mUser,
            (ICharSequence**)&(info->mContentDescription));
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
        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        AutoPtr<IBitmap> saved;
        FAIL_GOTO(factory->DecodeByteArray(data, 0, data->GetLength(), (IBitmap**)&saved), ERROR)
        AutoPtr<IBitmap> loaded;
        loaded = info->GetIcon(mIconCache);
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
        StringBuilder sb;
        sb += "going to save icon bitmap for info=";
        sb += TO_STR(info);
        Slogger::D(TAG, sb.ToString());
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
    AutoPtr<IInteger64> value = CoreUtils::Convert(id);
    AutoPtr<IInterface> obj;
    folders->Get(TO_IINTERFACE(value), (IInterface**)&obj);
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

    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    AutoPtr<ICollator> collator;
    helper->GetInstance((ICollator**)&collator);

    AutoPtr<IComparator> c = new MyComparator3(collator);
    *comparator = c;
    REFCOUNT_ADD(*comparator);
    return NOERROR;
}

ECode LauncherModel::GetWidgetNameComparator(
    /* [out] */ IComparator** comparator)
{
    VALIDATE_NOT_NULL(comparator);

    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    AutoPtr<ICollator> collator;
    helper->GetInstance((ICollator**)&collator);
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
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        return CComponentName::New(pname, name, outname);
    }
    else {
        AutoPtr<IServiceInfo> serviceInfo;
        info->GetServiceInfo((IServiceInfo**)&serviceInfo);
        String pname;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(serviceInfo)->GetName(&name);
        return CComponentName::New(pname, name, outname);
    }
}

ECode LauncherModel::DumpState()
{
    StringBuilder sb;
    sb += "mCallbacks=";
    sb += TO_STR(mCallbacks);
    Slogger::D(TAG, sb.ToString());
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mAllAppsList.data"),
            mBgAllAppsList->mData);
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mAllAppsList.added"),
            mBgAllAppsList->mAdded);
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mAllAppsList.removed"),
            mBgAllAppsList->mRemoved);
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mAllAppsList.modified"),
            mBgAllAppsList->mModified);
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