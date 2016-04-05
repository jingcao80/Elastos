package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;


/**
 * Radio Band Mode Selection Class
 *
 * It will query baseband about all available band modes and display them
 * in screen. It will display all six band modes if the query failed.
 *
 * After user select one band, it will send the selection to baseband.
 *
 * It will alter user the result of select operation and exit, no matter success
 * or not.
 *
 */
public class BandMode extends Activity {
    private static const String LOG_TAG = "phone";
    private static const Boolean DBG = FALSE;

    private static const Int32 EVENT_BAND_SCAN_COMPLETED = 100;
    private static const Int32 EVENT_BAND_SELECTION_DONE = 200;

    private static const String[] BAND_NAMES = new String[] {
            "Automatic",
            "EURO Band",
            "USA Band",
            "JAPAN Band",
            "AUS Band",
            "AUS2 Band"
    };

    private ListView mBandList;
    private ArrayAdapter mBandListAdapter;
    private BandListItem mTargetBand = NULL;
    private DialogInterface mProgressPanel;

    private Phone mPhone = NULL;

    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        RequestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);

        SetContentView(R.layout.band_mode);

        SetTitle(GetString(R::string::band_mode_title));
        GetWindow()->SetLayout(WindowManager.LayoutParams.MATCH_PARENT,
                                    WindowManager.LayoutParams.WRAP_CONTENT);

        mPhone = PhoneFactory->GetDefaultPhone();

        mBandList = (ListView) FindViewById(R.id.band);
        mBandListAdapter = new ArrayAdapter<BandListItem>(this,
                android.R.layout.simple_list_item_1);
        mBandList->SetAdapter(mBandListAdapter);
        mBandList->SetOnItemClickListener(mBandSelectionHandler);

        LoadBandList();
    }

    private AdapterView.OnItemClickListener mBandSelectionHandler =
            new AdapterView.OnItemClickListener () {
                CARAPI OnItemClick(AdapterView parent, View v,
                        Int32 position, Int64 id) {

                    GetWindow()->SetFeatureInt(
                            Window.FEATURE_INDETERMINATE_PROGRESS,
                            Window.PROGRESS_VISIBILITY_ON);

                    mTargetBand = (BandListItem) parent->GetAdapter()->GetItem(position);

                    if (DBG) Log("Select band : " + mTargetBand->ToString());

                    Message msg =
                            mHandler->ObtainMessage(EVENT_BAND_SELECTION_DONE);
                    mPhone->SetBandMode(mTargetBand->GetBand(), msg);
                }
            };

    static private class BandListItem {
        private Int32 mBandMode = Phone.BM_UNSPECIFIED;

        public BandListItem(Int32 bm) {
            mBandMode = bm;
        }

        public Int32 GetBand() {
            return mBandMode;
        }

        CARAPI ToString(
        /* [out] */ String* str)
    {
            if (mBandMode >= BAND_NAMES.length) return "Band mode " + mBandMode;
            return BAND_NAMES[mBandMode];
        }
    }

    private void LoadBandList() {
        String str = GetString(R::string::band_mode_loading);

        if (DBG) Log(str);


        //ProgressDialog->Show(this, NULL, str, TRUE, TRUE, NULL);
        mProgressPanel = new AlertDialog->Builder(this)
            .SetMessage(str)
            .Show();

        Message msg = mHandler->ObtainMessage(EVENT_BAND_SCAN_COMPLETED);
        mPhone->QueryAvailableBandMode(msg);

    }

    private void BandListLoaded(AsyncResult result) {
        if (DBG) Log("network list loaded");

        if (mProgressPanel != NULL) mProgressPanel->Dismiss();

        ClearList();

        Boolean addBandSuccess = FALSE;
        BandListItem item;

        if (result.result != NULL) {
            Int32 bands[] = (Int32[])result.result;
            Int32 size = bands[0];

            if (size > 0) {
                for (Int32 i=1; i<size; i++) {
                    item = new BandListItem(bands[i]);
                    mBandListAdapter->Add(item);
                    if (DBG) Log("Add " + item->ToString());
                }
                addBandSuccess = TRUE;
            }
        }

        if (addBandSuccess == FALSE) {
            if (DBG) Log("Error in query, add default list");
            for (Int32 i=0; i<Phone.BM_BOUNDARY; i++) {
                item = new BandListItem(i);
                mBandListAdapter->Add(item);
                if (DBG) Log("Add default " + item->ToString());
            }
        }
        mBandList->RequestFocus();
    }

    private void DisplayBandSelectionResult(Throwable ex) {
        String status = GetString(R::string::band_mode_set)
                +" [" + mTargetBand->ToString() + "] ";

        if (ex != NULL) {
            status = status + GetString(R::string::band_mode_failed);
        } else {
            status = status + GetString(R::string::band_mode_succeeded);
        }

        mProgressPanel = new AlertDialog->Builder(this)
            .SetMessage(status)
            .SetPositiveButton(android.R::string::ok, NULL).Show();
    }

    private void ClearList() {
        While(mBandListAdapter->GetCount() > 0) {
            mBandListAdapter->Remove(
                    mBandListAdapter->GetItem(0));
        }
    }

    private void Log(String msg) {
        Logger::D(LOG_TAG, "[BandsList] " + msg);
    }

    private Handler mHandler = new Handler() {
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar;
            switch (msg.what) {
                case EVENT_BAND_SCAN_COMPLETED:
                    ar = (AsyncResult) msg.obj;

                    BandListLoaded(ar);
                    break;

                case EVENT_BAND_SELECTION_DONE:
                    ar = (AsyncResult) msg.obj;

                    GetWindow()->SetFeatureInt(
                            Window.FEATURE_INDETERMINATE_PROGRESS,
                            Window.PROGRESS_VISIBILITY_OFF);

                    if (!IsFinishing()) {
                        DisplayBandSelectionResult(ar.exception);
                    }
                    break;
            }
        }
    };


}
