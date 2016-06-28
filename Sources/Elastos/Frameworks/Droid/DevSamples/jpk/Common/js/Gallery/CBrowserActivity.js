module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    //member of CBrowserActivity

    //private static const
    var TAG = "CBrowserActivity";
    //var MSG_IMAGE_LOADED = 0;
    //var MSG_LOAD_ALBUM_VIEW = 1;

    //private
    var mShowBrowser = false;

    var mShowPicPop = false;

    var mBackButton;
    var mAlbumName;
    var mWallpaperLayout;
    var mDetailLayout;
    var mShareLayout;
    var mPopButton;
    var mGridView;

    var mPicPopupWindow;
    function ClosePicPopupWindow() {
        if (mPicPopupWindow && mPicPopupWindow.IsShowing()) {
            elog(TAG + "ClosePicPopupWindow()---");
            mPicPopupWindow.Dismiss();
            mPicPopupWindow = null;
        }
        mShowPicPop = false;
    }
    function OpenPicPopupWindow(v) {
        elog(TAG + "OpenPicPopupWindow()-----");

        var inflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);
        var layout = inflater.Inflate(R.layout.pic_popup_dialog);

        var textTime = layout.FindViewById(R.id.pic_pop_time);
        textTime.SetOnClickListener({
            OnClick : function(aoView){
                //
            }
        });

        var textLocation = oActivity.FindViewById(R.id.pic_pop_location);
        textLocation.SetOnClickListener({
            OnClick : function(aoView){
                //
            }
        });

        mPicPopupWindow = Droid_New("CPopupWindow", layout,80, 62, true);
        mPicPopupWindow.SetTouchable(true);
        mPicPopupWindow.SetOutsideTouchable(true);

        var res = aoActivity.GetResources();
        var bitmapDrawable = Droid_New("CBitmapDrawable", res);

        mPicPopupWindow.SetBackgroundDrawable(bitmapDrawable);
        mPicPopupWindow.SetOnDismissListener({
            OnDismiss : function () {
                //
            }
        })

        mPicPopupWindow.ShowAsDropDown(v);

        mShowPicPop = true;
    }

    function GetSimpleAdapter() {
        //TODO
    }

    var DataSourceHelper;

    function PictureEntry() {
        //TODO
    }

    var mPictureEntryList = []; //ArrayOf<PictureEntry>

    //----------------------------------------------------------

    return {
        OnCreate:function(aoContext){
            elog('====jso_activity_cb====OnCreate.begin====0');

            oActivity.SetContentView(R.layout.activity_browser);

            mBackButton = oActivity.FindViewById(R.id.pic_back);
            mBackButton.SetOnClickListener({
                OnClick : function(aoView) {
                    if (mShowPicPop) {
                        ClosePicPopupWindow();
                    }
                    else {
                        oActivity.OnBackPressed();
                    }
                },
            });

            mAlbumName = oActivity.FindViewById(R.id.pic_folder_name);

            mWallpaperLayout = oActivity.FindViewById(R.id.pic_btn_wallpaper);
            mWallpaperLayout.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_btn_wallpaper");
               },
            });

            mDetailLayout = oActivity.FindViewById(R.id.pic_btn_detail);
            mDetailLayout.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_btn_detail");
               },
            });

            mShareLayout = oActivity.FindViewById(R.id.pic_btn_share);
            mShareLayout.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_btn_share");
               },
            });

            mPopButton = oActivity.FindViewById(R.id.pic_btn_popup);
            mPopButton.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_btn_popup");
                    if (mShowPicPop) {
                        ClosePicPopupWindow();
                    }
                    else {
                        OpenPicPopupWindow(v);
                    }
               },
            });

            mTime = oActivity.FindViewById(R.id.pic_pop_time);
            mTime.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_pop_time");
                    ClosePicPopupWindow();
               },
            });

            mLocation = oActivity.FindViewById(R.id.pic_pop_location);
            mLocation.SetOnClickListener({
                OnClick : function(aoView) {
                    elog(TAG + "OnClick()---pic_pop_location");
                    ClosePicPopupWindow();
               },
            });

            mGridView = FindViewById(R.id.pic_gridview);

            var intent = oActivity.GetIntent();
            if (intent) {
                var path = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
                if (path.length) {
                    folderName = intent.GetStringExtra(DataSourceHelper.SOURCE_DESC);
                    var imageItems = DataSourceHelper.GetItemList(path);
                    if (imageItems instanceof Array) {
                        var entry;
                        for (var i=0,im=imageItems.leng;i<im;i++) {
                            entry = new PictureEntry();
                            entry.sourcePath = path + DataSourceHelper.PATH_SPLITE + imageItems[i];
                            entry.desc = folderName;
                            elog(TAG + " > " + i + ", path:" + entry.sourcePath + ", folderName:" + entry.desc;
                            mPictureEntryList.push(entry);
                        }
                        var cs = CString(folderName);
                        mAlbumName.SetText(cs);
                        mSimpleAdapter = GetSimpleAdapter();
                        mGridView.SetAdapter(mSimpleAdapter);
                    }
                    else {
                        elog(TAG + "imageItems is null!");
                    }
               }
            }
        },  //OnCreate
        OnStart:function(aoContext){
            elog('====jso_activity_cb====OnStart.begin====');
        },
        OnResume:function(aoContext){
            elog('====jso_activity_cb====OnResume.begin====');
        },
        OnPause:function(aoContext){
            elog('====jso_activity_cb====OnPause.begin====');
        },
        OnStop:function(aoContext){
            elog('====jso_activity_cb====OnStop.begin====');
        },
        OnDestroy:function(aoContext){
            elog('====jso_activity_cb====OnDestroy.begin====');
        },
        OnActivityResult:function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            elog('====jso_activity_cb====OnActivityResult.begin====');
        },
        OnCreateDialog:function(aoContext, aiId, out_aoDialog){
            elog('====jso_activity_cb====OnCreateDialog.begin====');
        },   //OnCreateDialog
        OnCreateContextMenu:function(aoContext, aoMenu, aoV, aoMenuInfo){
            elog('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            elog('====jso_activity_cb====CreateNavigationBar.begin====');
        },
        //OnExchangeData : function(inObject_0, inObject_1, inObject_2, inObject_3, inObject_4, outObject_0, outObject_1, outObject_2, outObject_3, outObject_4) {
        OnExchangeData : function(aiTimes ,aoInObject, aoOutObject) {
            elog('====jso_activity_cb====OnExchangeData.begin====');
        },
        OnHandleMessage:function(aoContext, aoMessage){
            elog('====jso_activity_cb====OnHandleMessage.begin====');
        },
    }
};  //module.exports