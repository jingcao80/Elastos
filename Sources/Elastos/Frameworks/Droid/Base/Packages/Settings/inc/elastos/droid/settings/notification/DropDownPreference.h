

package com.android.settings.notification;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;

using Elastos::Utility::IArrayList;

public class DropDownPreference extends Preference {
    private final Context mContext;
    private final ArrayAdapter<String> mAdapter;
    private final Spinner mSpinner;
    private final ArrayList<Object> mValues = new ArrayList<Object>();

    private Callback mCallback;

    public DropDownPreference(Context context) {
        This(context, NULL);
    }

    public DropDownPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
        mContext = context;
        mAdapter = new ArrayAdapter<String>(mContext,
                android.R.layout.simple_spinner_dropdown_item);

        mSpinner = new Spinner(mContext);

        mSpinner->SetVisibility(View.INVISIBLE);
        mSpinner->SetAdapter(mAdapter);
        mSpinner->SetOnItemSelectedListener(new OnItemSelectedListener() {
            //@Override
            CARAPI OnItemSelected(AdapterView<?> parent, View v, Int32 position, Int64 id) {
                SetSelectedItem(position);
            }

            //@Override
            CARAPI OnNothingSelected(AdapterView<?> parent) {
                // noop
            }
        });
        SetPersistent(FALSE);
        SetOnPreferenceClickListener(new OnPreferenceClickListener() {
            //@Override
            public Boolean OnPreferenceClick(Preference preference) {
                mSpinner->PerformClick();
                return TRUE;
            }
        });
    }

    CARAPI SetDropDownWidth(Int32 dimenResId) {
        mSpinner->SetDropDownWidth(mContext->GetResources()->GetDimensionPixelSize(dimenResId));
    }

    CARAPI SetCallback(Callback callback) {
        mCallback = callback;
    }

    CARAPI SetSelectedItem(Int32 position) {
        final Object value = mValues->Get(position);
        if (mCallback != NULL && !mCallback->OnItemSelected(position, value)) {
            return;
        }
        mSpinner->SetSelection(position);
        SetSummary(mAdapter->GetItem(position));
        final Boolean disableDependents = value == NULL;
        NotifyDependencyChange(disableDependents);
    }

    CARAPI SetSelectedValue(Object value) {
        final Int32 i = mValues->IndexOf(value);
        if (i > -1) {
            SetSelectedItem(i);
        }
    }

    CARAPI AddItem(Int32 captionResid, Object value) {
        AddItem(mContext->GetResources()->GetString(captionResid), value);
    }

    CARAPI AddItem(String caption, Object value) {
        mAdapter->Add(caption);
        mValues->Add(value);
    }

    CARAPI ClearItems(){
        mAdapter->Clear();
        mValues->Clear();
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        if (view->Equals(mSpinner->GetParent())) return;
        if (mSpinner->GetParent() != NULL) {
            ((ViewGroup)mSpinner->GetParent()).RemoveView(mSpinner);
        }
        final ViewGroup vg = (ViewGroup)view;
        vg->AddView(mSpinner, 0);
        final ViewGroup.LayoutParams lp = mSpinner->GetLayoutParams();
        lp.width = 0;
        mSpinner->SetLayoutParams(lp);
    }

    public interface Callback {
        Boolean OnItemSelected(Int32 pos, Object value);
    }
}
