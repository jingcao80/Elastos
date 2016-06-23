module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    //member of CAlbumActivity

    //private static const
    var TAG = "CAlbumActivity";
    var MSG_IMAGE_LOADED = 0;
    var MSG_LOAD_ALBUM_VIEW = 1;

    //private
    var mShowAlbum = false;

    var mMyListener;
    var mAboutButton;
    var mCheckAll;
    // var mDetailButton;
    var mDetailLayout;
    var mDeleteButton;
    var mPopupButton;
    var mListView;
    var mGridView;
    var mEmptyView;
    var mAlbumPopupWindow;
    var mSimpleAdapter;
    var mArrayAdapter;
    var mHandler;
    var mProgressBar;
    var mAlbumEntryList;
    var mMyHandler;

    //public class
    var sFileName = '/data/temp/node/JSGallery/data/DataSourceHelper.js';
    var DataSourceHelper = require(sFileName)(aoElastos);

    sFileName = '/data/temp/node/JSGallery/data/AsyncImageLoader.js';
    var AsyncImageLoader = require(sFileName)(aoElastos);

    var _SetFolerLayoutListener = function (folderLayout) {
        folderLayout.SetOnTouchListener({
            OnTouch : function (aoView, aoEvent, out_abResult) {
                var IMotionEvent__ACTION_DOWN = 0;
                var IMotionEvent__ACTION_UP = 1;

                var action = aoEvent.GetAction();
                var viewParent = aoView.GetParent();
                var itemLayout = viewParent;

                if (0 == action) {
                    itemLayout.SetBackgroundResource(R.drawable.tools_bg_d);
                }
                else {
                    var res = oActivity.GetResources();
                    var bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable", res, null);
                    itemLayout.SetBackgroundDrawable(bitmapDrawable);

                    var viewParent2 = viewParent.GetParentEx();
                    var adapterView = viewParent2;
                    var index = adapterView.GetPositionForView(aoView);

                    if (IMotionEvent__ACTION_UP == action) {
                        var entry = mAlbumEntryList[index];
                        var sourcePath = entry.sourcePath;
                        var lastIndex = sourcePath.lastIndexOf(DataSourceHelper.PATH_SPLITE);
                        var folderPath = sourcePath.substring(0,lastIndex);

                        var intent = Droid_New("Elastos.Droid.Content.CIntent");
                        intent.SetClassNameEx("JSGallery", "JSGallery.CBrowserActivity");
                        intent.PutStringExtra(DataSourceHelper.SOURCE_PATH, folderPath);
                        intent.PutStringExtra(DataSourceHelper.SOURCE_DESC, entry.desc);

                        var ec = oActivity.StartActivity(intent)
                        if (ec) {
                            elog("OnClick()---StartActivity CBrowserActivity failed!");
                        }
                    }
                }

                out_abResult.data = true;
            },  //OnTouch
        });
    };  //_SetFolerLayoutListener

    var _GetSimpleAdapter = function (aoInObject) {
        elog('==================_GetSimpleAdapter.begin=============');

        elog('==================_GetSimpleAdapter.begin.0.0============='+typeof(aoInObject));

// AutoPtr<ISimpleAdapter> CAlbumActivity::GetSimpleAdapter()
// {
//     Logger::D(TAG, "GetSimpleAdapter()-----");
//     AutoPtr<ISimpleAdapter> simpleAdapter
        var simpleAdapter;

        //simpleAdapter = aoInObject;

        elog('==================_GetSimpleAdapter.begin.0.1============='+typeof(simpleAdapter));


//     if (!mAlbumEntryList.IsEmpty()) {
        if (mAlbumEntryList.length > 0) {

//if(0==1) {

//         Logger::D(TAG, "GetSimpleAdapter()---AlbumEntry count: %d", mAlbumEntryList.GetSize());
//         AutoPtr<IObjectContainer> dataList;
//         CParcelableObjectContainer::New((IObjectContainer**)&dataList);
//            var dataList = Droid_New('Elastos.Droid.Utility.CParcelableObjectContainer');

    // class CArrayList {
    //     constructor();

    //     constructor(
    //         [in] Int32 capacity);

    //     constructor(
    //         [in] ICollection* c);

    //     interface IArrayList;
    //     interface IList;
    //     interface ICollection;
    //     interface IIterable;
    //     interface ICloneable;
    //     interface ISerializable;
    //     interface IRandomAccess;
    // }

            var dataList = Core_New("Elastos.Utility.CArrayList");

            elog('==================_GetSimpleAdapter.begin=====1========');

//         AutoPtr<IObjectStringMap> map;
//         AutoPtr<IBoolean> boolValue;
//         AutoPtr<AlbumEntry> entry;
//         AutoPtr<ICharSequence> cs;

//         String key1("cover");
//         String key2("count");
//         String key3("checkbox");
//         String key4("name");
//         String key5("folderlayout");
            var key1 = "cover";
            var key2 = "count";
            var key3 = "checkbox";
            var key4 = "name";
            var key5 = "folderlayout";

            elog('==================_GetSimpleAdapter.begin=====1.1========len:'+mAlbumEntryList.length);

//         List< AutoPtr<AlbumEntry> >::Iterator it = mAlbumEntryList.Begin();
//         for (Int32 i = 0; it != mAlbumEntryList.End(); ++it, ++i) {
            for (var i=0, im=mAlbumEntryList.length; i<im; i++) {

                elog('==================_GetSimpleAdapter.begin=====1.1.0========i:'+i+'==url:'+JSON.stringify(mAlbumEntryList[i]));

//             entry = *it;
//             map = NULL;
//             CObjectStringMap::New((IObjectStringMap**)&map);
                //var entry = mAlbumEntryList[i];
                var entry = mAlbumEntryList[i];
                elog('==================_GetSimpleAdapter.begin=====1.1.1========');
                //var map = Core_New('Elastos.Utility.CObjectStringMap');
                var map = Core_New('Elastos.Utility.CHashMap');
                elog('==================_GetSimpleAdapter.begin=====1.1.2========');

//             cs = NULL;
//             CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
//             map->Put(key1, cs);
                //map.Put( key1, CString(entry.sourcePath) );
                map.Put( CString(key1), CString(entry.sourcePath) );
                elog('==================_GetSimpleAdapter.begin=====1.1.3========');

//             cs = NULL;
//             CStringWrapper::New(entry->num, (ICharSequence**)&cs);
//             map->Put(key2, cs);
                //map.Put( key2, CString(entry.num+'') );
                map.Put( CString(key2), CString(entry.num+'') );
                elog('==================_GetSimpleAdapter.begin=====1.1.4========');

//             boolValue = NULL;
//             CBoolean::New(FALSE, (IBoolean**)&boolValue);
//             map->Put(key3, boolValue);
                //map.Put( key3, Core_New('Elastos.Core.CBoolean', false) );
                map.Put( CString(key3), Core_New('Elastos.Core.CBoolean', false) );
                elog('==================_GetSimpleAdapter.begin=====1.1.5========');

//             cs = NULL;
//             CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
//             map->Put(key4, cs);
                //map.Put( key4, CString(entry.desc) );
                map.Put( CString(key4), CString(entry.desc) );
                elog('==================_GetSimpleAdapter.begin=====1.1.6========');

//             dataList->Add(map);
                dataList.Add(map);
                elog('==================_GetSimpleAdapter.begin=====1.1.7========');

//             // Logger::D(TAG, " > album %d, path:%s, num:%s, desc:%s",
//             //     i, entry->sourcePath.string(), entry->num.string(), entry->desc.string());
//         }
            }

            //return dataList;

            elog('==================_GetSimpleAdapter.begin=====2========');

//         AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(5);
//         (*from)[0] = key1;
//         (*from)[1] = key2;
//         (*from)[2] = key3;
//         (*from)[3] = key4;
//         (*from)[4] = key5;
            var from = [key1, key2, key3, key4, key5];

//         AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(5);
//         (*to)[0] = R::id::album_set_cover;
//         (*to)[1] = R::id::album_count;
//         (*to)[2] = R::id::album_set_check;
//         (*to)[3] = R::id::album_set_desc;
//         (*to)[4] = R::id::album_cover_layout;
            var to = [
                R.id.album_set_cover,
                R.id.album_count,
                R.id.album_set_check,
                R.id.album_set_desc,
                R.id.album_cover_layout,
            ];

//         CSimpleAdapter::New(this, dataList, R::layout::album_item, from, to, (ISimpleAdapter**)&simpleAdapter);
//         AutoPtr<MyViewBinder> myViewBinder = new MyViewBinder(this);
//         simpleAdapter->SetViewBinder(ISimpleAdapterViewBinder::Probe(myViewBinder));

    // [local, deprecated]
    // class CSimpleAdapter {
    //     constructor(
    //         [in] IContext* ctx,
    //         [in] IObjectContainer* data,
    //         [in] Int32 resource,
    //         [in] ArrayOf<String>* from,
    //         [in] ArrayOf<Int32>* to);

    //     interface ISimpleAdapter;
    //     interface IFilterable;
    //     interface ISpinnerAdapter;
    // }

//}   //if(0==1)

            elog('==================ssssssssssssssssssssssssssssssssss========');

    // [local, deprecated]
    // class CSimpleAdapter {
    //     constructor(
    //         [in] IContext* ctx,
    //         [in] IList* data,
    //         [in] Int32 resource,
    //         [in] ArrayOf<String>* from,
    //         [in] ArrayOf<Int32>* to);

    //     interface ISimpleAdapter;
    //     interface IBaseAdapter;
    //     interface IListAdapter;
    //     interface ISpinnerAdapter;
    //     interface IAdapter;
    //     interface IFilterable;
    // }

            simpleAdapter = Droid_New('Elastos.Droid.Widget.CSimpleAdapter', oActivity, dataList, R.layout.album_item, from, to);

            elog('==================tttttttttttttttttttttttttttttttttt========');

//aa.bb();    //debug break

            elog('==================_GetSimpleAdapter.begin=====3.0========'+typeof(simpleAdapter));

            //simpleAdapter = aoInObject;

            //return simpleAdapter;

            //var myViewBinder = Droid_new('MyViewBinder', oActivity);
            //simpleAdapter.SetViewBinder(myViewBinder);

            elog('==================_GetSimpleAdapter.begin=====3.1========'+typeof(simpleAdapter));

            simpleAdapter.SetViewBinder({

                SetViewValue : function (aoView, aoData, asTextRepresentation, out_abResult) {
                    elog('==================SetViewValue.begin=============asTextRepresentation:'+asTextRepresentation);

                    out_abResult.data = false;

                    var viewType = "";
                    if (typeof(aoView.SetOnTouchListener) == 'function') viewType += " LinearLayout";
                    if (typeof(aoView.SetImageResource) == 'function') viewType += " ImageView";
                    elog('==================SetViewValue.viewtype=============' + viewType);

                    //if(typeof(aoView.SetImageResource) == 'function') {
                    //if (false) {
                    if (viewType.indexOf("ImageView")>-1) {
                        elog('==================SetViewValue.ImageView.begin=============');

                        //var myLoadImage = new MyLoadImageCallback(oActivity);

                        var aoImageView = aoView;

                        var myLoadImage = {
                            ImageLoaded : function(aoImageDrawable, aoImageView){
                                elog('==================ImageLoaded.begin=============');

                                //MyImageLoaded(aoImageDrawable, aoImageView);
                                aoImageView.SetImageDrawable(aoImageDrawable);

    // AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    // args->mArg1 = imageDrawable;
    // args->mArg2 = imageView;

    // AutoPtr<IMessage> msg;
    // mHost->mMyHandler->ObtainMessageEx(CAlbumActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    // Boolean result;
    // return mHost->mMyHandler->SendMessage(msg, &result);

                                elog('==================ImageLoaded.end=============');
                            },
                        };

                        elog('==================SetViewValue.ImageView.begin======1=======');

                        var drawable = AsyncImageLoader.LoadDrawable(asTextRepresentation, false, aoImageView, myLoadImage);
                        elog('==================SetViewValue.ImageView.begin======2=======');
                        if (drawable) {
                            aoImageView.SetImageDrawable(drawable);
                        }
                        else {
                            aoImageView.SetImageResource(R.color.divide_line_bg);
                        }

                        out_abResult.data = true;
                    }
                    else if (typeof(aoView.SetOnTouchListener) == 'function') {  //ILinearLayout
                        elog('==================SetViewValue.LinearLayout.begin=============');

                        _SetFolerLayoutListener(aoView);
                        // aoView.SetOnTouchListener({
                        //     //(IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener))
                        //     //CAlbumActivity::MyListener::OnTouch
                        //     OnTouch : function(aoView, aoEvent, out_abResult) {
                        //         elog('==================LinearLayout.OnTouch.begin=============');
                        //     }
                        // });

                        out_abResult.data = false;
                        elog('==================SetViewValue.LinearLayout.end=============');
                    }

                    elog('==================SetViewValue.end=============');
                },
            });
        }

        elog('==================_GetSimpleAdapter.end=============');

        return simpleAdapter;
    };

    var _LoadAlbumView = function () {

        var IView_VISIBLE = 0x00000000;
        var IView_INVISIBLE = 0x00000004;
        var IView_GONE = 0x00000008;

        elog("========_LoadAlbumView====0====")

        mProgressBar.SetVisibility(IView_GONE);

        elog("========_LoadAlbumView====1====")

        mSimpleAdapter = _GetSimpleAdapter();

        elog("========_LoadAlbumView====2====")

        if (mSimpleAdapter) {
            elog("========_LoadAlbumView====2.1====")
            mGridView.SetVisibility(IView_VISIBLE);
            //mGridView.SetAdapter(IAdapter::Probe(mSimpleAdapter));
            mGridView.SetAdapter(mSimpleAdapter);
            // mSimpleAdapter->NotifyDataSetChanged();
        }
        else {
            elog("========_LoadAlbumView====2.2====")

            elog("LoadAlbumView()----mSimpleAdapter is empty");
            mEmptyView.SetVisibility(IView_VISIBLE);
            mGridView.SetVisibility(IView_GONE);
        }

        elog("========_LoadAlbumView====3====")

        // case R::id::album_btn_delete: {
        //     Logger::D(TAG, "OnClick()---album_btn_delete");
        //     break;
        // }

        mDeleteButton.SetOnClickListener( (function(){
            return {
                OnClick : function (aoView) {
                    elog("=========================mDeleteButton.OnClick.begin=============================");
                },
            };
        })() );

        mDetailLayout.SetOnClickListener( (function(){
            return {
                OnClick : function (aoView) {
                    elog("=========================mDetailLayout.OnClick.begin=============================");
                },
            };
        })() );

        mCheckAll.SetOnClickListener( (function(){
            return {
                OnClick : function (aoView) {
                    elog("=========================mCheckAll.OnClick.begin=============================");
                },
            };
        })() );

        mPopupButton.SetOnClickListener( (function(){
            function OpenAlbumPopupWindow(aoView){
                var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";
                var inflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

                var layout = inflater.Inflate(R.layout.album_popup_dialog, null);

                var popupAlbum = layout.FindViewById(R.id.pop_album);
                popupAlbum.SetOnClickListener({
                    OnClick : function (aoView) {
                        CloseAlbumPopupWindow();
                    },
                });

                var popupTime = layout.FindViewById(R.id.pop_time);
                popupTime.SetOnClickListener({
                    OnClick : function (aoView) {
                        CloseAlbumPopupWindow();
                    },
                });

                var popupLocation = layout.FindViewById(R.id.pop_location);
                popupLocation.SetOnClickListener({
                    OnClick : function (aoView) {
                        CloseAlbumPopupWindow();
                    },
                });

                if (!mAlbumPopupWindow) {
                    mAlbumPopupWindow = Droid_New('Elastos.Droid.Widget.CPopupWindow', layout, 80, 93, true);
                    mAlbumPopupWindow.SetTouchable(true);
                    mAlbumPopupWindow.SetOutsideTouchable(true);
                }

                var res = oActivity.GetResources();
                var bitmapDrawable = Droid_New('Elastos.Droid.Graphics.Drawable.CBitmapDrawable');
                mAlbumPopupWindow.SetBackgroundDrawable(bitmapDrawable);
                mAlbumPopupWindow.SetOnDismissListener({
                    OnDismiss : function (aoView) {
                        elog('================mAlbumPopupWindow.OnDismiss=================');
                    },
                });
                mAlbumPopupWindow.ShowAsDropDown(aoView);
                mShowAlbum = true;
            };
            function CloseAlbumPopupWindow(){
                if (mAlbumPopupWindow && mAlbumPopupWindow.IsShowing()) {
                    mAlbumPopupWindow.Dismiss();
                    mShowAlbum = false;
                }
            };

            return {
                OnClick : function (aoView) {
                    if (mShowAlbum) {
                        CloseAlbumPopupWindow();
                    }
                    else {
                        OpenAlbumPopupWindow(aoView);
                    }
                },
            };
        })() );

        mAboutButton.SetOnClickListener({
            OnClick : function (aoView) {
                elog('================mAboutButton.OnClick.begin=======1==========');

                var intent = Droid_New('Elastos.Droid.Content.CIntent');
                intent.SetClassNameEx("JSGallery", "JSGallery.CAboutActivity");
                var status = oActivity.StartActivity(intent);
            }
        });
    };

    var LoadImageInfoRunnable_Run = function () {

        elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====');

        var pathMap = DataSourceHelper.GetImageFileMap();

        elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====0====');

        var pathList = DataSourceHelper.GetImagePathList();

        elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====1====');

        mAlbumEntryList = [];

        var fileList, entry, path;
        for (var i=0, im=pathList.length; i<im; i++) {
            elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====2====');
            path = pathList[i];
            fileList = pathMap[path];
            elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====3===='+JSON.stringify(fileList));

            entry = {};
            entry.sourcePath = [
                path,
                //fileList.GetFront()
                fileList[0]
            ].join(DataSourceHelper.PATH_SPLITE);
            elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====4====');

            //var lastIndex = path.LastIndexOf(DataSourceHelper.PATH_SPLITE);
            //entry.desc = path.slice(lastIndex + 1);
            entry.desc = path.split(DataSourceHelper.PATH_SPLITE).pop();
            entry.num = fileList.length;
            mAlbumEntryList.push(entry);
            elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====5====');
        }

//elog('====jso_activity_cb====LoadImageInfoRunnable_Run.begin====6==== typeof oHandler:' + typeof oHandler);
//var aa = [];
//for (var pp in oHandler) aa.push(pp);
//elog('====oHandler methods:[' + aa.join('][') + ']');

        var result = oHandler.SendEmptyMessage(MSG_LOAD_ALBUM_VIEW);

        elog('====jso_activity_cb====LoadImageInfoRunnable_Run.end====');

        //_LoadAlbumView();
    };

//----------------------------------------------------------

    function _LoadImageInfos () {
        elog('========DataSourceHelper::LoadImageInfos.begin====');

        //to be run in new thread
        LoadImageInfoRunnable_Run();

    // Logger::D(TAG, "LoadImageInfos()----");

    // AutoPtr<IWeakReference> weakHost;
    // GetWeakReference((IWeakReference**)&weakHost);

    // AutoPtr<IThread> loadThread;
    // AutoPtr<LoadImageInfoRunnable> loadRunnable = new LoadImageInfoRunnable(weakHost);
    // CThread::New(IRunnable::Probe(loadRunnable), (IThread**)&loadThread);
    // assert(loadThread != NULL);
    // loadThread->Start();


    };


    var _CSimpleAdapterParams = {
        ctx : oActivity,
        data : null,
        resource : R.layout.album_item,
        from : null,
        to : null,
        viewBinder : null,
    };

    //========CAlbumActivity::MyViewBinder::SetViewValue========
    var _imageView;    //SetViewValue
    var _textRepresentation;
    var _myLoadImage;

    return {
        OnCreate:function(aoContext){
            elog('====jso_activity_cb====OnCreate.begin====0====');

            oActivity.SetContentView(R.layout.activity_album);

            elog('====jso_activity_cb====OnCreate.begin====1====');

            // Init image file map.
            DataSourceHelper.SetRootPath(DataSourceHelper.ROOT_PATH);

            elog('====jso_activity_cb====OnCreate.begin====2====');

            mPopupButton = oActivity.FindViewById(R.id.album_btn_popup);
            mGridView = oActivity.FindViewById(R.id.album_gridview);
            mEmptyView = oActivity.FindViewById(R.id.album_empty);
            mAboutButton = oActivity.FindViewById(R.id.album_about);
            mCheckAll = oActivity.FindViewById(R.id.album_check_all);
            mDetailLayout = oActivity.FindViewById(R.id.album_btn_detail);
            mDeleteButton = oActivity.FindViewById(R.id.album_btn_delete);
            mProgressBar = oActivity.FindViewById(R.id.album_progress_bar);

            elog('====jso_activity_cb====OnCreate.begin====3====');

            _LoadImageInfos();

            elog('====jso_activity_cb====OnCreate.begin====4====');

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
            elog('====jso_activity_cb====OnExchangeData.begin====times:'+aiTimes);

if (aiTimes == 0) {

        //mSimpleAdapter = _GetSimpleAdapter(aoInObject);
        mSimpleAdapter = aoInObject;

        var IView_VISIBLE = 0x00000000;
        var IView_INVISIBLE = 0x00000004;
        var IView_GONE = 0x00000008;

        if (mSimpleAdapter) {
            mGridView.SetVisibility(IView_VISIBLE);
            //mGridView.SetAdapter(IAdapter::Probe(mSimpleAdapter));
            mGridView.SetAdapter(mSimpleAdapter);
            // mSimpleAdapter->NotifyDataSetChanged();
        }
        else {
            elog("LoadAlbumView()----mSimpleAdapter is empty");
            mEmptyView.SetVisibility(IView_VISIBLE);
            mGridView.SetVisibility(IView_GONE);
        }

        aoOutObject.data = mSimpleAdapter;

}   //if (aiTimes == 0)


if (aiTimes == 2) {
                var dataList = _GetSimpleAdapter();
                _CSimpleAdapterParams.data = dataList;
                aoOutObject.data = dataList;

}   //if (aiTimes == 2)

if (aiTimes == 3) {
                _CSimpleAdapterParams.viewBinder = aoInObject;
                elog('====jso_activity_cb====OnExchangeData.viewBinder======3======');

                aoOutObject.data = oActivity;
}   //if (aiTimes == 3)
if (aiTimes == 4) {
                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.0======');

                // elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======1======');
                var o = _CSimpleAdapterParams;
                // elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======2======');

                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.1======');
                //var oCSimpleAdapter_00 = aoElastos.Test.New_CSimpleAdapter(o.ctx, o.data, o.resource, o.from, o.to);
                //var oCSimpleAdapter = aoInObject;
                // elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======3======');
                var oCSimpleAdapter = Droid_New("Elastos.Droid.Widget.CSimpleAdapter", o.ctx, o.data, o.resource, o.from, o.to);
                //var oCSimpleAdapter = aoElastos.Test.New_CSimpleAdapter(o.ctx, o.data, o.resource, o.from, o.to);
                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.2======');

                //var a=[];
                //for (var p in oCSimpleAdapter_00)a.push(p);
                //elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.2.0======'+a.join('=='));
                var a=[];
                for (var p in oCSimpleAdapter)a.push(p);
                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.2.1======'+a.join('=='));

                oCSimpleAdapter.SetViewBinder(o.viewBinder);
                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.3======');

                aoOutObject.data = oCSimpleAdapter;
                elog('====jso_activity_cb====OnExchangeData.New_CSimpleAdapter======4.4======');

                //aa = bb.cc();   //break

}   //if (aiTimes == 3)

if (aiTimes == 10) {
                var folderLayout = aoInObject;

                _SetFolerLayoutListener(folderLayout);

                // folerLayout.SetOnTouchListener(
                //     (IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener))
                // );

                aoOutObject.data = folderLayout;
}   //if (aiTimes == 10)


if (aiTimes == 11) {
                _imageView = aoInObject;
}
if (aiTimes == 12) {
                _myLoadImage = aoInObject;
}

if (aiTimes == 14) {

            elog('====jso_activity_cb====OnExchangeData.12===========1============'+typeof(drawable));

                //AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(textRepresentation, FALSE, imageView, myLoadImage);
                var oAsyncImageLoader = Droid_New("AsyncImageLoader");
                var drawable = oAsyncImageLoader.LoadDrawable(_textRepresentation, FALSE, _imageView, _myLoadImage);

                var drawable = aoInObject;
                //if ( typeof(drawable)=="object" ) {
                if (drawable) {
                    elog('====jso_activity_cb====OnExchangeData.12===========2============');
                    _imageView.SetImageDrawable(drawable);
                    elog('====jso_activity_cb====OnExchangeData.12===========3============');
                }
                else {
                    elog('====jso_activity_cb====OnExchangeData.12===========4============');
                    _imageView.SetImageResource(R.color.divide_line_bg);
                    elog('====jso_activity_cb====OnExchangeData.12===========5============');
                }
}   //if (aiTimes == 12)



            elog('====jso_activity_cb====OnExchangeData.end====');
        },  //OnExchangeData

        OnExchangeData_String : function(aiTimes ,aoInObject, aoOutObject) {
            elog('====jso_activity_cb====OnExchangeData.begin====times:'+aiTimes);

if (aiTimes == 13) {
                _textRepresentation = aoInObject;
}   //if (aiTimes == 13)

        },  //OnExchangeData_String

        OnExchangeData_LocalPtr_ArrayOf_Int32 : function(aiTimes ,aoInArray, aoOutArray) {
            elog('====jso_activity_cb====OnExchangeData_LocalPtr_ArrayOf_Int32.begin====');

if (aiTimes == 0) {
            elog('====jso_activity_cb====OnExchangeData_LocalPtr_ArrayOf_Int32.begin====aoInArray type:'+typeof(aoInArray));

            var to = [
                R.id.album_set_cover,
                R.id.album_count,
                R.id.album_set_check,
                R.id.album_set_desc,
                R.id.album_cover_layout,
            ];

            _CSimpleAdapterParams.to = to;

            aoOutArray.data = to;
}   //if (aiTimes == 0)
        },  //OnExchangeData_LocalPtr_ArrayOf_Int32

        OnExchangeData_LocalPtr_ArrayOf_String : function(aiTimes ,aoInArray, aoOutArray) {
            elog('====jso_activity_cb====OnExchangeData_LocalPtr_ArrayOf_String.begin====');
if (aiTimes == 0) {
            var from = [
                "cover",
                "count",
                "checkbox",
                "name",
                "folderlayout",
            ];

            _CSimpleAdapterParams.from = from;

            aoOutArray.data = from;
}   //if (aiTimes == 0)


        },  //OnExchangeData_LocalPtr_ArrayOf_String

        OnHandleMessage:function(aoContext, aoMessage){
            elog('====jso_activity_cb====OnHandleMessage.begin====');

            elog('====jso_activity_cb====OnHandleMessage.begin===='+typeof(aoMessage));

            var aa = [];
            for (prop in aoMessage) aa.push(prop);
            elog('====jso_activity_cb====OnHandleMessage.aoMessage.methods:===='+aa.join('--'));

            var what = aoMessage.GetWhat();

            if (what == MSG_LOAD_ALBUM_VIEW) {
                elog('====jso_activity_cb====OnHandleMessage====MSG_LOAD_ALBUM_VIEW====');
                _LoadAlbumView();
            }
            else if (what == MSG_IMAGE_LOADED) {
                elog('====jso_activity_cb====OnHandleMessage====MSG_IMAGE_LOADED====');

                var obj = aoMessage.GetObj();
                //var args = obj.Get();

    // msg->GetObj((IInterface**)&obj);

    // switch(what) {
    //     case CAlbumActivity::MSG_IMAGE_LOADED: {
    //         SomeArgs* args = (SomeArgs*)obj.Get();
    //         IDrawable* drawable = IDrawable::Probe(args->mArg1);
    //         IImageView* imageView = IImageView::Probe(args->mArg2);
    //         mHost->MyImageLoaded(drawable, imageView);
    //         args->Recycle();
    //         break;


            }
            else {
                elog('====jso_activity_cb====OnHandleMessage====MSG_OTHER====');
            }
        },
    }
};  //module.exports