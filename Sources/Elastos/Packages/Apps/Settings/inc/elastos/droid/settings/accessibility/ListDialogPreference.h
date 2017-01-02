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

/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.accessibility;

using Elastos::Droid::App::AlertDialog::IBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemClickListener;
using Elastos::Droid::Widget::IBaseAdapter;

/**
 * Abstract dialog preference that displays a set of values and optional titles.
 */
public abstract class ListDialogPreference extends DialogPreference {
    private CharSequence[] mEntryTitles;
    private Int32[] mEntryValues;

    private OnValueChangedListener mOnValueChangedListener;

    /** The layout resource to use for grid items. */
    private Int32 mListItemLayout;

    /** The current value of this preference. */
    private Int32 mValue;

    /** The index within the value set of the current value. */
    private Int32 mValueIndex;

    /** Whether the value had been set using {@link #setValue}. */
    private Boolean mValueSet;

    public ListDialogPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    /**
     * Sets a listened to invoke when the value of this preference changes.
     *
     * @param listener the listener to invoke
     */
    CARAPI SetOnValueChangedListener(OnValueChangedListener listener) {
        mOnValueChangedListener = listener;
    }

    /**
     * Sets the layout to use for grid items.
     *
     * @param layoutResId the layout to use for displaying grid items
     */
    CARAPI SetListItemLayoutResource(Int32 layoutResId) {
        mListItemLayout = layoutResId;
    }

    /**
     * Sets the list of item values. Values must be distinct.
     *
     * @param values the list of item values
     */
    CARAPI SetValues(Int32[] values) {
        mEntryValues = values;

        if (mValueSet && mValueIndex == AbsListView.INVALID_POSITION) {
            mValueIndex = GetIndexForValue(mValue);
        }
    }

    /**
     * Sets the list of item titles. May be NULL if no titles are specified, or
     * may be shorter than the list of values to leave some titles unspecified.
     *
     * @param titles the list of item titles
     */
    CARAPI SetTitles(CharSequence[] titles) {
        mEntryTitles = titles;
    }

    /**
     * Populates a list item view with data for the specified index.
     *
     * @param view the view to populate
     * @param index the index for which to populate the view
     * @see #SetListItemLayoutResource(Int32)
     * @see #GetValueAt(Int32)
     * @see #GetTitleAt(Int32)
     */
    protected abstract void OnBindListItem(View view, Int32 index);

    /**
     * @return the title at the specified index, or NULL if none specified
     */
    protected CharSequence GetTitleAt(Int32 index) {
        if (mEntryTitles == NULL || mEntryTitles.length <= index) {
            return NULL;
        }

        return mEntryTitles[index];
    }

    /**
     * @return the value at the specified index
     */
    protected Int32 GetValueAt(Int32 index) {
        return mEntryValues[index];
    }

    //@Override
    public CharSequence GetSummary() {
        if (mValueIndex >= 0) {
            return GetTitleAt(mValueIndex);
        }

        return NULL;
    }

    //@Override
    protected void OnPrepareDialogBuilder(Builder builder) {
        super->OnPrepareDialogBuilder(builder);

        final Context context = GetContext();
        final Int32 dialogLayout = GetDialogLayoutResource();
        final View picker = LayoutInflater->From(context).Inflate(dialogLayout, NULL);
        final ListPreferenceAdapter adapter = new ListPreferenceAdapter();
        final AbsListView list = (AbsListView) picker->FindViewById(android.R.id.list);
        list->SetAdapter(adapter);
        list->SetOnItemClickListener(new OnItemClickListener() {
            //@Override
            CARAPI OnItemClick(AdapterView<?> adapter, View v, Int32 position, Int64 id) {
                if (CallChangeListener((Int32) id)) {
                    SetValue((Int32) id);
                }

                final Dialog dialog = GetDialog();
                if (dialog != NULL) {
                    dialog->Dismiss();
                }
            }
        });

        // Set initial selection.
        final Int32 selectedPosition = GetIndexForValue(mValue);
        if (selectedPosition != AbsListView.INVALID_POSITION) {
            list->SetSelection(selectedPosition);
        }

        builder->SetView(picker);
        builder->SetPositiveButton(NULL, NULL);
    }

    /**
     * @return the index of the specified value within the list of entry values,
     *         or {@link AbsListView#INVALID_POSITION} if not found
     */
    protected Int32 GetIndexForValue(Int32 value) {
        final Int32[] values = mEntryValues;
        if (values != NULL) {
            final Int32 count = values.length;
            for (Int32 i = 0; i < count; i++) {
                if (values[i] == value) {
                    return i;
                }
            }
        }

        return AbsListView.INVALID_POSITION;
    }

    /**
     * Sets the current value. If the value exists within the set of entry
     * values, updates the selection index.
     *
     * @param value the value to set
     */
    CARAPI SetValue(Int32 value) {
        final Boolean changed = mValue != value;
        if (changed || !mValueSet) {
            mValue = value;
            mValueIndex = GetIndexForValue(value);
            mValueSet = TRUE;
            PersistInt(value);
            if (changed) {
                NotifyDependencyChange(ShouldDisableDependents());
                NotifyChanged();
            }
            if (mOnValueChangedListener != NULL) {
                mOnValueChangedListener->OnValueChanged(this, value);
            }
        }
    }

    /**
     * @return the current value
     */
    public Int32 GetValue() {
        return mValue;
    }

    //@Override
    protected Object OnGetDefaultValue(TypedArray a, Int32 index) {
        return a->GetInt(index, 0);
    }

    //@Override
    protected void OnSetInitialValue(Boolean restoreValue, Object defaultValue) {
        SetValue(restoreValue ? GetPersistedInt(mValue) : (Integer) defaultValue);
    }

    //@Override
    protected Parcelable OnSaveInstanceState() {
        final Parcelable superState = super->OnSaveInstanceState();
        if (IsPersistent()) {
            // No need to save instance state since it's persistent
            return superState;
        }

        final SavedState myState = new SavedState(superState);
        myState.value = GetValue();
        return myState;
    }

    //@Override
    protected void OnRestoreInstanceState(Parcelable state) {
        if (state == NULL || !state->GetClass()->Equals(SavedState.class)) {
            // Didn't save state for us in onSaveInstanceState
            super->OnRestoreInstanceState(state);
            return;
        }

        SavedState myState = (SavedState) state;
        super->OnRestoreInstanceState(myState->GetSuperState());
        SetValue(myState.value);
    }

    private class ListPreferenceAdapter extends BaseAdapter {
        private LayoutInflater mInflater;

        //@Override
        public Int32 GetCount() {
            return mEntryValues.length;
        }

        //@Override
        public Integer GetItem(Int32 position) {
            return mEntryValues[position];
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return mEntryValues[position];
        }

        //@Override
        public Boolean HasStableIds() {
            return TRUE;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            if (convertView == NULL) {
                if (mInflater == NULL) {
                    mInflater = LayoutInflater->From(parent->GetContext());
                }
                convertView = mInflater->Inflate(mListItemLayout, parent, FALSE);
            }
            OnBindListItem(convertView, position);
            return convertView;
        }
    }

    private static class SavedState extends BaseSavedState {
        public Int32 value;

        public SavedState(Parcel source) {
            Super(source);
            value = source->ReadInt();
        }

        //@Override
        CARAPI WriteToParcel(Parcel dest, Int32 flags) {
            super->WriteToParcel(dest, flags);
            dest->WriteInt(value);
        }

        public SavedState(Parcelable superState) {
            Super(superState);
        }

        @SuppressWarnings({ "hiding", "unused" })
        public static const Creator<SavedState> CREATOR = new Creator<SavedState>() {
            //@Override
            public SavedState CreateFromParcel(Parcel in) {
                return new SavedState(in);
            }

            //@Override
            public SavedState[] NewArray(Int32 size) {
                return new SavedState[size];
            }
        };
    }

    public interface OnValueChangedListener {
        CARAPI OnValueChanged(ListDialogPreference preference, Int32 value);
    }
}
