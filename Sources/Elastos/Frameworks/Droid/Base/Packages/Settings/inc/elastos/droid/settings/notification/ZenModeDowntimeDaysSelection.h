

package com.android.settings.notification;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;
using Elastos::Droid::Widget::ILinearLayout;

using Elastos::Droid::Settings::IR;

using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ICalendar;

public class ZenModeDowntimeDaysSelection extends ScrollView {
    public static const Int32[] DAYS = {
        Calendar.MONDAY, Calendar.TUESDAY, Calendar.WEDNESDAY, Calendar.THURSDAY, Calendar.FRIDAY,
        Calendar.SATURDAY, Calendar.SUNDAY
    };
    private static const SimpleDateFormat DAY_FORMAT = new SimpleDateFormat("EEEE");

    private final SparseBooleanArray mDays = new SparseBooleanArray();
    private final LinearLayout mLayout;

    public ZenModeDowntimeDaysSelection(Context context, String mode) {
        Super(context);
        mLayout = new LinearLayout(mContext);
        final Int32 hPad = context->GetResources()->GetDimensionPixelSize(R.dimen.zen_downtime_margin);
        mLayout->SetPadding(hPad, 0, hPad, 0);
        AddView(mLayout);
        final Int32[] days = ZenModeConfig->TryParseDays(mode);
        if (days != NULL) {
            for (Int32 i = 0; i < days.length; i++) {
                mDays->Put(days[i], TRUE);
            }
        }
        mLayout->SetOrientation(LinearLayout.VERTICAL);
        final Calendar c = Calendar->GetInstance();
        final LayoutInflater inflater = LayoutInflater->From(context);
        for (Int32 i = 0; i < DAYS.length; i++) {
            final Int32 day = DAYS[i];
            final CheckBox checkBox = (CheckBox) inflater->Inflate(R.layout.zen_downtime_day,
                    this, FALSE);
            c->Set(Calendar.DAY_OF_WEEK, day);
            checkBox->SetText(DAY_FORMAT->Format(c->GetTime()));
            checkBox->SetChecked(mDays->Get(day));
            checkBox->SetOnCheckedChangeListener(new OnCheckedChangeListener() {
                //@Override
                CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                    mDays->Put(day, isChecked);
                    OnChanged(GetMode());
                }
            });
            mLayout->AddView(checkBox);
        }
    }

    private String GetMode() {
        final StringBuilder sb = new StringBuilder(ZenModeConfig.SLEEP_MODE_DAYS_PREFIX);
        Boolean empty = TRUE;
        for (Int32 i = 0; i < mDays->Size(); i++) {
            final Int32 day = mDays->KeyAt(i);
            if (!mDays->ValueAt(i)) continue;
            if (empty) {
                empty = FALSE;
            } else {
                sb->AppendChar32(',');
            }
            sb->Append(day);
        }
        return empty ? NULL : sb->ToString();
    }

    protected void OnChanged(String mode) {
        // event hook for subclasses
    }
}
