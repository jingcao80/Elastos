

package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ILinearLayout;

using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::IR;

using Elastos::Utility::IArrayList;

public class SwitchBar extends LinearLayout implements CompoundButton.OnCheckedChangeListener,
        View.OnClickListener {

    public static interface OnSwitchChangeListener {
        /**
         * Called when the checked state of the Switch has changed.
         *
         * @param switchView The Switch view whose state has changed.
         * @param isChecked  The new checked state of switchView.
         */
        void OnSwitchChanged(Switch switchView, Boolean isChecked);
    }

    private ToggleSwitch mSwitch;
    private TextView mTextView;

    private ArrayList<OnSwitchChangeListener> mSwitchChangeListeners =
            new ArrayList<OnSwitchChangeListener>();

    private static Int32[] MARGIN_ATTRIBUTES = {
            R.attr.switchBarMarginStart, R.attr.switchBarMarginEnd};

    public SwitchBar(Context context) {
        This(context, NULL);
    }

    public SwitchBar(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public SwitchBar(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public SwitchBar(Context context, AttributeSet attrs, Int32 defStyleAttr, Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);

        LayoutInflater->From(context).Inflate(R.layout.switch_bar, this);

        final TypedArray a = context->ObtainStyledAttributes(attrs, MARGIN_ATTRIBUTES);
        Int32 switchBarMarginStart = (Int32) a->GetDimension(0, 0);
        Int32 switchBarMarginEnd = (Int32) a->GetDimension(1, 0);
        a->Recycle();

        mTextView = (TextView) FindViewById(R.id.switch_text);
        mTextView->SetText(R::string::switch_off_text);
        ViewGroup.MarginLayoutParams lp = (MarginLayoutParams) mTextView->GetLayoutParams();
        lp->SetMarginStart(switchBarMarginStart);

        mSwitch = (ToggleSwitch) FindViewById(R.id.switch_widget);
        // Prevent OnSaveInstanceState() to be called as we are managing the state of the Switch
        // on our own
        mSwitch->SetSaveEnabled(FALSE);
        lp = (MarginLayoutParams) mSwitch->GetLayoutParams();
        lp->SetMarginEnd(switchBarMarginEnd);

        AddOnSwitchChangeListener(new OnSwitchChangeListener() {
            //@Override
            CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
                SetTextViewLabel(isChecked);
            }
        });

        SetOnClickListener(this);

        // Default is hide
        SetVisibility(View.GONE);
    }

    CARAPI SetTextViewLabel(Boolean isChecked) {
        mTextView->SetText(isChecked ? R::string::switch_on_text : R::string::switch_off_text);
    }

    CARAPI SetChecked(Boolean checked) {
        SetTextViewLabel(checked);
        mSwitch->SetChecked(checked);
    }

    CARAPI SetCheckedInternal(Boolean checked) {
        SetTextViewLabel(checked);
        mSwitch->SetCheckedInternal(checked);
    }

    public Boolean IsChecked() {
        return mSwitch->IsChecked();
    }

    CARAPI SetEnabled(Boolean enabled) {
        super->SetEnabled(enabled);
        mTextView->SetEnabled(enabled);
        mSwitch->SetEnabled(enabled);
    }

    public final ToggleSwitch GetSwitch() {
        return mSwitch;
    }

    CARAPI Show() {
        if (!IsShowing()) {
            SetVisibility(View.VISIBLE);
            mSwitch->SetOnCheckedChangeListener(this);
        }
    }

    CARAPI Hide() {
        if (IsShowing()) {
            SetVisibility(View.GONE);
            mSwitch->SetOnCheckedChangeListener(NULL);
        }
    }

    public Boolean IsShowing() {
        return (GetVisibility() == View.VISIBLE);
    }

    //@Override
    CARAPI OnClick(View v) {
        final Boolean isChecked = !mSwitch->IsChecked();
        SetChecked(isChecked);
    }

    CARAPI PropagateChecked(Boolean isChecked) {
        final Int32 count = mSwitchChangeListeners->Size();
        for (Int32 n = 0; n < count; n++) {
            mSwitchChangeListeners->Get(n).OnSwitchChanged(mSwitch, isChecked);
        }
    }

    //@Override
    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        PropagateChecked(isChecked);
    }

    CARAPI AddOnSwitchChangeListener(OnSwitchChangeListener listener) {
        if (mSwitchChangeListeners->Contains(listener)) {
            throw new IllegalStateException("Cannot add twice the same OnSwitchChangeListener");
        }
        mSwitchChangeListeners->Add(listener);
    }

    CARAPI RemoveOnSwitchChangeListener(OnSwitchChangeListener listener) {
        if (!mSwitchChangeListeners->Contains(listener)) {
            throw new IllegalStateException("Cannot remove OnSwitchChangeListener");
        }
        mSwitchChangeListeners->Remove(listener);
    }

    static class SavedState extends BaseSavedState {
        Boolean checked;
        Boolean visible;

        SavedState(Parcelable superState) {
            Super(superState);
        }

        /**
         * Constructor called from {@link #CREATOR}
         */
        private SavedState(Parcel in) {
            Super(in);
            checked = (Boolean)in->ReadValue(NULL);
            visible = (Boolean)in->ReadValue(NULL);
        }

        //@Override
        CARAPI WriteToParcel(Parcel out, Int32 flags) {
            super->WriteToParcel(out, flags);
            out->WriteValue(checked);
            out->WriteValue(visible);
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "SwitchBar.SavedState{"
                    + Integer->ToHexString(System->IdentityHashCode(this))
                    + " checked=" + checked
                    + " visible=" + visible + "}";
        }

        public static const Parcelable.Creator<SavedState> CREATOR
                = new Parcelable.Creator<SavedState>() {
            public SavedState CreateFromParcel(Parcel in) {
                return new SavedState(in);
            }

            public SavedState[] NewArray(Int32 size) {
                return new SavedState[size];
            }
        };
    }

    //@Override
    public Parcelable OnSaveInstanceState() {
        Parcelable superState = super->OnSaveInstanceState();

        SavedState ss = new SavedState(superState);
        ss.checked = mSwitch->IsChecked();
        ss.visible = IsShowing();
        return ss;
    }

    //@Override
    CARAPI OnRestoreInstanceState(Parcelable state) {
        SavedState ss = (SavedState) state;

        super->OnRestoreInstanceState(ss->GetSuperState());

        mSwitch->SetCheckedInternal(ss.checked);
        SetTextViewLabel(ss.checked);
        SetVisibility(ss.visible ? View.VISIBLE : View.GONE);
        mSwitch->SetOnCheckedChangeListener(ss.visible ? this : NULL);

        RequestLayout();
    }
}
