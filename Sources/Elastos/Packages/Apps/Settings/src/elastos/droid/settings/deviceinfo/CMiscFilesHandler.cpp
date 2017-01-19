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

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/deviceinfo/CMiscFilesHandler.h"
#include "elastos/droid/settings/deviceinfo/CFileItemInfoLayout.h"
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/Formatter.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_IMultiChoiceModeListener;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  CMiscFilesHandler::MemoryMearurementAdapter::GetViewOnCheckedChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CMiscFilesHandler::MemoryMearurementAdapter::GetViewOnCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

CMiscFilesHandler::MemoryMearurementAdapter::GetViewOnCheckedChangeListener::GetViewOnCheckedChangeListener(
    /* [in] */ MemoryMearurementAdapter* host,
    /* [in] */ IListView* listView,
    /* [in] */ Int32 listPosition)
    : mHost(host)
    , mListView(listView)
    , mListPosition(listPosition)
{}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetViewOnCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return IAbsListView::Probe(mListView)->SetItemChecked(mListPosition, isChecked);
}

//===============================================================================
//                  CMiscFilesHandler::MemoryMearurementAdapter::GetOnLongClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CMiscFilesHandler::MemoryMearurementAdapter::GetOnLongClickListener, Object, IViewOnLongClickListener)

CMiscFilesHandler::MemoryMearurementAdapter::GetOnLongClickListener::GetOnLongClickListener(
    /* [in] */ MemoryMearurementAdapter* host,
    /* [in] */ IListView* listView,
    /* [in] */ Int32 listPosition,
    /* [in] */ Boolean viewIsChecked)
    : mHost(host)
    , mListView(listView)
    , mListPosition(listPosition)
    , mViewIsChecked(viewIsChecked)
{}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetOnLongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count;
    IAbsListView* listView = IAbsListView::Probe(mListView);
    if ((listView->GetCheckedItemCount(&count), count) > 0) {
        *result = FALSE;
        return NOERROR;
    }
    listView->SetItemChecked(mListPosition, !mViewIsChecked);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CMiscFilesHandler::MemoryMearurementAdapter::GetOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CMiscFilesHandler::MemoryMearurementAdapter::GetOnClickListener, Object, IViewOnClickListener)

CMiscFilesHandler::MemoryMearurementAdapter::GetOnClickListener::GetOnClickListener(
    /* [in] */ MemoryMearurementAdapter* host,
    /* [in] */ IListView* listView,
    /* [in] */ Int32 listPosition,
    /* [in] */ Boolean viewIsChecked)
    : mHost(host)
    , mListView(listView)
    , mListPosition(listPosition)
    , mViewIsChecked(viewIsChecked)
{}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 count;
    IAbsListView* listView = IAbsListView::Probe(mListView);
    if ((listView->GetCheckedItemCount(&count), count) > 0) {
        listView->SetItemChecked(mListPosition, !mViewIsChecked);
    }
    return NOERROR;
}

//===============================================================================
//                  CMiscFilesHandler::MemoryMearurementAdapter
//===============================================================================

CMiscFilesHandler::MemoryMearurementAdapter::MemoryMearurementAdapter(
    /* [in] */ IActivity* activity,
    /* [in] */ CMiscFilesHandler* host)
    : mDataSize(0)
    , mHost(host)
{
    mContext = IContext::Probe(activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            IStorageVolume::EXTRA_STORAGE_VOLUME, (IParcelable**)&parcelable);
    IStorageVolume* storageVolume = IStorageVolume::Probe(parcelable);
    AutoPtr<StorageMeasurement> mMeasurement = StorageMeasurement::GetInstance(
            IContext::Probe(activity), storageVolume);
    if (mMeasurement == NULL) return;

    mData = mMeasurement->mFileInfoForMisc;
    if (mData != NULL) {
        Int32 size;
        mData->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mData->Get(i, (IInterface**)&obj);
            StorageMeasurement::FileInfo* info = (StorageMeasurement::FileInfo*)IObject::Probe(obj);
            mDataSize += info->mSize;
        }
    }
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mData == NULL) {
        *result = 0;
        return NOERROR;
    }
    return mData->GetSize(result);
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result) //StorageMeasurement.FileInfo
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 size;
    if (mData == NULL || (mData->GetSize(&size), size <= position)) {
        return NOERROR;
    }
    return mData->Get(position, result);
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 size;
    if (mData == NULL || (mData->GetSize(&size), size <= position)) {
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mData->Get(position, (IInterface**)&obj);
    *result = ((StorageMeasurement::FileInfo*)IObject::Probe(obj))->mId;
    return NOERROR;
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::RemoveAll(
    /* [in] */ IList* objs) //List<Object>
{
    if (mData == NULL) {
        return NOERROR;
    }
    Int32 size;
    objs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        objs->Get(i, (IInterface**)&o);

        mData->Remove(o);
        mDataSize -= ((StorageMeasurement::FileInfo*)IObject::Probe(o))->mSize;
    }
    return NOERROR;
}

Int64 CMiscFilesHandler::MemoryMearurementAdapter::GetDataSize()
{
    return mDataSize;
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::NotifyDataSetChanged()
{
    return BaseAdapter::NotifyDataSetChanged();
}

ECode CMiscFilesHandler::MemoryMearurementAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CFileItemInfoLayout> view;
    if (convertView == NULL) {
        AutoPtr<IView> tmp;
        mHost->mInflater->Inflate(R::layout::settings_storage_miscfiles, parent, FALSE, (IView**)&tmp);
        view = (CFileItemInfoLayout*)tmp.Get();
    }
    else {
        view = (CFileItemInfoLayout*)convertView;
    }
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    StorageMeasurement::FileInfo* item = (StorageMeasurement::FileInfo*)IObject::Probe(obj);
    view->SetFileName(item->mFileName);
    view->SetFileSize(Formatter::FormatFileSize(mContext, item->mSize));
    AutoPtr<IListView> listView = IListView::Probe(parent);
    Int32 listPosition = position;
    AutoPtr<GetViewOnCheckedChangeListener> listener = new GetViewOnCheckedChangeListener(this, listView, listPosition);
    ICompoundButton::Probe(view->GetCheckBox())->SetOnCheckedChangeListener(listener);

    Boolean viewIsChecked;
    view->IsChecked(&viewIsChecked);
    AutoPtr<GetOnLongClickListener> listener1 = new GetOnLongClickListener(this, listView, listPosition, viewIsChecked);
    view->SetOnLongClickListener(listener1);

    AutoPtr<GetOnClickListener> listener2 = new GetOnClickListener(this, listView, listPosition, viewIsChecked);
    view->SetOnClickListener(listener2);
    *result = (IView*)view.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  CMiscFilesHandler::ModeCallback
//===============================================================================

CAR_INTERFACE_IMPL_2(CMiscFilesHandler::ModeCallback, Object, IMultiChoiceModeListener, IActionModeCallback)

CMiscFilesHandler::ModeCallback::ModeCallback(
    /* [in] */ IContext* context,
    /* [in] */ CMiscFilesHandler* host)
{
    mContext = context;
    mHost = host;
    mHost->mAdapter->GetCount(&mDataCount);
}

ECode CMiscFilesHandler::ModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMenuInflater> inflater;
    mHost->GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::misc_files_menu, menu);
    *result = TRUE;
    return NOERROR;
}

ECode CMiscFilesHandler::ModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode CMiscFilesHandler::ModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IListView> lv;
    mHost->GetListView((IListView**)&lv);
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case R::id::action_delete: {
            // delete the files selected
            IAbsListView* _lv = IAbsListView::Probe(lv);
            AutoPtr<ISparseBooleanArray> checkedItems;
            _lv->GetCheckedItemPositions((ISparseBooleanArray**)&checkedItems);
            Int32 checkedCount;
            _lv->GetCheckedItemCount(&checkedCount);
            if (checkedCount > mDataCount) {
                Logger::E(TAG, "checked item counts do not match. checkedCount: %d, dataSize: %d", checkedCount, mDataCount);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            if (mDataCount > 0) {
                AutoPtr<IList> toRemove; //ArrayList<Object>
                CArrayList::New((IList**)&toRemove);
                for (Int32 i = 0; i < mDataCount; i++) {
                    Boolean res;
                    checkedItems->Get(i, &res);
                    if (!res) {
                        //item not selected
                        continue;
                    }

                    AutoPtr<IInterface> obj;
                    mHost->mAdapter->GetItem(i, (IInterface**)&obj);
                    if (StorageMeasurement::LOGV) {
                        Logger::I(TAG, "deleting: %s", TO_CSTR(obj));
                    }
                    // delete the file
                    AutoPtr<IFile> file;
                    CFile::New(((StorageMeasurement::FileInfo*)IObject::Probe(obj))->mFileName, (IFile**)&file);
                    if (file->IsDirectory(&res), res) {
                        DeleteDir(file);
                    }
                    else {
                        file->Delete();
                    }
                    toRemove->Add(obj);
                }
                mHost->mAdapter->RemoveAll(toRemove);
                mHost->mAdapter->NotifyDataSetChanged();
                mHost->mAdapter->GetCount(&mDataCount);
            }
            mode->Finish();
            break;
        }

        case R::id::action_select_all:
            // check ALL items
            for (Int32 i = 0; i < mDataCount; i++) {
                IAbsListView::Probe(lv)->SetItemChecked(i, TRUE);
            }
            // update the title and subtitle with number selected and numberBytes selected
            OnItemCheckedStateChanged(mode, 1, 0, TRUE);
            break;
        }
    *result = TRUE;
    return NOERROR;
}

Boolean CMiscFilesHandler::ModeCallback::DeleteDir(
    /* [in] */ IFile* dir)
{
    AutoPtr< ArrayOf<String> > children;
    dir->List((ArrayOf<String>**)&children);
    if (children != NULL) {
        for (Int32 i = 0; i < children->GetLength(); i++) {
            AutoPtr<IFile> file;
            CFile::New(dir, (*children)[i], (IFile**)&file);
            Boolean success = DeleteDir(file);
            if (!success) {
                return FALSE;
            }
        }
    }
    // The directory is now empty so delete it
    Boolean res;
    dir->Delete(&res);
    return res;
}

ECode CMiscFilesHandler::ModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    // This block intentionally left blank
    return NOERROR;
}

ECode CMiscFilesHandler::ModeCallback::OnItemCheckedStateChanged(
    /* [in] */ IActionMode* mode,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [in] */ Boolean checked)
{
    AutoPtr<IListView> lv;
    mHost->GetListView((IListView**)&lv);
    IAbsListView* _lv = IAbsListView::Probe(lv);
    Int32 numChecked;
    _lv->GetCheckedItemCount(&numChecked);
    Int32 count;
    mHost->mAdapter->GetCount(&count);
    mode->SetTitle(CoreUtils::Convert(String::Format(mHost->mNumSelectedFormat, numChecked, count)));

    // total the sizes of all items selected so far
    AutoPtr<ISparseBooleanArray> checkedItems;
    _lv->GetCheckedItemPositions((ISparseBooleanArray**)&checkedItems);
    Int64 selectedDataSize = 0;
    if (numChecked > 0) {
        for (Int32 i = 0; i < mDataCount; i++) {
            Boolean res;
            checkedItems->Get(i, &res);
            if (res) {
                // item is checked
                AutoPtr<IInterface> obj;
                mHost->mAdapter->GetItem(i, (IInterface**)&obj);
                selectedDataSize += ((StorageMeasurement::FileInfo*)IObject::Probe(obj))->mSize;
            }
        }
    }

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, CoreUtils::Convert(Formatter::FormatFileSize(mContext, selectedDataSize)));
    args->Set(1, CoreUtils::Convert(Formatter::FormatFileSize(mContext, mHost->mAdapter->GetDataSize())));

    mode->SetSubtitle(CoreUtils::Convert(StringUtils::Format(mHost->mNumBytesSelectedFormat, args)));
    return NOERROR;
}

//===============================================================================
//                  CMiscFilesHandler
//===============================================================================

const String CMiscFilesHandler::TAG("MemorySettings");

CMiscFilesHandler::CMiscFilesHandler()
{}

CMiscFilesHandler::~CMiscFilesHandler()
{}

ECode CMiscFilesHandler::constructor()
{
    return ListActivity::constructor();
}

ECode CMiscFilesHandler::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    ListActivity::OnCreate(savedInstanceState);
    SetFinishOnTouchOutside(TRUE);
    SetTitle(R::string::misc_files);
    GetString(R::string::misc_files_selected_count, &mNumSelectedFormat);
    GetString(R::string::misc_files_selected_count_bytes, &mNumBytesSelectedFormat);
    mAdapter = new MemoryMearurementAdapter((IActivity*)this, this);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    SetContentView(R::layout::settings_storage_miscfiles_list);
    AutoPtr<IListView> lv;
    GetListView((IListView**)&lv);
    lv->SetItemsCanFocus(TRUE);
    IAbsListView* _lv = IAbsListView::Probe(lv);
    _lv->SetChoiceMode(IAbsListView::CHOICE_MODE_MULTIPLE_MODAL);
    AutoPtr<ModeCallback> mc = new ModeCallback((IContext*)this, this);
    _lv->SetMultiChoiceModeListener(mc);
    SetListAdapter(mAdapter);
    return NOERROR;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos