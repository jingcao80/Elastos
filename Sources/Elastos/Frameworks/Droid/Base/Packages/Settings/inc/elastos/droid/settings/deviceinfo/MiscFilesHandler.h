/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.deviceinfo;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IListActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::View::IOnLongClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;
using Elastos::Droid::Widget::IListView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Deviceinfo::StorageMeasurement::IFileInfo;

using Elastos::IO::IFile;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * This class handles the selection and removal of Misc files.
 */
public class MiscFilesHandler extends ListActivity {
    private static const String TAG = "MemorySettings";
    private String mNumSelectedFormat;
    private String mNumBytesSelectedFormat;
    private MemoryMearurementAdapter mAdapter;
    private LayoutInflater mInflater;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        SetFinishOnTouchOutside(TRUE);
        SetTitle(R::string::misc_files);
        mNumSelectedFormat = GetString(R::string::misc_files_selected_count);
        mNumBytesSelectedFormat = GetString(R::string::misc_files_selected_count_bytes);
        mAdapter = new MemoryMearurementAdapter(this);
        mInflater = (LayoutInflater)GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        SetContentView(R.layout.settings_storage_miscfiles_list);
        ListView lv = GetListView();
        lv->SetItemsCanFocus(TRUE);
        lv->SetChoiceMode(ListView.CHOICE_MODE_MULTIPLE_MODAL);
        lv->SetMultiChoiceModeListener(new ModeCallback(this));
        SetListAdapter(mAdapter);
    } 

    private class ModeCallback implements ListView.MultiChoiceModeListener {
        private Int32 mDataCount;
        private final Context mContext;

        public ModeCallback(Context context) {
            mContext = context;
            mDataCount = mAdapter->GetCount();
        }

        public Boolean OnCreateActionMode(ActionMode mode, Menu menu) {
            final MenuInflater inflater = GetMenuInflater();
            inflater->Inflate(R.menu.misc_files_menu, menu);
            return TRUE;
        }

        public Boolean OnPrepareActionMode(ActionMode mode, Menu menu) {
            return TRUE;
        }

        public Boolean OnActionItemClicked(ActionMode mode, MenuItem item) {
            ListView lv = GetListView();
            switch (item->GetItemId()) {
            case R.id.action_delete:
                // delete the files selected
                SparseBooleanArray checkedItems = lv->GetCheckedItemPositions();
                Int32 checkedCount = GetListView()->GetCheckedItemCount();
                if (checkedCount > mDataCount) {
                    throw new IllegalStateException("checked item counts do not match. " +
                            "checkedCount: " + checkedCount + ", dataSize: " + mDataCount);
                }
                if (mDataCount > 0) {
                    ArrayList<Object> toRemove = new ArrayList<Object>();
                    for (Int32 i = 0; i < mDataCount; i++) {
                        if (!checkedItems->Get(i)) {
                            //item not selected
                            continue;
                        }
                        if (StorageMeasurement.LOGV) {
                            Logger::I(TAG, "deleting: " + mAdapter->GetItem(i));
                        }
                        // delete the file
                        File file = new File(mAdapter->GetItem(i).mFileName);
                        if (file->IsDirectory()) {
                            DeleteDir(file);
                        } else {
                            file->Delete();                            
                        }
                        toRemove->Add(mAdapter->GetItem(i));
                    }
                    mAdapter->RemoveAll(toRemove);
                    mAdapter->NotifyDataSetChanged();
                    mDataCount = mAdapter->GetCount();
                }
                mode->Finish();
                break;

            case R.id.action_select_all:
                // check ALL items
                for (Int32 i = 0; i < mDataCount; i++) {
                    lv->SetItemChecked(i, TRUE);
                }
                // update the title and subtitle with number selected and numberBytes selected
                OnItemCheckedStateChanged(mode, 1, 0, TRUE);
                break;
            }
            return TRUE;
        }

        // Deletes all files and subdirectories under given dir.
        // Returns TRUE if all deletions were successful.
        // If a deletion fails, the method stops attempting to delete and returns FALSE.
        private Boolean DeleteDir(File dir) {
            String[] children = dir->List();
            if (children != NULL) {
                for (Int32 i=0; i < children.length; i++) {
                    Boolean success = DeleteDir(new File(dir, children[i]));
                    if (!success) {
                        return FALSE;
                    }
                }
            }
            // The directory is now empty so delete it
            return dir->Delete();
        }

        CARAPI OnDestroyActionMode(ActionMode mode) {
            // This block intentionally left blank
        }

        CARAPI OnItemCheckedStateChanged(ActionMode mode, Int32 position, Int64 id,
                Boolean checked) {
            ListView lv = GetListView();
            Int32 numChecked = lv->GetCheckedItemCount();
            mode->SetTitle(String->Format(mNumSelectedFormat, numChecked, mAdapter->GetCount()));

            // total the sizes of all items selected so far
            SparseBooleanArray checkedItems = lv->GetCheckedItemPositions();
            Int64 selectedDataSize = 0;
            if (numChecked > 0) {
                for (Int32 i = 0; i < mDataCount; i++) {
                    if (checkedItems->Get(i)) {
                        // item is checked
                        selectedDataSize += mAdapter->GetItem(i).mSize;
                    }
                }
            }
            mode->SetSubtitle(String->Format(mNumBytesSelectedFormat,
                    Formatter->FormatFileSize(mContext, selectedDataSize),
                    Formatter->FormatFileSize(mContext, mAdapter->GetDataSize())));
        }
    }

    class MemoryMearurementAdapter extends BaseAdapter {
        private ArrayList<StorageMeasurement.FileInfo> mData = NULL;
        private Int64 mDataSize = 0;
        private Context mContext;

        public MemoryMearurementAdapter(Activity activity) {
            mContext = activity;
            final StorageVolume storageVolume = activity->GetIntent()->GetParcelableExtra(
                    StorageVolume.EXTRA_STORAGE_VOLUME);
            StorageMeasurement mMeasurement = StorageMeasurement->GetInstance(
                    activity, storageVolume);
            if (mMeasurement == NULL) return;
            mData = (ArrayList<StorageMeasurement.FileInfo>) mMeasurement.mFileInfoForMisc;
            if (mData != NULL) {
                for (StorageMeasurement.FileInfo info : mData) {
                    mDataSize += info.mSize;
                }
            }
        }

        //@Override
        public Int32 GetCount() {
            return (mData == NULL) ? 0 : mData->Size();
        }

        //@Override
        public StorageMeasurement.FileInfo GetItem(Int32 position) {
            if (mData == NULL || mData->Size() <= position) {
                return NULL;
            }
            return mData->Get(position);
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            if (mData == NULL || mData->Size() <= position) {
                return 0;
            }
            return mData->Get(position).mId;
        }

        CARAPI RemoveAll(List<Object> objs) {
            if (mData == NULL) {
                return;
            }
            for (Object o : objs) {
                mData->Remove(o);
                mDataSize -= ((StorageMeasurement.FileInfo) o).mSize;
            }
        }

        public Int64 GetDataSize() {
            return mDataSize;
        }

        //@Override
        CARAPI NotifyDataSetChanged() {
            super->NotifyDataSetChanged();
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            final FileItemInfoLayout view = (convertView == NULL) ?
                    (FileItemInfoLayout) mInflater->Inflate(R.layout.settings_storage_miscfiles,
                            parent, FALSE) : (FileItemInfoLayout) convertView;
            FileInfo item = GetItem(position);
            view->SetFileName(item.mFileName);
            view->SetFileSize(Formatter->FormatFileSize(mContext, item.mSize));
            final ListView listView = (ListView) parent;
            final Int32 listPosition = position;
            view->GetCheckBox()->SetOnCheckedChangeListener(new OnCheckedChangeListener() {
                
                //@Override
                CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                    listView->SetItemChecked(listPosition, isChecked);
                }
                
            });
            view->SetOnLongClickListener(new OnLongClickListener() {
                //@Override
                public Boolean OnLongClick(View v) {
                    if (listView->GetCheckedItemCount() > 0) {
                        return FALSE;
                    }
                    listView->SetItemChecked(listPosition, !view->IsChecked());
                    return TRUE;
                }
            });
            view->SetOnClickListener(new OnClickListener() {
                //@Override
                CARAPI OnClick(View v) {
                    if (listView->GetCheckedItemCount() > 0) {
                        listView->SetItemChecked(listPosition, !view->IsChecked());
                    }
                }
            });
            return view;
        }
    }
}
