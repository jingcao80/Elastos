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

    //--------MyLoadImageCallback--------
    function MyLoadImageCallback(ao_host) {
        this.mHost = ao_host;
    };

// ECode CBrowserActivity::MyLoadImageCallback::ImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
// {
    MyLoadImageCallback.prototype.ImageLoaded(imageDrawable, imageView){
//     Logger::D(TAG, "ImageLoaded()-----");
        elog(TAG + "ImageLoaded()-----");
//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CBrowserActivity* mHost = (CBrowserActivity*)strongObj.Get();
//     if (mHost->mMyHandler == NULL) {
//         return NOERROR;
//     }

//     AutoPtr<SomeArgs> args = SomeArgs::Obtain();
//     args->mArg1 = imageDrawable;
//     args->mArg2 = imageView;

//     AutoPtr<IMessage> msg;
//     mHost->mMyHandler->ObtainMessageEx(CBrowserActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
//     Boolean result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        //message should be used
        imageView.SetImageDrawable(imageDrawable);
// }
    }   //ImageLoaded

    //--------OnTouchListener--------
    function SetOnTouchListener() {};

// ECode CBrowserActivity::MyListener::OnTouch(
//     /* [in] */ IView* v,
//     /* [in] */ IMotionEvent* event,
//     /* [out] */ Boolean* result)
// {
    SetOnTouchListener.prototype.OnTouch = function(v, event){
        var result = false;
//     VALIDATE_NOT_NULL(result)
//     Int32 action = 0;
//     AutoPtr<IView> view;
//     event->GetAction(&action);
        var action = event.GetAction();
//     v->FindViewById(R::id::pic_thumb_overlay, (IView**)&view);
//     AutoPtr<IImageView> overlayView = IImageView::Probe(view);
        var overlayView = v.FindViewById(R.id.pic_thumb_overlay);
//     if (IMotionEvent::ACTION_DOWN == action) {
        if (IMotionEvent__ACTION_DOWN == action) {
//         Logger::D(TAG, "onTouch()--ACTION_DOWN");
            elog(TAG + "onTouch()--ACTION_DOWN");
//         overlayView->SetVisibility(IView::VISIBLE);
            overlayView.SetVisibility(IView__VISIBLE);
//     }
        }
//     else {
        else {
//         Logger::D(TAG, "onTouch()--ACTION:%d", action);
            elog(TAG + "onTouch()--ACTION:" + action);
//         overlayView->SetVisibility(IView::GONE);
            overlayView.SetVisibility(IView__GONE);
//         AutoPtr<IViewParent> viewParent;
//         AutoPtr<IViewParent> viewParent2;
//         v->GetParent((IViewParent**)&viewParent);
//         viewParent->GetParentEx((IViewParent**)&viewParent2);
//         AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(viewParent2);
            var adapterView = v.GetParent();
//         Int32 index = 0;
//         adapterView->GetPositionForView(v, &index);
            var index = adapterView.GetPositionForView(v);
//         Logger::D(TAG, "onTouch()--index:%d", index);
            elog(TAG + "OnTouch()--index:" + index);
//         if (IMotionEvent::ACTION_UP == action) {
            if (IMotionEvent__ACTION_UP == action) {
//             AutoPtr<PictureEntry> entry = mHost->mPictureEntryList[index];
                var entry = mPictureEntryList[index];
//             AutoPtr<IIntent> intent;
//             CIntent::New((IIntent**)&intent);
                var intent = Droid_New("CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CPhotoActivity"));
                intent.SetClassName("Gallery", "Gallery.CPhotoActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutString(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             intent->PutInt32Extra(DataSourceHelper::SOURCE_INDEX, index);
                intent.PutInt32(DataSourceHelper.SOURCE_INDEX, index);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {mHost.StartActivity(intent)} catch(e) {
//                 Logger::E(TAG, "onTouch()--StartActivity CPhotoActivity failed!");
                    elog(TAG + "onTouch()--StartActivity CPhotoActivity failed!");
//             }
                }
//         }
            }
//     }
        }
//     *result = TRUE;
        result = true;
//     return NOERROR;
        return result;
// }
    }   //OnTouch

    //--------MyViewBinder--------
    function MyViewBinder(ao_this) {};

// ECode CBrowserActivity::MyViewBinder::SetViewValue(
//     /* [in] */ IView* view,
//     /* [in] */ IInterface* data,
//     /* [in] */ const String& textRepresentation,
//     /* [out] */ Boolean* result)
// {
    MyViewBinder.prototype.SetViewValue = function(ao_view, ao_data, as_textRepresentation){
//     // Logger::D(TAG, "MyViewBinder()---SetViewValue---");
        elog(TAG + "MyViewBinder()---SetViewValue---");
//     VALIDATE_NOT_NULL(result)
//     *result = FALSE;
        var result = false;
//     if (IRelativeLayout::Probe(view) != NULL) {
        if (_hasInterface(ao_view, "IRelativeLayout")) {
//         // Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnTouchListener");
            elog(TAG + "MyViewBinder()---SetViewValue---SetOnTouchListener");
//         AutoPtr<IRelativeLayout> thumbLayout = IRelativeLayout::Probe(view);
//         thumbLayout->SetOnTouchListener((IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener)));
            ao_view.SetOnTouchListener(new SetOnTouchListener());
//         *result = TRUE;
            result = true;
//     }
        }
//     else if (IImageView::Probe(view) != NULL) {
        else if (_hasInterface(ao_view, "IImageView")) {
//         // Logger::D(TAG, "MyViewBinder()---SetViewValue---text:%s", textRepresentation.string());
            elog(TAG + "MyViewBinder()---SetViewValue---text:" + as_textRepresentation);
//         AutoPtr<IImageView> imageView = IImageView::Probe(view);
//         AutoPtr<IWeakReference> weakHost;
//         mHost->GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
            var myLoadImage = new MyLoadImageCallback(ao_view);
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(textRepresentation, FALSE, imageView, myLoadImage);
            var drawable = AsyncImageLoader.LoadDrawable(as_textRepresentation, false, ao_view, myLoadImage);
//         if (drawable != NULL) {
            if (drawable) {
//             imageView->SetImageDrawable(IDrawable::Probe(drawable));
                imageView.SetImageDrawable(drawable);
//         }
            }
//         else {
            else {
//             imageView->SetImageResource(R::color::divide_line_bg);
                imageView.SetImageResource(R.color.divide_line_bg);
//         }
            }
//         *result = TRUE;
            result = true;
//     }
        }
//     //    else if (ICheckable::Probe(view) != NULL) {
//     //        Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnCheckedChangeListener");
//     //        AutoPtr<ICheckBox> checkBox = ICheckBox::Probe(view);
//     //        checkBox->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)
//     //                        (mHost->mMyListener->Probe(EIID_ICompoundButtonOnCheckedChangeListener)));
//     //        *result = FALSE;
//     //    }
//     return NOERROR;
        return result;
// }
    }   //SetViewValue

// AutoPtr<ISimpleAdapter> CBrowserActivity::GetSimpleAdapter()
// {
    function GetSimpleAdapter() {
//     Logger::D(TAG, "GetSimpleAdapter()---");
//     AutoPtr<ISimpleAdapter> simpleAdapter;
        var simpleAdapter;
//     if (!mPictureEntryList.IsEmpty()) {
        if (mPictureEntryList.length) {
//         Logger::D(TAG, "GetSimpleAdapter()---PictureEntry count: %d", mPictureEntryList.GetSize());
//         AutoPtr<IObjectContainer> dataList;
//         CParcelableObjectContainer::New((IObjectContainer**)&dataList);
            var dataList = Core_New("CParcelableObjectContainer");

//         AutoPtr<PictureEntry> entry;
            var entry;
//         String key1("image");
            var key1 = "image";
//         String key2("checkbox");
            var key2 = "checkbox";
//         String key3("thumbLayout");
            var key3 = "thumbLayout";

//         List< AutoPtr<PictureEntry> >::Iterator it = mPictureEntryList.Begin();
//         for (; it != mPictureEntryList.End(); ++it) {
            for (var i0,im=mPictureEntryList.length;i<im;i++) {
//             entry = *it;
                entry = mPictureEntryList[i];
//             AutoPtr<IObjectStringMap> map;
//             CObjectStringMap::New((IObjectStringMap**)&map);
                var map = Droid_New("CObjectStringMap");
//             AutoPtr<IFile> file;
//             CFile::New(entry->sourcePath, (IFile**)&file);
                var file = Core_New("CFile", entry.sourcePath);
//             Int64 last = 0;
//             file->LastModified(&last);
                var last = file.LastModified();
//             Logger::D(TAG, "GetSimpleAdapter()---sort--last:%lld,--path:%s", last, entry->sourcePath.string());
                elog(TAG + "GetSimpleAdapter()---sort--last:" + last + ",--path:" + entry.sourcePath);
//             AutoPtr<ICharSequence> cs;
//             CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
                var cs = Core_New("CStringWrapper", entry.sourcePath);
//             map->Put(key1, cs->Probe(EIID_ICharSequence));
                map.Put(key1, cs);
//             AutoPtr<IBoolean> boolValue;
//             CBoolean::New(FALSE, (IBoolean**)&boolValue);
                var boolValue = Core_New("CBoolean", false);
//             map->Put(key2, boolValue->Probe(EIID_IBoolean));
                map.Put(key2, boolValue);
//             dataList->Add(map);
                dataList.Add(map);
//         }
            }

//         AutoPtr< ArrayOf<String> > from = ArrayOf<String>::Alloc(3);
//         (*from)[0] = key1;
//         (*from)[1] = key2;
//         (*from)[2] = key3;
            var from = [key1, key2, key3];
//         AutoPtr< ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(3);
//         (*to)[0] = R::id::pic_thumb;
//         (*to)[1] = R::id::pic_check;
//         (*to)[2] = R::id::pic_thumb_layout;
            var to = [R.id.pic_thumb, R.id.pic_check, R.id.pic_thumb_layout];
//         CSimpleAdapter::New(this, dataList, R::layout::pic_item, from, to, (ISimpleAdapter**)&simpleAdapter);
            simpleAdapter = Droid_New("CSimpleAdapter", oActivity, dataList, R.layout.pic_item, from, to);
//         AutoPtr<MyViewBinder> myViewBinder = new MyViewBinder(this);
            var myViewBinder = new MyViewBinder(oActivity);
//         simpleAdapter->SetViewBinder(ISimpleAdapterViewBinder::Probe(myViewBinder));
            simpleAdapter.SetViewBinder(myViewBinder);
//     }
        }

//     return simpleAdapter;
        return simpleAdapter;
// }
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