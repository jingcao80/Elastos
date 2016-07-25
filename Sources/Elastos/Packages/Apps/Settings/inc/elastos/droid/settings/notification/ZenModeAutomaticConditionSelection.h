

package com.android.settings.notification;

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ILinearLayout;

using Elastos::Droid::Settings::IR;

public class ZenModeAutomaticConditionSelection extends LinearLayout {
    private static const String TAG = "ZenModeAutomaticConditionSelection";
    private static const Boolean DEBUG = TRUE;

    private final INotificationManager mNoMan;
    private final H mHandler = new H();
    private final Context mContext;
    private final ArraySet<Uri> mSelectedConditions = new ArraySet<Uri>();

    public ZenModeAutomaticConditionSelection(Context context) {
        Super(context);
        mContext = context;
        SetOrientation(VERTICAL);
        SetLayoutTransition(new LayoutTransition());
        final Int32 p = mContext->GetResources()->GetDimensionPixelSize(R.dimen.content_margin_left);
        SetPadding(p, p, p, 0);
        mNoMan = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        RefreshSelectedConditions();
    }

    private void RefreshSelectedConditions() {
        try {
            final Condition[] automatic = mNoMan->GetAutomaticZenModeConditions();
            mSelectedConditions->Clear();
            if (automatic != NULL) {
                for (Condition c : automatic) {
                    mSelectedConditions->Add(c.id);
                }
            }
        } catch (RemoteException e) {
            Logger::W(TAG, "Error calling getAutomaticZenModeConditions", e);
        }
    }

    private CheckBox NewCheckBox(Object tag) {
        final CheckBox button = new CheckBox(mContext);
        button->SetTag(tag);
        button->SetOnCheckedChangeListener(new CompoundButton->OnCheckedChangeListener() {
            //@Override
            CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                 SetSelectedCondition((Uri)button->GetTag(), isChecked);
            }
        });
        AddView(button);
        return button;
    }

    private void SetSelectedCondition(Uri conditionId, Boolean selected) {
        if (DEBUG) Logger::D(TAG, "setSelectedCondition conditionId=" + conditionId
                + " selected=" + selected);
        if (selected) {
            mSelectedConditions->Add(conditionId);
        } else {
            mSelectedConditions->Remove(conditionId);
        }
        final Uri[] automatic = new Uri[mSelectedConditions->Size()];
        for (Int32 i = 0; i < automatic.length; i++) {
            automatic[i] = mSelectedConditions->ValueAt(i);
        }
        try {
            mNoMan->SetAutomaticZenModeConditions(automatic);
        } catch (RemoteException e) {
            Logger::W(TAG, "Error calling setAutomaticZenModeConditions", e);
        }
    }

    //@Override
    protected OnAttachedToWindow() {
        super->OnAttachedToWindow();
        RequestZenModeConditions(Condition.FLAG_RELEVANT_ALWAYS);
    }

    //@Override
    protected OnDetachedFromWindow() {
        super->OnDetachedFromWindow();
        RequestZenModeConditions(0 /*none*/);
    }

    protected void RequestZenModeConditions(Int32 relevance) {
        if (DEBUG) Logger::D(TAG, "requestZenModeConditions " + Condition->RelevanceToString(relevance));
        try {
            mNoMan->RequestZenModeConditions(mListener, relevance);
        } catch (RemoteException e) {
            Logger::W(TAG, "Error calling requestZenModeConditions", e);
        }
    }

    protected void HandleConditions(Condition[] conditions) {
        for (final Condition c : conditions) {
            CheckBox v = (CheckBox) FindViewWithTag(c.id);
            if (c.state != Condition.STATE_ERROR) {
                if (v == NULL) {
                    v = NewCheckBox(c.id);
                }
            }
            if (v != NULL) {
                v->SetText(c.summary);
                v->SetEnabled(c.state != Condition.STATE_ERROR);
                v->SetChecked(mSelectedConditions->Contains(c.id));
            }
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
            if (msg.what == CONDITIONS) HandleConditions((Condition[])msg.obj);
        }
    }
}
