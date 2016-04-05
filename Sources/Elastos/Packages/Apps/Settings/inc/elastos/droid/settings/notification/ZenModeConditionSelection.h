

package com.android.settings.notification;

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IRadioGroup;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class ZenModeConditionSelection extends RadioGroup {
    private static const String TAG = "ZenModeConditionSelection";
    private static const Boolean DEBUG = TRUE;

    private final INotificationManager mNoMan;
    private final H mHandler = new H();
    private final Context mContext;
    private final List<Condition> mConditions;
    private Condition mCondition;

    public ZenModeConditionSelection(Context context) {
        Super(context);
        mContext = context;
        mConditions = new ArrayList<Condition>();
        SetLayoutTransition(new LayoutTransition());
        final Int32 p = mContext->GetResources()->GetDimensionPixelSize(R.dimen.content_margin_left);
        SetPadding(p, p, p, 0);
        mNoMan = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        final RadioButton b = NewRadioButton(NULL);
        b->SetText(mContext->GetString(R::string::zen_mode_forever));
        b->SetChecked(TRUE);
        for (Int32 i = ZenModeConfig.MINUTE_BUCKETS.length - 1; i >= 0; --i) {
            HandleCondition(ZenModeConfig->ToTimeCondition(ZenModeConfig.MINUTE_BUCKETS[i]));
        }
    }

    private RadioButton NewRadioButton(Condition condition) {
        final RadioButton button = new RadioButton(mContext);
        button->SetTag(condition);
        button->SetOnCheckedChangeListener(new CompoundButton->OnCheckedChangeListener() {
            //@Override
            CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                if (isChecked) {
                    SetCondition((Condition) button->GetTag());
                }
            }
        });
        AddView(button);
        return button;
    }

    //@Override
    protected void OnAttachedToWindow() {
        super->OnAttachedToWindow();
        RequestZenModeConditions(Condition.FLAG_RELEVANT_NOW);
    }

    //@Override
    protected void OnDetachedFromWindow() {
        super->OnDetachedFromWindow();
        RequestZenModeConditions(0 /*none*/);
    }

    protected void RequestZenModeConditions(Int32 relevance) {
        if (DEBUG) Logger::D(TAG, "requestZenModeConditions " + Condition->RelevanceToString(relevance));
        try {
            mNoMan->RequestZenModeConditions(mListener, relevance);
        } catch (RemoteException e) {
            // noop
        }
    }

    protected void HandleConditions(Condition[] conditions) {
        for (Condition c : conditions) {
            HandleCondition(c);
        }
    }

    protected void HandleCondition(Condition c) {
        if (mConditions->Contains(c)) return;
        RadioButton v = (RadioButton) FindViewWithTag(c.id);
        if (c.state == Condition.STATE_TRUE || c.state == Condition.STATE_UNKNOWN) {
            if (v == NULL) {
                v = NewRadioButton(c);
            }
        }
        if (v != NULL) {
            v->SetText(c.summary);
            v->SetEnabled(c.state == Condition.STATE_TRUE);
        }
        mConditions->Add(c);
    }

    protected void SetCondition(Condition c) {
        if (DEBUG) Logger::D(TAG, "setCondition " + c);
        mCondition = c;
    }

    CARAPI ConfirmCondition() {
        if (DEBUG) Logger::D(TAG, "confirmCondition " + mCondition);
        try {
            mNoMan->SetZenModeCondition(mCondition);
        } catch (RemoteException e) {
            // noop
        }
    }

    private final IConditionListener mListener = new IConditionListener->Stub() {
        //@Override
        CARAPI OnConditionsReceived(Condition[] conditions) {
            if (conditions == NULL || conditions.length == 0) return;
            mHandler->ObtainMessage(H.CONDITIONS, conditions).SendToTarget();
        }
    };

    private final class H extends Handler {
        private static const Int32 CONDITIONS = 1;

        //@Override
        CARAPI HandleMessage(Message msg) {
            if (msg.what == CONDITIONS) HandleConditions((Condition[]) msg.obj);
        }
    }
}
