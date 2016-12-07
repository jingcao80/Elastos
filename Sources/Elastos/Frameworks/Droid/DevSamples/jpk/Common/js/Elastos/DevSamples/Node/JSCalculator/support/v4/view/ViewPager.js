elog("====ViewPager.js====begin====");

module.exports = function(aoElastos, aoActivity){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;


//--------common definition----end----

    //visible mask:0x0000000C====0/4/8/C
    var IView__VISIBILITY_MASK = 0x0000000C;
    var IView__VISIBLE = 0x00000000;
    var IView__INVISIBLE = 0x00000004;
    var IView__GONE = 0x00000008;

    var IView__FOCUS_FORWARD = 0x00000002;
    var IView__FOCUS_LEFT = 0x00000011;
    var IView__FOCUS_RIGHT = 0x00000042;

    var LayoutParams__FILL_PARENT = -1;
    var LayoutParams__MATCH_PARENT = -1;
    var LayoutParams__WRAP_CONTENT = -2;

    var ViewGroup__FLAG_CLIP_CHILDREN = 0x1;
    var ViewGroup__FLAG_INVALIDATE_REQUIRED  = 0x4;
    var ViewGroup__FLAG_ANIMATION_DONE = 0x10;
    var ViewGroup__FLAG_OPTIMIZE_INVALIDATE = 0x80;
    var ViewGroup__FLAG_CLEAR_TRANSFORMATION = 0x100;
    var ViewGroup__FLAG_ALWAYS_DRAWN_WITH_CACHE = 0x4000;
    var ViewGroup__FLAG_CHILDREN_DRAWN_WITH_CACHE = 0x8000;

    var IView__SOUND_EFFECTS_ENABLED = 0x08000000;
    var IView__HAPTIC_FEEDBACK_ENABLED = 0x10000000;

    var _mViewFlags = IView__SOUND_EFFECTS_ENABLED | IView__HAPTIC_FEEDBACK_ENABLED;


//--------.java----begin----

// /*
//  * Copyright (C) 2011 The Android Open Source Project
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *      http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// package android.support.v4.view;

// import android.content.Context;
// import android.content.res.Resources;
// import android.content.res.TypedArray;
// import android.database.DataSetObserver;
    class DataSetObserver {
        constructor() {
            // elog("====DataSetObserver::constructor====begin====");
            // var oRet = Droid_New("Elastos.Droid.Database.IDataSetObserver");
            // elog("====DataSetObserver::constructor====end====");
            // return oRet;
        }

        GetCallbackInterfaceInfo(result) {
            elog("====DataSetObserver::GetCallbackInterfaceInfo====begin====");
            var oMuduleInfo = aoElastos.Droid.Get();
            elog("====DataSetObserver::GetCallbackInterfaceInfo====0====");
            var oInterfaceInfo = oMuduleInfo.GetInterfaceInfo("Elastos.Droid.Database.IDataSetObserver");
            elog("====DataSetObserver::GetCallbackInterfaceInfo====1====");
            var oRet = oInterfaceInfo.GetInternalObject();
            elog("====DataSetObserver::GetCallbackInterfaceInfo====2====");
            result.data = oRet;
            elog("====DataSetObserver::GetCallbackInterfaceInfo====end====");
        }
        GetCallbackInterfaceName(result) {
            result.data = "Elastos.Droid.Database.IDataSetObserver";
        }
    };
// import android.graphics.Canvas;
// import android.graphics.Rect;
    class Rect {};   // = Elastos.Droid.Graphics.CRect;
// import android.graphics.drawable.Drawable;
// import android.os.Build;
// import android.os.Bundle;
// import android.os.Parcel;
// import android.os.Parcelable;
// import android.os.SystemClock;
// import android.support.annotation.DrawableRes;
// import android.support.v4.os.ParcelableCompat;
// import android.support.v4.os.ParcelableCompatCreatorCallbacks;
// import android.support.v4.view.accessibility.AccessibilityEventCompat;
// import android.support.v4.view.accessibility.AccessibilityNodeInfoCompat;
// import android.support.v4.view.accessibility.AccessibilityRecordCompat;
// import android.support.v4.widget.EdgeEffectCompat;

// import android.util.AttributeSet;
// import android.util.Log;
// import android.view.FocusFinder;
// import android.view.Gravity;
    var __Gravity = {};    //Elastos::Droid::View::IGravity


        __Gravity.NO_GRAVITY = 0x0000;
        __Gravity.AXIS_SPECIFIED = 0x0001;
        __Gravity.AXIS_PULL_BEFORE = 0x0002;
        __Gravity.AXIS_PULL_AFTER = 0x0004;
        __Gravity.AXIS_CLIP = 0x0008;
        __Gravity.AXIS_X_SHIFT = 0;
        __Gravity.AXIS_Y_SHIFT = 4;
        __Gravity.TOP = 0x0003 << __Gravity.AXIS_Y_SHIFT;
        __Gravity.BOTTOM = 0x0005 << __Gravity.AXIS_Y_SHIFT;
        __Gravity.LEFT = 0x0003 << __Gravity.AXIS_X_SHIFT;
        __Gravity.RIGHT = 0x0005 << __Gravity.AXIS_X_SHIFT;
        __Gravity.CENTER_VERTICAL = __Gravity.AXIS_SPECIFIED<<__Gravity.AXIS_Y_SHIFT;
        __Gravity.FILL_VERTICAL = __Gravity.TOP|__Gravity.BOTTOM;
        __Gravity.CENTER_HORIZONTAL = __Gravity.AXIS_SPECIFIED<<__Gravity.AXIS_X_SHIFT;
        __Gravity.FILL_HORIZONTAL = __Gravity.LEFT|__Gravity.RIGHT;
        __Gravity.CENTER = __Gravity.CENTER_VERTICAL|__Gravity.CENTER_HORIZONTAL;
        __Gravity.FILL = __Gravity.FILL_VERTICAL|__Gravity.FILL_HORIZONTAL;
        __Gravity.CLIP_VERTICAL = __Gravity.AXIS_CLIP<<__Gravity.AXIS_Y_SHIFT;
        __Gravity.CLIP_HORIZONTAL = __Gravity.AXIS_CLIP<<__Gravity.AXIS_X_SHIFT;
        __Gravity.RELATIVE_LAYOUT_DIRECTION = 0x00800000;
        __Gravity.HORIZONTAL_GRAVITY_MASK = 0x0007 << __Gravity.AXIS_X_SHIFT;
        __Gravity.VERTICAL_GRAVITY_MASK = 0x0007 << __Gravity.AXIS_Y_SHIFT;
        __Gravity.DISPLAY_CLIP_VERTICAL = 0x10000000;
        __Gravity.DISPLAY_CLIP_HORIZONTAL = 0x01000000;
        __Gravity.START = __Gravity.RELATIVE_LAYOUT_DIRECTION | __Gravity.LEFT;
        __Gravity.END = __Gravity.RELATIVE_LAYOUT_DIRECTION | __Gravity.RIGHT;

    var Gravity = __Gravity;

// import android.view.KeyEvent;
// import android.view.MotionEvent;
// import android.view.SoundEffectConstants;
// import android.view.VelocityTracker;
// import android.view.View;
// import android.view.ViewConfiguration;
    var ViewConfiguration = new Proxy(
        Droid_New("Elastos.Droid.View.CViewConfigurationHelper"),
        {
            get: function(target, property){
                var ret = null;
                if (property == "Get") {
                    ret = function(o) {
                        return target[property](o);
                    }
                }
                return ret;
            }
        }
    );

    var gggg = {};

// import android.view.ViewGroup;
class ViewGroup {
    constructor() {
        elog("====ViewGroup::constructor====begin==8888==");
        this.TAG = "instance of ViewGroup";
        ViewGroup.TAG = "class of ViewGroup";
    }

    OnCreate(_this, context, attrs) {
        elog("====ViewGroup::OnCreate====begin==9999==");
        _this._constructor(context, attrs);
    }

    static get LayoutParams() {
        elog("====ViewGroup::LayoutParams====begin====");
        if (this._LayoutParams) return this._LayoutParams;
        this._LayoutParams = class LayoutParams {

            // get _obj(){
            //     elog("====ViewGroup::LayoutParams::_obj====get====");
            //     return this.__obj;
            // }
            // set _obj(v){
            //     elog("====ViewGroup::LayoutParams::_obj====set====");
            //     CObject.showMethods(v);
            //     this.__obj = v;
            //     return v;
            // }

            constructor(_this, context, attrs) {
                elog("====ViewGroup::LayoutParams::constructor====begin====this:"+typeof this);


                var _obj;

                var typeName = attrs.getClass?attrs.getClass().GetName():typeof attrs;
                //if (typeName == "function") typeName = typeName.name;

                elog("====ViewGroup::LayoutParams::constructor====0====typeName:"+typeName);

                if (typeName == "number") {
elog("====ssss1====");
                    //_obj = Droid_New("Elastos.Droid.View.CViewGroupLayoutParams", context, attrs);
                    _obj = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", context, attrs);
elog("====ssss2====");

                    //Assert(0);
                }
                else if (typeName == "IAttributeSet") {
elog("====ssss3====");

                    //_obj = Droid_New("Elastos.Droid.View.CViewGroupLayoutParams", context, attrs);
                    _obj = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", context, attrs);
                    //oRet = Droid_New("Elastos.Droid.View.CViewGroupLayoutParams");
                    //_obj = _this._GenerateLayoutParams(attrs);

elog("====ssss4====");

    // VALIDATE_NOT_NULL(params);
    // AutoPtr<IViewGroupLayoutParams> temp;
    // AutoPtr<IContext> ctx;
    // GetContext((IContext**)&ctx);
    // CViewGroupLayoutParams::New(ctx, attrs, (IViewGroupLayoutParams**)&temp);
    // *params = temp;
    // REFCOUNT_ADD(*params);
    // return NOERROR;



                }
                else if (typeName == "CViewGroupLayoutParams") {
elog("====ssss5====");

                    //oRet = Droid_New("Elastos.Droid.View.CViewGroupLayoutParams", context, attrs);
                    //oRet = Droid_New("Elastos.Droid.View.CViewGroupLayoutParams");
                    _obj = _this._GenerateLayoutParams(attrs);

elog("====ssss6====");
                }
                else {
                    ss.tt();Assert(0);
                    _obj = _this._GenerateLayoutParams(attrs);
                }

                elog("====ViewGroup::LayoutParams::constructor====1====");


                //CObject.showMethods(_obj);

                //this._obj = _obj;
                //oRet._obj = _obj;

                var oRet;


    //elog("====ViewGroup::LayoutParams::constructor====fail====typeName:"+typeName);
    //CObject.showMethods(attrs);

    var im = attrs.GetAttributeCount();
    elog("========cnt:"+im);

    var aa = [];
    for (var i=0;i<im;i++) {
        var nn = attrs.GetAttributeName(i);
        var vv = attrs.GetAttributeValue(i);
        aa.push("==cnt:"+i+"==name:"+nn+"==value:"+vv+"==");
    }
    elog("====ViewGroup::LayoutParams::constructor====fail====typeName:"+typeName);
    for (var i=0;i<im;i++) {
        elog(aa[i]);
    }
    //ss.tt();Assert(0);

if (!_obj) {
    elog("====ViewGroup::LayoutParams::constructor====kuang====");
    s.t();
    Assert(0);
}


if(_obj._obj) {
    elog("====have been proxy!====");
    //Assert(0);
    oRet = oRet;
}
else {

                var _prop = {
                    isDecor:false,
                    gravity:0,
                    widthFactor:0,
                    needsMeasure:0,
                    position:0,
                    childIndex:0,
                };

                //TODO:wrapped with proxy
                //oRet = new Proxy(this, {
                //oRet = new Proxy({_obj:_obj,}, {
                oRet = new Proxy({}, {
                    set : function(target, key, value) {
                        elog("====ViewGroup::LayoutParams::Proxy::set====" + key);
                        //if (key == "view") target.view = value;
                        //target[key] = value;
                        //return Reflect.set(target, key, value);
                        //if (typeof _prop[key] != "undefined") {
                            _prop[key] = value;
                            return true;
                        //}
                    },
                    get : function(target, key){
                        elog("====ViewGroup::LayoutParams::Proxy::get====begin====" + key);

                        //CObject.showMethods(target);

                        var bTargetProp;
                        // = target.hasOwnProperty(key);
                        var aProp = ["_obj","isDecor","gravity","widthFactor","needsMeasure","position","childIndex"];

                        bTargetProp = aProp.find(n=>n==key);

                        var oGetFn;
                        if (bTargetProp) {
                            elog("====ViewGroup::LayoutParams::Proxy::get====use target===="+key +"==type:" + typeof _prop[key]);
                            //oRet = target[property];
                            //if (property == "widthFactor") return 0.3;
                            //return Reflect.get(target, property);
                            return _prop[key];
                        }
                        else {
                            elog("====ViewGroup::LayoutParams::Proxy::get==0==use proxy====");

                            //var _this = target._obj;
                            //var _this = _obj;
                            //oRet = _this[property];
                            //elog("====ViewGroup::LayoutParams::Proxy::get==1==use proxy====" + typeof _this );
                            //elog("====ViewGroup::LayoutParams::Proxy::get==2==use proxy====" + _this );

                            elog("====ViewGroup::LayoutParams::Proxy::get==1==use proxy====_this type:"+typeof _this);
                            elog("====ViewGroup::LayoutParams::Proxy::get==2==use proxy====_this type:"+typeof _this[key]);

                            //CObject.showMethods(target);
                            //CObject.showMethods(_this);

                            //var oRet_bak;
                            oGetFn = function() {
                                elog("========Proxy get====begin===="+key);
                                //elog("====typeof:" + property + " " + typeof target[property]);
                                var args = [...arguments];
                                var len = args.length;

// if(property=="GetWidth"){
//     elog("====Proxy get====GetWidth===="+args.length);
//     //ss.tt();Assert(0);
// }

                                //CObject.showMethods(this);

                                //var _this = this._obj;
                                //var _this = target._obj;
                                elog("========Proxy get==0==args.length===="+len);

                                elog("========Proxy get==1==type:===="+typeof _this);

                                //elog("========Proxy get==2==name:===="+typeof _this[property]);

                                //elog("========Proxy get==2==name:===="+typeof gggg._obj);

                                elog("====ToString:"+_this.ToString()+"====");

                                //CObject.showMethods(_this);
                                elog("========Proxy get==3==name:====");

                                var args_in = [];
                                var args_out = [];
                                for (var i=0;i<len;i++){
                                    var arg = args[i];
                                    if (arg.data) {
                                        args_out.push(arg);
                                    }
                                    else {
                                        args_in.push(arg);
                                    }
                                }


                                //var r = _this[key].apply(_this, args);
                                var r = _obj[key].apply(_obj, args_in);

                                var outlen = args_out.length;
                                if (outlen == 0) {
                                    return r;
                                }
                                else if(outlen == 1) {
                                    args_out[0].data = r;
                                }
                                else {
                                    var i = 0;
                                    for (var p in r) {
                                        args_out[i].data = r[p];
                                        i++;
                                    }
                                }

                                return;

                                // var r;
                                // switch (len) {
                                //     case 0:
                                //         r = _this[property]();
                                //         break;
                                //     case 1:
                                //         r = _this[property](args[0]);
                                //         break;
                                //     case 2:
                                //         r = _this[property](args[0], args[1]);
                                //         break;
                                //     default:
                                //         r = _this[property].apply(_this, args);
                                //         break;
                                // }   //switch (len)

                                elog("====proxy ok ok!====");
                                elog("====proxy ok ok!===="+_this.getClass().GetName()+"::"+key);
                                //Assert(0);
                                return r;
                            }
                        }
                        elog("====ViewGroup::LayoutParams::Proxy::get====end====" + key);
                        return oGetFn;
                    }
                });

}   //if(_obj._obj)

                //gggg._obj = _obj;

                //elog("====ViewGroup::LayoutParams::constructor====_obj type:" + typeof gggg._obj);
                //elog("====ToString:"+gggg._obj.ToString()+"====");

                //CObject.showMethods(gggg._obj);


                //oRet._obj = _obj;

                return oRet;

            }
        }
        return this._LayoutParams;
    }   //get LayoutParams
}   //class ViewGroup

// import android.view.ViewParent;
// import android.view.accessibility.AccessibilityEvent;
// import android.view.animation.Interpolator;
    class Interpolator {
        constructor() {
            elog("====Interpolator::constructor====");

            // return Droid_New("Elastos.Droid.Graphics.CInterpolator");
        }
    }
// import android.widget.Scroller;
    class Scroller {
        constructor(context, sInterpolator) {
            elog("====Scroller::constructor====begin====");
            var s = CObject.getConstructorProtos("/system/lib/Elastos.Droid.Core.eco", "Elastos.Droid.Widget.CScroller");
            elog("====PROTO: " + s);

            elog("====Scroller::constructor====0====sInterpolator:"+typeof sInterpolator);
            var oRet = Droid_New("Elastos.Droid.Widget.CScroller", context, sInterpolator);
            elog("====Scroller::constructor====1====");
            return oRet;

            return new Proxy(
                {
                 _obj : Droid_New("Elastos.Droid.Widget.CScroller", context, sInterpolator),
                },
                {
                    get : function(target, property){
                        elog("====Scroll::Proxy::get====begin====");

                        elog("====Scroll::Proxy::get====0====");
                        var oo = Droid_New("Elastos.Droid.Widget.CScroller", context, sInterpolator);
                        elog("====Scroll::Proxy::get====1====");

                        //CObject.showMethods(oo);

                        if (target.hasOwnProperty(property)) {
                            return target[property];
                        }
                    },
                    // set : function(target, property, value) {
                    //     elog("====Scroll::Proxy::set====");
                    // },
                }
            );
        }
    }

// import java.lang.reflect.Method;
// import java.util.ArrayList;
    class ArrayList {
        constructor() {
            return Core_New("Elastos.Utility.CArrayList");
        }
    };  // = Elastos.Utility.IArrayList;
// import java.util.Collections;

    var Collections = Core_New("Elastos.Utility.CCollections");

// import java.util.Comparator;
    class Comparator {}; // = Elastos.Core.IComparator;

    class Runnable {};
    class AccessibilityDelegateCompat {};

    var ViewConfigurationCompat = require("./ViewConfigurationCompat.js")(aoElastos, aoActivity);

    class EdgeEffectCompat {
        constructor(context) {
            // var s = CObject.getConstructorProtos("/system/lib/Elastos.Droid.Core.eco", "Elastos.Droid.Widget.CEdgeEffect");
            // elog("====PROTO: " + s);

            return Droid_New("Elastos.Droid.Widget.CEdgeEffect", context);
        }
    }

    var ViewCompat = new Proxy(
        {
            view : null,
            //TODO: IView::static propery
            IMPORTANT_FOR_ACCESSIBILITY_AUTO : 0x00000000,
            IMPORTANT_FOR_ACCESSIBILITY_YES : 0x00000001,
            IMPORTANT_FOR_ACCESSIBILITY_NO : 0x00000002,

            OVER_SCROLL_ALWAYS : 0,
            OVER_SCROLL_IF_CONTENT_SCROLLS : 1,
        },
        {
            set : function(target, property, value) {
                elog("====ViewCompat::Proxy::set====" + property);
                if (property == "view") target.view = value;
            },
            get : function(target, property){
                elog("====ViewCompat::Proxy::get====" + property);

                if (target.hasOwnProperty(property)) {
                    return target[property];
                }
                else {
                    var ret = function() {
                        //elog("====typeof:" + property + " " + typeof target[property]);
                        var args = [...arguments];
                        var _this = args.shift();
                        //var r = target[property].apply(target, arguments);
                        var r = _this[property].apply(_this, args);
                        elog("====proxy ok!===="+_this[property]);
                        return r;
                    }
                    return ret;
                }
            }
        }
    );

    var tempData = {};

// View::MeasureSpec
// MeasureSpec.AT_MOST;
// MeasureSpec.EXACTLY;
// MeasureSpec.makeMeasureSpec
// MeasureSpec.GetEXACTLY()
// MeasureSpec.GetSize

    var MeasureSpec = new Proxy(
        {

            _obj:null,
        },
        {
            set : function(target, property, value) {
                elog("====MeasureSpec::Proxy::set====" + property);
                if (property == "_obj") {
                    elog("======================oooo=====0======");
                    target._obj = value;
                    elog("======================oooo=====1======");
                    return value;
                }
            },
            get : function(target, property){
                elog("====MeasureSpec::Proxy::get====" + property);

                if (target.hasOwnProperty(property)) {
                    return target[property];
                }
                else {
                    var ret = function() {
                        elog("====MeasureSpec::Proxy::get====typeof:" + property + " " + typeof target._obj[property]);
                        var args = [...arguments];
                        //var _this = args.shift();
                        //var r = target[property].apply(target, arguments);
                        var r = target._obj[property].apply(target._obj, args);
                        elog("====proxy ok!====");
                        return r;
                    }
                    return ret;
                }
            }
        }
    );

/**
 * Layout manager that allows the user to flip left and right
 * through pages of data.  You supply an implementation of a
 * {@link PagerAdapter} to generate the pages that the view shows.
 *
 * <p>Note this class is currently under early design and
 * development.  The API will likely change in later updates of
 * the compatibility library, requiring changes to the source code
 * of apps when they are compiled against the newer version.</p>
 *
 * <p>ViewPager is most often used in conjunction with {@link android.app.Fragment},
 * which is a convenient way to supply and manage the lifecycle of each page.
 * There are standard adapters implemented for using fragments with the ViewPager,
 * which cover the most common use cases.  These are
 * {@link android.support.v4.app.FragmentPagerAdapter} and
 * {@link android.support.v4.app.FragmentStatePagerAdapter}; each of these
 * classes have simple code showing how to build a full user interface
 * with them.
 *
 * <p>For more information about how to use ViewPager, read <a
 * href="{@docRoot}training/implementing-navigation/lateral.html">Creating Swipe Views with
 * Tabs</a>.</p>
 *
 * <p>Below is a more complicated example of ViewPager, using it in conjunction
 * with {@link android.app.ActionBar} tabs.  You can find other examples of using
 * ViewPager in the API 4+ Support Demos and API 13+ Support Demos sample code.
 *
 * {@sample development/samples/Support13Demos/src/com/example/android/supportv13/app/ActionBarTabsPager.java
 *      complete}
 */

// public class ViewPager extends ViewGroup {
class ViewPager extends ViewGroup {
    constructor() {
        elog("====ViewPager::constructor====begin==9999==");
        super();
    }

//     private static final String TAG = "ViewPager";
    static get TAG() {return "ViewPager";}

//     private static final boolean DEBUG = false;
    static get DEBUG() {return false;}

//     private static final boolean USE_CACHE = false;
    static get USE_CACHE() {return false;}

//     private static final int DEFAULT_OFFSCREEN_PAGES = 1;
    static get DEFAULT_OFFSCREEN_PAGES() {return 1;}
//     private static final int MAX_SETTLE_DURATION = 600; // ms
    static get MAX_SETTLE_DURATION() {return 600;} // ms
//     private static final int MIN_DISTANCE_FOR_FLING = 25; // dips
    static get MIN_DISTANCE_FOR_FLING() {return 25;} // dips

//     private static final int DEFAULT_GUTTER_SIZE = 16; // dips
    static get DEFAULT_GUTTER_SIZE() {return 16;} // dips

//     private static final int MIN_FLING_VELOCITY = 400; // dips
    static get MIN_FLING_VELOCITY() {return 400;} // dips

//     private static final int[] LAYOUT_ATTRS = new int[] {
//         android.R.attr.layout_gravity
//     };
    static get LAYOUT_ATTRS() {return [
                0x010100b3 ];}

    /**
     * Used to track what the expected number of items in the adapter should be.
     * If the app changes this when we don't expect it, we'll throw a big obnoxious exception.
     */
//     private int mExpectedAdapterCount;
    get mExpectedAdapterCount() {
        elog("====ViewPager::mExpectedAdapterCount::get====v:"+this._mExpectedAdapterCount);
        return this._mExpectedAdapterCount || 0;
    }
    set mExpectedAdapterCount(v) {
        this._mExpectedAdapterCount = v;

        this._mTmpCnt = this._mTmpCnt || 0;
        this._mTmpCnt++;

        // if (this._mTmpCnt==1) {
        //     ss.tt();Assert(0);
        // }

        elog("====ViewPager::mExpectedAdapterCount::set====v:"+this._mExpectedAdapterCount+"====cnt:"+this._mTmpCnt);
        return v;
    }

//     static class ItemInfo {
//         Object object;
//         int position;
//         boolean scrolling;
//         float widthFactor;
//         float offset;
//     }
    static get ItemInfo() {
        if (ViewPager._ItemInfo) return ViewPager._ItemInfo;
        ViewPager._ItemInfo = class ItemInfo {
            // constructor() {
            //     return {
            //         object : null,
            //         position : 0,
            //         scrolling : false,
            //         offset : 0.0,
            //     };
            // }

            //object为PagerAdapter的instantiateItem函数返回的对象
            get object() {return this._object || null;};
            set object(v) {this._object = v; return v;};

            //position为页面的序号，即第几个页面
            get position() {
                elog("====ItemInfo::get position====");
                return this._position || 0;
            };
            set position(v) {
                elog("====ItemInfo::set position====");
                this._position = v; return v;
            };

            //是否正在滚动
            get scrolling() {return this._scrolling || false;};
            set scrolling(v) {this._scrolling = v; return v;};

            //页面宽度，取值为0到1，表示为页面宽度与ViewPager显示区域宽度比例，默认为1
            get widthFactor() {return this._widthFactor || 1;};
            set widthFactor(v) {
                elog("====ItemInfo::widthFactor::set====");
                this._widthFactor = v; return v;
            };

            //偏移量，页面移动的偏移量，默认为0
            get offset() {return this._offset || 0;};
            set offset(v) {this._offset = v; return v;};
        }
        return ViewPager._ItemInfo;
    }

//     private static final Comparator<ItemInfo> COMPARATOR = new Comparator<ItemInfo>(){
//         @Override
//         public int compare(ItemInfo lhs, ItemInfo rhs) {
//             return lhs.position - rhs.position;
//         }
//     };
    static get COMPARATOR() {
        if (ViewPager._COMPARATOR_) return ViewPager._COMPARATOR;

        ViewPager._COMPARATOR_ = true;
        ViewPager._COMPARATOR = new class _tmp extends Comparator {
            compare(lhs, rhs) {
                return lhs.position - rhs.position;
            }
        }
        return ViewPager._COMPARATOR;
    }

//     private static final Interpolator sInterpolator = new Interpolator() {
//         public float getInterpolation(float t) {
//             t -= 1.0f;
//             return t * t * t * t * t + 1.0f;
//         }
//     };
    //Scroller使用的动画插值器，根据不同的时间控制滑动的速度
    static get sInterpolator() {
        if (ViewPager._sInterpolator_) return ViewPager._sInterpolator;
        ViewPager._sInterpolator_ = true;
        ViewPager._sInterpolator = new class _tmp extends Interpolator {
            GetInterpolation(t) {
                t -= 1.0;
                return t * t * t * t * t + 1.0;
            }
        }();
        return ViewPager._sInterpolator;
    };

//     private final ArrayList<ItemInfo> mItems = new ArrayList<ItemInfo>();
    get mItems() {
        if (this._mItems) return this._mItems;
        this._mItems = new ArrayList();
        return this._mItems;
    }
//     private final ItemInfo mTempItem = new ItemInfo();
    get mTempItem() {
        if (this._mTempItem) return this._mTempItem;
        this._mTempItem = new ViewPager.ItemInfo();
        return this._mTempItem;
    }

//     private final Rect mTempRect = new Rect();
    get mTempRect() {
        if (this._mTempRect) return this._mTempRect;
        this._mTempRect = new Rect();
        return this._mTempRect;
    }

//     private PagerAdapter mAdapter;
    get mAdapter() {return this._mAdapter || null;}
    set mAdapter(v) {this._mAdapter = v;return v;}

//     private int mCurItem;   // Index of currently displayed page.
    get mCurItem() {return this._mCurItem || 0;}
    set mCurItem(v) {this._mCurItem = v;return v;}
//     private int mRestoredCurItem = -1;
//     private Parcelable mRestoredAdapterState = null;
//     private ClassLoader mRestoredClassLoader = null;
//     private Scroller mScroller;

//     private PagerObserver mObserver;
    get mObserver() {return this._mObserver || null;}
    set mObserver(v) {this._mObserver = v;}

//     private int mPageMargin;
//     private Drawable mMarginDrawable;
//     private int mTopPageBounds;
//     private int mBottomPageBounds;

//     // Offsets of the first and last items, if known.
//     // Set during population, used to determine if we are at the beginning
//     // or end of the pager data set during touch scrolling.
//     private float mFirstOffset = -Float.MAX_VALUE;
        //var mFirstOffset = 0;   //Elastos.Core.Math.FLOAT_MAX_VALUE;
    get mFirstOffset() {return this._mFirstOffset || 0xEFFFFFFF;}
    set mFirstOffset(v) {this._mFirstOffset = v;}


//const Float Math::FLOAT_MAX_VALUE           = 3.40282346638528860e+38F;
//const Float Math::FLOAT_MIN_VALUE           = 1.40129846432481707e-45F;
//const Float Math::FLOAT_ZERO                = 0.0F;


//     private float mLastOffset = Float.MAX_VALUE;
        //var mLastOffset = 0;    //Elastos.Core.Math.FLOAT_MAX_VALUE;
    get mLastOffset() {return this._mLastOffset || 0xEFFFFFFF;}
    set mLastOffset(v) {this._mLastOffset = v;}

//     private int mChildWidthMeasureSpec;
//     private int mChildHeightMeasureSpec;
//     private boolean mInLayout;
    get mInLayout() {
        elog("====ViewPager::mInLayout====get====");
        return this._mInLayout;
    }
    set mInLayout(v) {
        elog("====ViewPager::mInLayout====set====");
        this._mInLayout = v;
        return v;
    }

//     private boolean mScrollingCacheEnabled;

//     private boolean mPopulatePending;
//     private int mOffscreenPageLimit = DEFAULT_OFFSCREEN_PAGES;
    get mOffscreenPageLimit() {return ViewPager.DEFAULT_OFFSCREEN_PAGES;}

//     private boolean mIsBeingDragged;
//     private boolean mIsUnableToDrag;
//     private boolean mIgnoreGutter;
//     private int mDefaultGutterSize;
//     private int mGutterSize;
//     private int mTouchSlop;

    /**
     * Position of the last motion event.
     */
//     private float mLastMotionX;
//     private float mLastMotionY;
//     private float mInitialMotionX;
//     private float mInitialMotionY;
    /**
     * ID of the active pointer. This is used to retain consistency during
     * drags/flings if multiple pointers are used.
     */
//     private int mActivePointerId = INVALID_POINTER;
    /**
     * Sentinel value for no current active pointer.
     * Used by {@link #mActivePointerId}.
     */
//     private static final int INVALID_POINTER = -1;
    static get INVALID_POINTER() {return -1;}

//     /**
//      * Determines speed during touch scrolling
//      */
//     private VelocityTracker mVelocityTracker;
//     private int mMinimumVelocity;
//     private int mMaximumVelocity;
//     private int mFlingDistance;
//     private int mCloseEnough;

    // If the pager is at least this close to its final position, complete the scroll
    // on touch down and let the user interact with the content inside instead of
    // "catching" the flinging pager.
//     private static final int CLOSE_ENOUGH = 2; // dp
    static get CLOSE_ENOUGH() {return 2;}

//     private boolean mFakeDragging;
//     private long mFakeDragBeginTime;

//     private EdgeEffectCompat mLeftEdge;
//     private EdgeEffectCompat mRightEdge;

//     private boolean mFirstLayout = true;
    get mFirstLayout() {return this._mFirstLayout || true;}
    set mFirstLayout(v) {this._mFirstLayout = v;}
//     private boolean mNeedCalculatePageOffsets = false;
    get mNeedCalculatePageOffsets() {return this._mNeedCalculatePageOffsets || false;}
    set mNeedCalculatePageOffsets(v) {this._mNeedCalculatePageOffsets = v;return v;}
 //     private boolean mCalledSuper;
//     private int mDecorChildCount;

//     private OnPageChangeListener mOnPageChangeListener;
    get mOnPageChangeListener() {return this._mOnPageChangeListener;}
    set mOnPageChangeListener(v) {this._mOnPageChangeListener = v;}
//     private OnPageChangeListener mInternalPageChangeListener;
//     private OnAdapterChangeListener mAdapterChangeListener;
//     private PageTransformer mPageTransformer;
//     private Method mSetChildrenDrawingOrderEnabled;

//     private static final int DRAW_ORDER_DEFAULT = 0;
    static get DRAW_ORDER_DEFAULT() {return 0;}
//     private static final int DRAW_ORDER_FORWARD = 1;
    static get DRAW_ORDER_FORWARD() {return 1;}
//     private static final int DRAW_ORDER_REVERSE = 2;
    static get DRAW_ORDER_REVERSE() {return 2;}
//     private int mDrawingOrder;
//     private ArrayList<View> mDrawingOrderedChildren;
//     private static final ViewPositionComparator sPositionComparator = new ViewPositionComparator();
    static get sPositionComparator() {
        elog("====get sPositionComparator====");
        if (this._sPositionComparator_) return this._sPositionComparator;
        this._sPositionComparator_ = true;
        this._sPositionComparator = new this.ViewPositionComparator();
        return this._sPositionComparator;
    }

//     /**
//      * Indicates that the pager is in an idle, settled state. The current page
//      * is fully in view and no animation is in progress.
//      */
//     public static final int SCROLL_STATE_IDLE = 0;
    static get SCROLL_STATE_IDLE() {return 0;}

//     /**
//      * Indicates that the pager is currently being dragged by the user.
//      */
//     public static final int SCROLL_STATE_DRAGGING = 1;
    static get SCROLL_STATE_DRAGGING() {return 1;}

//     /**
//      * Indicates that the pager is in the process of settling to a final position.
//      */
//     public static final int SCROLL_STATE_SETTLING = 2;
    static get SCROLL_STATE_SETTLING() {return 2;}

//     private final Runnable mEndScrollRunnable = new Runnable() {
//         public void run() {
//             setScrollState(SCROLL_STATE_IDLE);
//             populate();
//         }
//     };
    get mEndScrollRunnable() {
        if (this._mEndScrollRunnable_) return this._mEndScrollRunnable;

        this._mEndScrollRunnable_ = true;
        this._mEndScrollRunnable = new class _tmp extends Runnable {
            Run() {
                this.SetScrollState(ViewPager.SCROLL_STATE_IDLE);
                elog("====ViewPager::mEndScrollRunnable====Populate====call====0====");
                this.Populate();
            }
        }

        return this._mEndScrollRunnable;
    }

//     private int mScrollState = SCROLL_STATE_IDLE;
    static get mScrollState() {return ViewPager.SCROLL_STATE_IDLE;}

    /**
     * Callback interface for responding to changing state of the selected page.
     */
//     public interface OnPageChangeListener {
    get OnPageChangeListener() {
        if (this._OnPageChangeListener_) return this._OnPageChangeListener;

        this._OnPageChangeListener_ = true;
        this._OnPageChangeListener = class {
        /**
         * This method will be invoked when the current page is scrolled, either as part
         * of a programmatically initiated smooth scroll or a user initiated touch scroll.
         *
         * @param position Position index of the first page currently being displayed.
         *                 Page position+1 will be visible if positionOffset is nonzero.
         * @param positionOffset Value from [0, 1) indicating the offset from the page at position.
         * @param positionOffsetPixels Value in pixels indicating the offset from position.
         */
//         public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels);

        /**
         * This method will be invoked when a new page becomes selected. Animation is not
         * necessarily complete.
         *
         * @param position Position index of the new selected page.
         */
//         public void onPageSelected(int position);

        /**
         * Called when the scroll state changes. Useful for discovering when the user
         * begins dragging, when the pager is automatically settling to the current page,
         * or when it is fully stopped/idle.
         *
         * @param state The new scroll state.
         * @see ViewPager#SCROLL_STATE_IDLE
         * @see ViewPager#SCROLL_STATE_DRAGGING
         * @see ViewPager#SCROLL_STATE_SETTLING
         */
//         public void onPageScrollStateChanged(int state);
        };

        return this._OnPageChangeListener;
    }

    /**
     * Simple implementation of the {@link OnPageChangeListener} interface with stub
     * implementations of each method. Extend this if you do not intend to override
     * every method of {@link OnPageChangeListener}.
     */
//     public static class SimpleOnPageChangeListener implements OnPageChangeListener {
    static get SimpleOnPageChangeListener() {
        if (ViewPager._SimpleOnPageChangeListener_) return ViewPager._SimpleOnPageChangeListener;

        ViewPager._SimpleOnPageChangeListener_ = true;
        ViewPager._SimpleOnPageChangeListener = class _tmp {
//         @Override
//         public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {
            OnPageScrolled(position, positionOffset, positionOffsetPixels) {
//             // This space for rent
            }

//         @Override
//         public void onPageSelected(int position) {
            OnPageSelected(position) {
//             // This space for rent
            }

//         @Override
//         public void onPageScrollStateChanged(int state) {
            OnPageScrollStateChanged(state) {
//             // This space for rent
            }
        };
        return ViewPager._SimpleOnPageChangeListener;
    }

    /**
     * A PageTransformer is invoked whenever a visible/attached page is scrolled.
     * This offers an opportunity for the application to apply a custom transformation
     * to the page views using animation properties.
     *
     * <p>As property animation is only supported as of Android 3.0 and forward,
     * setting a PageTransformer on a ViewPager on earlier platform versions will
     * be ignored.</p>
     */
//     public interface PageTransformer {
    get PageTransformer() {
        return class _temp {

        /**
         * Apply a property transformation to the given page.
         *
         * @param page Apply the transformation to this page
         * @param position Position of page relative to the current front-and-center
         *                 position of the pager. 0 is front and center. 1 is one full
         *                 page position to the right, and -1 is one page position to the left.
         */
//         public void transformPage(View page, float position);
            TransformPage(page, position) {
                elog("====PageTransformer::TransformPage====TODO====");
            }
        };
    }

    /**
     * Used internally to monitor when adapters are switched.
     */
//     interface OnAdapterChangeListener {
//         public void onAdapterChanged(PagerAdapter oldAdapter, PagerAdapter newAdapter);
//     }

    /**
     * Used internally to tag special types of child views that should be added as
     * pager decorations by default.
     */
//     interface Decor {}
    get Decor() {
        if(this._Decor)return this._Decor;
        this._Decor = class Decor {};
        return this._Decor;
    }

//     public ViewPager(Context context) {
//         super(context);
//         initViewPager();
//     }

//     public ViewPager(Context context, AttributeSet attrs) {
//         super(context, attrs);
//         initViewPager();
//     }
    OnCreate(_this, context, attrs) {
        elog("====ViewPager::ViewPager====");

        attrs = attrs || null;

        MeasureSpec._obj = _this.GetMeasureSpec();
        global.MeasureSpec = MeasureSpec;

//CObject.showMethods(MeasureSpec._obj);
        //ss.tt();Assert(0);

        super.OnCreate(_this, context, attrs);
        this.InitViewPager();
    }

//     void initViewPager() {
//         setWillNotDraw(false);
//         setDescendantFocusability(FOCUS_AFTER_DESCENDANTS);
//         setFocusable(true);
//         final Context context = getContext();
//         mScroller = new Scroller(context, sInterpolator);
//         final ViewConfiguration configuration = ViewConfiguration.get(context);
//         final float density = context.getResources().getDisplayMetrics().density;

//         mTouchSlop = ViewConfigurationCompat.getScaledPagingTouchSlop(configuration);
//         mMinimumVelocity = (int) (MIN_FLING_VELOCITY * density);
//         mMaximumVelocity = configuration.getScaledMaximumFlingVelocity();
//         mLeftEdge = new EdgeEffectCompat(context);
//         mRightEdge = new EdgeEffectCompat(context);

//         mFlingDistance = (int) (MIN_DISTANCE_FOR_FLING * density);
//         mCloseEnough = (int) (CLOSE_ENOUGH * density);
//         mDefaultGutterSize = (int) (DEFAULT_GUTTER_SIZE * density);

//         ViewCompat.setAccessibilityDelegate(this, new MyAccessibilityDelegate());

//         if (ViewCompat.getImportantForAccessibility(this)
//                 == ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
//             ViewCompat.setImportantForAccessibility(this,
//                     ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);
//         }
//     }
    InitViewPager() {
        elog("====ViewPager::InitViewPager====begin====");
        var _this = this._obj;

        var FOCUS_AFTER_DESCENDANTS = 0x40000;  //TODO
        _this.SetWillNotDraw(false);
        _this.SetDescendantFocusability(FOCUS_AFTER_DESCENDANTS);
        _this.SetFocusable(true);
        var context = _this.GetContext();
        this.mScroller = new Scroller(context, ViewPager.sInterpolator);
        var configuration = ViewConfiguration.Get(context);
        var density = context.GetResources().GetDisplayMetrics().GetDensity();

        this.mTouchSlop = ViewConfigurationCompat.GetScaledPagingTouchSlop(configuration);
        this.mMinimumVelocity = this.MIN_FLING_VELOCITY * density;
        this.mMaximumVelocity = configuration.GetScaledMaximumFlingVelocity();
        this.mLeftEdge = new EdgeEffectCompat(context);
        this.mRightEdge = new EdgeEffectCompat(context);

        this.mFlingDistance = this.MIN_DISTANCE_FOR_FLING * density;
        this.mCloseEnough = this.CLOSE_ENOUGH * density;
        this.mDefaultGutterSize = this.DEFAULT_GUTTER_SIZE * density;

        ViewCompat.SetAccessibilityDelegate(_this, new this.MyAccessibilityDelegate());

        if (ViewCompat.GetImportantForAccessibility(_this)
                == ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
            ViewCompat.SetImportantForAccessibility(_this,
                    ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);
        }
        elog("====ViewPager::InitViewPager====end====");
    }

//     @Override
//     protected void onDetachedFromWindow() {
//         removeCallbacks(mEndScrollRunnable);
//         super.onDetachedFromWindow();
//     }
    OnDetachedFromWindow() {
        elog("====ViewPager::OnDetachedFromWindow====TODO====");
        removeCallbacks(mEndScrollRunnable);
        super.onDetachedFromWindow();
    }

//     private void setScrollState(int newState) {
//         if (mScrollState == newState) {
//             return;
//         }

//         mScrollState = newState;
//         if (mPageTransformer != null) {
//             // PageTransformers can do complex things that benefit from hardware layers.
//             enableLayers(newState != SCROLL_STATE_IDLE);
//         }
//         if (mOnPageChangeListener != null) {
//             mOnPageChangeListener.onPageScrollStateChanged(newState);
//         }
//      }
    SetScrollState(newState) {
        if (mScrollState == newState) {
            return;
        }

        mScrollState = newState;
        if (mPageTransformer != null) {
            // PageTransformers can do complex things that benefit from hardware layers.
            enableLayers(newState != SCROLL_STATE_IDLE);
        }
        if (mOnPageChangeListener != null) {
            mOnPageChangeListener.onPageScrollStateChanged(newState);
        }
     }

//     /**
//      * Set a PagerAdapter that will supply views for this pager as needed.
//      *
//      * @param adapter Adapter to use
//      */
//     public void setAdapter(PagerAdapter adapter) {
//         if (mAdapter != null) {
//             mAdapter.unregisterDataSetObserver(mObserver);
//             mAdapter.startUpdate(this);
//             for (int i = 0; i < mItems.size(); i++) {
//                 final ItemInfo ii = mItems.get(i);
//                 mAdapter.destroyItem(this, ii.position, ii.object);
//             }
//             mAdapter.finishUpdate(this);
//             mItems.clear();
//             removeNonDecorViews();
//             mCurItem = 0;
//             scrollTo(0, 0);
//         }

//         final PagerAdapter oldAdapter = mAdapter;
//         mAdapter = adapter;
//         mExpectedAdapterCount = 0;

//         if (mAdapter != null) {
//             if (mObserver == null) {
//                 mObserver = new PagerObserver();
//             }

//             mAdapter.registerDataSetObserver(mObserver);
//             mPopulatePending = false;
//             final boolean wasFirstLayout = mFirstLayout;
//             mFirstLayout = true;
//             mExpectedAdapterCount = mAdapter.getCount();

//             if (mRestoredCurItem >= 0) {
//                 mAdapter.restoreState(mRestoredAdapterState, mRestoredClassLoader);
//                 setCurrentItemInternal(mRestoredCurItem, false, true);
//                 mRestoredCurItem = -1;
//                 mRestoredAdapterState = null;
//                 mRestoredClassLoader = null;
//             } else if (!wasFirstLayout) {
//                 populate();
//             } else {
//                 requestLayout();
//             }

//         if (mAdapterChangeListener != null && oldAdapter != adapter) {
//             mAdapterChangeListener.onAdapterChanged(oldAdapter, adapter);
//         }
//     }
    //ViewPager与数据源之间需要通过适配器来适配。
    //调用ViewPager的setAdapter函数即可将ViewPager与PagerAdapter关联交互。
    //如果数据源发生变化，需要在代码里调用PagerAdapter的notifyDataSetChanged函数，
    //即通知ViewPager数据源发生变化，
    //ViewPager就是一个观察者，通过观察者类PagerObserver做相关应对操作。
    SetAdapter(adapter) {
        elog("====ViewPager::SetAdapter====begin====");

        var _this = this._obj;

        elog("====ViewPager::SetAdapter====mAdapter===="+typeof this.mAdapter);

        //1.如果已经设置过PagerAdapter，即mAdapter != null，
        // 则做一些清理工作
        if (this.mAdapter != null) {
            elog("====ViewPager::SetAdapter====0====");

            //2.清除观察者
            this.UnregisterDataSetObserver(this.mObserver);
            //3.回调startUpdate函数，告诉PagerAdapter开始更新要显示的页面
            this.mAdapter.startUpdate(this);
            //4.如果之前保存有页面，则将之前所有的页面destroy掉
            for (var i = 0; i < this.mItems.GetSize(); i++) {
                var ii = this.mItems.Get(i);
                this.mAdapter.destroyItem(this, ii.position, ii.object);
            }
            //5.回调finishUpdate，告诉PagerAdapter结束更新
            this.mAdapter.FinishUpdate(this);
            //6.将所有的页面清除
            this.mItems.Clear();
            //7.将所有的非Decor View移除，即将页面移除
            removeNonDecorViews();
            //8.当前的显示页面重置到第一个
            this.mCurItem = 0;
            //9.滑动重置到(0,0)位置
            _this.ScrollTo(0, 0);
        }

        //10.保存上一次的PagerAdapter
        var oldAdapter = this.mAdapter;
        //11.设置mAdapter为新的PagerAdapter
        this.mAdapter = adapter;
        //12.设置期望的适配器中的页面数量为0个
        this.mExpectedAdapterCount = 0;

        //13.如果设置的PagerAdapter不为null
        if (this.mAdapter != null) {
            elog("====ViewPager::SetAdapter====1====");

            //14.确保观察者不为null，观察者主要是用于监视数据源的内容发生变化
            if (this.mObserver == null) {
                elog("====ViewPager::SetAdapter====2====");
                this.mObserver = new this.PagerObserver();
            }

            //15.将观察者设置到PagerAdapter中
            this.mAdapter.RegisterDataSetObserver(this.mObserver);

            this.mPopulatePending = false;
            //16.保存上一次是否是第一次Layout
            var wasFirstLayout = this.mFirstLayout;
            //17.设定当前为第一次Layout
            this.mFirstLayout = true;
            //18.更新期望的数据源中页面个数
            this.mExpectedAdapterCount = this.mAdapter.GetCount();
            elog("====ViewPager::SetAdapter====2.1===="+this.mAdapter.GetCount());

            // ss.tt();Assert(0);

            if (this.mRestoredCurItem >= 0) {
                //19.如果有数据需要恢复
                elog("====ViewPager::SetAdapter====3.0====");
                //20.回调PagerAdapter的restoreState函数
                this.mAdapter.RestoreState(this.mRestoredAdapterState, this.mRestoredClassLoader);
                SetCurrentItemInternal(this.mRestoredCurItem, false, true);
                //21.标记无需再恢复
                this.mRestoredCurItem = -1;
                this.mRestoredAdapterState = null;
                this.mRestoredClassLoader = null;
            } else if (!wasFirstLayout) {
                //如果在此之前不是第一次Layout
                elog("====ViewPager::SetAdapter====3.1====");
                elog("====ViewPager::SetAdapter====Populate====call====1====");
                //22.由于ViewPager并不是将所有页面作为子View，
                // 而是最多缓存用户指定缓存个数*2（左右两边，可能左边或右边没有那么多页面）
                //因此需要创建和销毁页面，populate主要工作就是这些
                this.Populate();
            } else {
                //23.重新布局（Layout）
                elog("====ViewPager::SetAdapter====3.2====");
                _this.RequestLayout();
            }
        }

        //24.如果PagerAdapter发生变化，并且设置了OnAdapterChangeListener监听器
        if (this.mAdapterChangeListener != null && oldAdapter != adapter) {
            elog("====ViewPager::SetAdapter====4.0====");
            // 则回调OnAdapterChangeListener的onAdapterChanged函数
            this.mAdapterChangeListener.OnAdapterChanged(oldAdapter, adapter);
        }

        elog("====ViewPager::SetAdapter====end====");
    }   //SetAdapter

//     private void removeNonDecorViews() {
//         for (int i = 0; i < getChildCount(); i++) {
//             final View child = getChildAt(i);
//             final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//             if (!lp.isDecor) {
//                 removeViewAt(i);
//                 i--;
//             }
//         }
//     }
    RemoveNonDecorViews() {
        for (var i = 0; i < getChildCount(); i++) {
            var child = getChildAt(i);
            var lp = child.getLayoutParams();
            if (!lp.isDecor) {
                removeViewAt(i);
                i--;
            }
        }
    }

//     /**
//      * Retrieve the current adapter supplying pages.
//      *
//      * @return The currently registered PagerAdapter
//      */
//     public PagerAdapter getAdapter() {
//         return mAdapter;
//     }
    GetAdapter() {
        return this.mAdapter;
    }

//     void setOnAdapterChangeListener(OnAdapterChangeListener listener) {
//         mAdapterChangeListener = listener;
//     }
    SetOnAdapterChangeListener(listener) {
        this.mAdapterChangeListener = listener;
    }

//     private int getClientWidth() {
//         return getMeasuredWidth() - getPaddingLeft() - getPaddingRight();
//     }
    GetClientWidth() {
        var _this = this._obj;
        return _this.GetMeasuredWidth() - _this.GetPaddingLeft() - _this.GetPaddingRight();
    }

//     /**
//      * Set the currently selected page. If the ViewPager has already been through its first
//      * layout with its current adapter there will be a smooth animated transition between
//      * the current item and the specified item.
//      *
//      * @param item Item index to select
//      */
//     public void setCurrentItem(int item) {
//         mPopulatePending = false;
//         setCurrentItemInternal(item, !mFirstLayout, false);
//     }
    SetCurrentItem(item) {
        this.mPopulatePending = false;
        this.SetCurrentItemInternal(item, !this.mFirstLayout, false);
    }

//     /**
//      * Set the currently selected page.
//      *
//      * @param item Item index to select
//      * @param smoothScroll True to smoothly scroll to the new item, false to transition immediately
//      */
//     public void setCurrentItem(int item, boolean smoothScroll) {
//         mPopulatePending = false;
//         setCurrentItemInternal(item, smoothScroll, false);
//     }
    SetCurrentItem(item, smoothScroll) {
        this.mPopulatePending = false;
        this.SetCurrentItemInternal(item, smoothScroll, false);
    }

//     public int getCurrentItem() {
//         return mCurItem;
//     }
    GetCurrentItem() {
        return this.mCurItem;
    }

//     void setCurrentItemInternal(int item, boolean smoothScroll, boolean always) {
//         setCurrentItemInternal(item, smoothScroll, always, 0);
//     }
    SetCurrentItemInternal(item, smoothScroll, always) {
        this.SetCurrentItemInternal(item, smoothScroll, always, 0);
    }

//     void setCurrentItemInternal(int item, boolean smoothScroll, boolean always, int velocity) {
//         if (mAdapter == null || mAdapter.getCount() <= 0) {
//             setScrollingCacheEnabled(false);
//             return;
//         }
//         if (!always && mCurItem == item && mItems.size() != 0) {
//             setScrollingCacheEnabled(false);
//             return;
//         }

//         if (item < 0) {
//             item = 0;
//         } else if (item >= mAdapter.getCount()) {
//             item = mAdapter.getCount() - 1;
//         }
//         final int pageLimit = mOffscreenPageLimit;
//         if (item > (mCurItem + pageLimit) || item < (mCurItem - pageLimit)) {
//             // We are doing a jump by more than one page.  To avoid
//             // glitches, we want to keep all current pages in the view
//             // until the scroll ends.
//             for (int i=0; i<mItems.size(); i++) {
//                 mItems.get(i).scrolling = true;
//             }
//         }
//         final boolean dispatchSelected = mCurItem != item;

//         if (mFirstLayout) {
//             // We don't have any idea how big we are yet and shouldn't have any pages either.
//             // Just set things up and let the pending layout handle things.
//             mCurItem = item;
//             if (dispatchSelected && mOnPageChangeListener != null) {
//                 mOnPageChangeListener.onPageSelected(item);
//             }
//             if (dispatchSelected && mInternalPageChangeListener != null) {
//                 mInternalPageChangeListener.onPageSelected(item);
//             }
//             requestLayout();
//         } else {
//             populate(item);
//             scrollToItem(item, smoothScroll, velocity, dispatchSelected);
//         }
//     }
    SetCurrentItemInternal(item, smoothScroll, always, velocity) {
        var _this = this._obj;

        if (this.mAdapter == null || this.mAdapter.GetCount() <= 0) {
            setScrollingCacheEnabled(false);
            return;
        }
        if (!always && this.mCurItem == item && this.mItems.GetSize() != 0) {
            setScrollingCacheEnabled(false);
            return;
        }

        if (item < 0) {
            item = 0;
        } else if (item >= this.mAdapter.GetCount()) {
            item = this.mAdapter.GetCount() - 1;
        }
        var pageLimit = this.mOffscreenPageLimit;
        if (item > (this.mCurItem + pageLimit) || item < (this.mCurItem - pageLimit)) {
            // We are doing a jump by more than one page.  To avoid
            // glitches, we want to keep all current pages in the view
            // until the scroll ends.
            for (var i=0; i<this.mItems.GetSize(); i++) {
                this.mItems.Get(i).scrolling = true;
            }
        }
        var dispatchSelected = this.mCurItem != item;

        if (this.mFirstLayout) {
            // We don't have any idea how big we are yet and shouldn't have any pages either.
            // Just set things up and let the pending layout handle things.
            this.mCurItem = item;
            if (dispatchSelected && this.mOnPageChangeListener != null) {
                this.mOnPageChangeListener.onPageSelected(item);
            }
            if (dispatchSelected && this.mInternalPageChangeListener != null) {
                this.mInternalPageChangeListener.onPageSelected(item);
            }
            _this.RequestLayout();
        } else {
            populate(item);
            scrollToItem(item, smoothScroll, velocity, dispatchSelected);
        }
    }

//     private void scrollToItem(int item, boolean smoothScroll, int velocity,
//             boolean dispatchSelected) {
//         final ItemInfo curInfo = infoForPosition(item);
//         int destX = 0;
//         if (curInfo != null) {
//             final int width = getClientWidth();
//             destX = (int) (width * Math.max(mFirstOffset,
//                     Math.min(curInfo.offset, mLastOffset)));
//         }
//         if (smoothScroll) {
//             smoothScrollTo(destX, 0, velocity);
//             if (dispatchSelected && mOnPageChangeListener != null) {
//                 mOnPageChangeListener.onPageSelected(item);
//             }
//             if (dispatchSelected && mInternalPageChangeListener != null) {
//                 mInternalPageChangeListener.onPageSelected(item);
//             }
//         } else {
//             if (dispatchSelected && mOnPageChangeListener != null) {
//                 mOnPageChangeListener.onPageSelected(item);
//             }
//             if (dispatchSelected && mInternalPageChangeListener != null) {
//                 mInternalPageChangeListener.onPageSelected(item);
//             }
//             completeScroll(false);
//             scrollTo(destX, 0);
//             pageScrolled(destX);
//         }
//     }
    ScrollToItem(item, smoothScroll, velocity, dispatchSelected) {
        elog("====ViewPager::ScrollToItem====begin====");

        var _this = this._obj;

        var curInfo = this.InfoForPosition(item);
        var destX = 0;
        if (curInfo != null) {
            var width = this.GetClientWidth();
            destX = width * Math.max(this.mFirstOffset,
                    Math.min(curInfo.offset, this.mLastOffset));
        }
        if (smoothScroll) {
            smoothScrollTo(destX, 0, velocity);
            if (dispatchSelected && this.mOnPageChangeListener != null) {
                this.mOnPageChangeListener.OnPageSelected(item);
            }
            if (dispatchSelected && this.mInternalPageChangeListener != null) {
                this.mInternalPageChangeListener.OnPageSelected(item);
            }
        } else {
            if (dispatchSelected && this.mOnPageChangeListener != null) {
                this.mOnPageChangeListener.OnPageSelected(item);
            }
            if (dispatchSelected && this.mInternalPageChangeListener != null) {
                this.mInternalPageChangeListener.OnPageSelected(item);
            }
            this.CompleteScroll(false);
            _this.ScrollTo(destX, 0);
            this.PageScrolled(destX);
        }

        elog("====ViewPager::ScrollToItem====end====");
    }

    /**
     * Set a listener that will be invoked whenever the page changes or is incrementally
     * scrolled. See {@link OnPageChangeListener}.
     *
     * @param listener Listener to set
     */
//     public void setOnPageChangeListener(OnPageChangeListener listener) {
    SetOnPageChangeListener(listener) {
        this.mOnPageChangeListener = listener;
    }

    /**
     * Set a {@link PageTransformer} that will be called for each attached page whenever
     * the scroll position is changed. This allows the application to apply custom property
     * transformations to each page, overriding the default sliding look and feel.
     *
     * <p><em>Note:</em> Prior to Android 3.0 the property animation APIs did not exist.
     * As a result, setting a PageTransformer prior to Android 3.0 (API 11) will have no effect.</p>
     *
     * @param reverseDrawingOrder true if the supplied PageTransformer requires page views
     *                            to be drawn from last to first instead of first to last.
     * @param transformer PageTransformer that will modify each page's animation properties
     */

    GetInt32Property(property) {
        var sysProp = Droid_New("Elastos.Droid.Os.CSystemProperties");
        var value = sysProp.GetInt32(property, 0);
        return value;
    }

//     public void setPageTransformer(boolean reverseDrawingOrder, PageTransformer transformer) {
//         if (Build.VERSION.SDK_INT >= 11) {
//             final boolean hasTransformer = transformer != null;
//             final boolean needsPopulate = hasTransformer != (mPageTransformer != null);
//             mPageTransformer = transformer;
//             setChildrenDrawingOrderEnabledCompat(hasTransformer);
//             if (hasTransformer) {
//                 mDrawingOrder = reverseDrawingOrder ? DRAW_ORDER_REVERSE : DRAW_ORDER_FORWARD;
//             } else {
//                 mDrawingOrder = DRAW_ORDER_DEFAULT;
//             }
//             if (needsPopulate) populate();
//         }
//     }
    SetPageTransformer(reverseDrawingOrder, transformer) {
        elog("====ViewPager::SetPageTransformer====begin====");
        var SDK_INT = this.GetInt32Property("ro.build.version.sdk");
        if (SDK_INT >= 11) {
            var hasTransformer = transformer != null;
            var needsPopulate = hasTransformer != (this.mPageTransformer != null);
            this.mPageTransformer = transformer;
            this.SetChildrenDrawingOrderEnabledCompat(hasTransformer);
            if (hasTransformer) {
                this.mDrawingOrder = reverseDrawingOrder ? ViewPager.DRAW_ORDER_REVERSE : ViewPager.DRAW_ORDER_FORWARD;
            } else {
                this.mDrawingOrder = this.DRAW_ORDER_DEFAULT;
            }
            if (needsPopulate) {
                elog("====ViewPager::SetPageTransformer====Populate====call====2===");
                this.Populate();
            }
        }
        elog("====ViewPager::SetPageTransformer====end====");
    }

//     void setChildrenDrawingOrderEnabledCompat(boolean enable) {
//         if (Build.VERSION.SDK_INT >= 7) {
//             if (mSetChildrenDrawingOrderEnabled == null) {
//                 try {
//                     mSetChildrenDrawingOrderEnabled = ViewGroup.class.getDeclaredMethod(
//                             "setChildrenDrawingOrderEnabled", new Class[] { Boolean.TYPE });
//                 } catch (NoSuchMethodException e) {
//                     Log.e(TAG, "Can't find setChildrenDrawingOrderEnabled", e);
//                 }
//             }
//             try {
//                 mSetChildrenDrawingOrderEnabled.invoke(this, enable);
//             } catch (Exception e) {
//                 Log.e(TAG, "Error changing children drawing order", e);
//             }
//         }
//     }
    SetChildrenDrawingOrderEnabledCompat(enable) {
        elog("====ViewPager::SetChildrenDrawingOrderEnabledCompat====begin====");
        var SDK_INT = this.GetInt32Property("ro.build.version.sdk");
        if (SDK_INT >= 7) {
            // elog("====ViewPager::SetChildrenDrawingOrderEnabledCompat====todo====");
            // if (this.mSetChildrenDrawingOrderEnabled == null) {
            //     try {
            //         this.mSetChildrenDrawingOrderEnabled = ViewGroup.class.getDeclaredMethod(
            //                 "setChildrenDrawingOrderEnabled", new Class[] { Boolean.TYPE });
            //     } catch (e) {
            //         elog(TAG + "Can't find setChildrenDrawingOrderEnabled" + JSON.stringify(e));
            //     }
            // }
            // try {
            //     this.mSetChildrenDrawingOrderEnabled.Invoke(this, enable);
            // } catch (e) {
            //     elog(TAG + "Error changing children drawing order" + JSON.stringify(e));
            // }
            // this._obj._SetChildrenDrawingOrderEnabled(enable);

            this._obj._SetChildrenDrawingOrderEnabled(enable);
        }
        elog("====ViewPager::SetChildrenDrawingOrderEnabledCompat====end====");
    }

//     @Override
//     protected int getChildDrawingOrder(int childCount, int i) {
//         final int index = mDrawingOrder == DRAW_ORDER_REVERSE ? childCount - 1 - i : i;
//         final int result = ((LayoutParams) mDrawingOrderedChildren.get(index).getLayoutParams()).childIndex;
//         return result;
//     }
    GetChildDrawingOrder(_this, childCount, i, result) {
        var index = this.mDrawingOrder == DRAW_ORDER_REVERSE ? childCount - 1 - i : i;
        result.data = this.mDrawingOrderedChildren.Get(index).GetLayoutParams().childIndex;
    }

    /**
     * Set a separate OnPageChangeListener for internal use by the support library.
     *
     * @param listener Listener to set
     * @return The old listener that was set, if any.
     */
//     OnPageChangeListener setInternalPageChangeListener(OnPageChangeListener listener) {
//         OnPageChangeListener oldListener = mInternalPageChangeListener;
//         mInternalPageChangeListener = listener;
//         return oldListener;
//     }
    SetInternalPageChangeListener(listener) {
        var oldListener = this.mInternalPageChangeListener;
        this.mInternalPageChangeListener = listener;
        return oldListener;
    }

    /**
     * Returns the number of pages that will be retained to either side of the
     * current page in the view hierarchy in an idle state. Defaults to 1.
     *
     * @return How many pages will be kept offscreen on either side
     * @see #setOffscreenPageLimit(int)
     */
//     public int getOffscreenPageLimit() {
//         return mOffscreenPageLimit;
//     }
    GetOffscreenPageLimit() {
        return this.mOffscreenPageLimit;
    }

    /**
     * Set the number of pages that should be retained to either side of the
     * current page in the view hierarchy in an idle state. Pages beyond this
     * limit will be recreated from the adapter when needed.
     *
     * <p>This is offered as an optimization. If you know in advance the number
     * of pages you will need to support or have lazy-loading mechanisms in place
     * on your pages, tweaking this setting can have benefits in perceived smoothness
     * of paging animations and interaction. If you have a small number of pages (3-4)
     * that you can keep active all at once, less time will be spent in layout for
     * newly created view subtrees as the user pages back and forth.</p>
     *
     * <p>You should keep this limit low, especially if your pages have complex layouts.
     * This setting defaults to 1.</p>
     *
     * @param limit How many pages will be kept offscreen in an idle state.
     */
//     public void setOffscreenPageLimit(int limit) {
//         if (limit < DEFAULT_OFFSCREEN_PAGES) {
//             Log.w(TAG, "Requested offscreen page limit " + limit + " too small; defaulting to " +
//                     DEFAULT_OFFSCREEN_PAGES);
//             limit = DEFAULT_OFFSCREEN_PAGES;
//         }
//         if (limit != mOffscreenPageLimit) {
//             mOffscreenPageLimit = limit;
//             populate();
//         }
//     }
    //ViewPager缓存当前显示的页面左右两边的页面，
    //这个页面个数默认为左右两边各1个（如果左右都有至少1个的话），
    //设定当前显示的页面左右两边保持的页面个数。
    SetOffscreenPageLimit(limit) {
        if (limit < DEFAULT_OFFSCREEN_PAGES) {
            Log.w(TAG, "Requested offscreen page limit " + limit + " too small; defaulting to " +
                    ViewPager.DEFAULT_OFFSCREEN_PAGES);
            limit = ViewPager.DEFAULT_OFFSCREEN_PAGES;
        }
        if (limit != this.mOffscreenPageLimit) {
            this.mOffscreenPageLimit = limit;
            //每次改变缓存数量后也会调用populate函数。
            populate();
        }
    }

    /**
     * Set the margin between pages.
     *
     * @param marginPixels Distance between adjacent pages in pixels
     * @see #getPageMargin()
     * @see #setPageMarginDrawable(Drawable)
     * @see #setPageMarginDrawable(int)
     */
//     public void setPageMargin(int marginPixels) {
//         final int oldMargin = mPageMargin;
//         mPageMargin = marginPixels;

//         final int width = getWidth();
//         recomputeScrollPosition(width, width, marginPixels, oldMargin);

//         requestLayout();
//     }
    SetPageMargin(marginPixels) {
        var _this = this._obj;

        var oldMargin = this.mPageMargin;
        this.mPageMargin = marginPixels;

        var width = this._obj.GetWidth();
        this.RecomputeScrollPosition(width, width, marginPixels, oldMargin);

        _this.RequestLayout();
    }

//     /**
//      * Return the margin between pages.
//      *
//      * @return The size of the margin in pixels
//      */
//     public int getPageMargin() {
//         return mPageMargin;
//     }
    GetPageMargin() {
        return this.mPageMargin;
    }

    /**
     * Set a drawable that will be used to fill the margin between pages.
     *
     * @param d Drawable to display between pages
     */
//     public void setPageMarginDrawable(Drawable d) {
//         mMarginDrawable = d;
//         if (d != null) refreshDrawableState();
//         setWillNotDraw(d == null);
//         invalidate();
//     }
    _SetPageMarginDrawable(d) {
        this.mMarginDrawable = d;
        if (d != null) this.RefreshDrawableState();
        this.SetWillNotDraw(d == null);
        this.Invalidate();
    }

    /**
     * Set a drawable that will be used to fill the margin between pages.
     *
     * @param resId Resource ID of a drawable to display between pages
     */
//     public void setPageMarginDrawable(@DrawableRes int resId) {
    SetPageMarginDrawable(resId) {
        this._SetPageMarginDrawable(getContext().getResources().getDrawable(resId));
    }

//     @Override
//     protected boolean verifyDrawable(Drawable who) {
//         return super.verifyDrawable(who) || who == mMarginDrawable;
//    }
    VerifyDrawable(_this, who, result) {

        elog("====ViewPager::VerifyDrawable====begin====");

        result.data = _this._VerifyDrawable(who) || who == this.mMarginDrawable;

        elog("====ViewPager::VerifyDrawable====end====");
    }

//     @Override
//     protected void drawableStateChanged() {
//         super.drawableStateChanged();
//         final Drawable d = mMarginDrawable;
//         if (d != null && d.isStateful()) {
//             d.setState(getDrawableState());
//         }
//     }
    DrawableStateChanged(_this) {
        elog("====ViewPager::DrawableStateChanged====begin====");
        _this._DrawableStateChanged();
        var d = this.mMarginDrawable;
        if (d != null && d.IsStateful()) {
            d.SetState(_this.GetDrawableState());
        }
        elog("====ViewPager::DrawableStateChanged====end====");
    }

    // We want the duration of the page snap animation to be influenced by the distance that
    // the screen has to travel, however, we don't want this duration to be effected in a
    // purely linear fashion. Instead, we use this method to moderate the effect that the distance
    // of travel has on the overall snap duration.
//     float distanceInfluenceForSnapDuration(float f) {
//         f -= 0.5f; // center the values about 0.
//         f *= 0.3f * Math.PI / 2.0f;
//         return (float) Math.sin(f);
//     }
    DistanceInfluenceForSnapDuration(f) {
        f -= 0.5; // center the values about 0.
        f *= 0.3 * Math.PI / 2.0;
        return Math.sin(f);
    }

    /**
     * Like {@link View#scrollBy}, but scroll smoothly instead of immediately.
     *
     * @param x the number of pixels to scroll by on the X axis
     * @param y the number of pixels to scroll by on the Y axis
     */
//     void smoothScrollTo(int x, int y) {
//         smoothScrollTo(x, y, 0);
//     }
    SmoothScrollTo(x, y) {
        this.SmoothScrollTo(x, y, 0);
    }

    /**
     * Like {@link View#scrollBy}, but scroll smoothly instead of immediately.
     *
     * @param x the number of pixels to scroll by on the X axis
     * @param y the number of pixels to scroll by on the Y axis
     * @param velocity the velocity associated with a fling, if applicable. (0 otherwise)
     */
//     void smoothScrollTo(int x, int y, int velocity) {
//         if (getChildCount() == 0) {
//             // Nothing to do.
//             setScrollingCacheEnabled(false);
//             return;
//         }
//         int sx = getScrollX();
//         int sy = getScrollY();
//         int dx = x - sx;
//         int dy = y - sy;
//         if (dx == 0 && dy == 0) {
//             completeScroll(false);
//             populate();
//             setScrollState(SCROLL_STATE_IDLE);
//             return;
//         }

//         setScrollingCacheEnabled(true);
//         setScrollState(SCROLL_STATE_SETTLING);

//         final int width = getClientWidth();
//         final int halfWidth = width / 2;
//         final float distanceRatio = Math.min(1f, 1.0f * Math.abs(dx) / width);
//         final float distance = halfWidth + halfWidth *
//                 distanceInfluenceForSnapDuration(distanceRatio);

//         int duration = 0;
//         velocity = Math.abs(velocity);
//         if (velocity > 0) {
//             duration = 4 * Math.round(1000 * Math.abs(distance / velocity));
//         } else {
//             final float pageWidth = width * mAdapter.getPageWidth(mCurItem);
//             final float pageDelta = (float) Math.abs(dx) / (pageWidth + mPageMargin);
//             duration = (int) ((pageDelta + 1) * 100);
//         }
//         duration = Math.min(duration, MAX_SETTLE_DURATION);

//         mScroller.startScroll(sx, sy, dx, dy, duration);
//         ViewCompat.postInvalidateOnAnimation(this);
//     }
    SmoothScrollTo(x, y, velocity) {
        if (getChildCount() == 0) {
            // Nothing to do.
            setScrollingCacheEnabled(false);
            return;
        }
        var sx = getScrollX();
        var sy = getScrollY();
        var dx = x - sx;
        var dy = y - sy;
        if (dx == 0 && dy == 0) {
            this.CompleteScroll(false);
            populate();
            setScrollState(ViewPager.SCROLL_STATE_IDLE);
            return;
        }

        setScrollingCacheEnabled(true);
        setScrollState(ViewPager.SCROLL_STATE_SETTLING);

        var width = getClientWidth();
        var halfWidth = width / 2;
        var distanceRatio = Math.min(1, 1.0 * Math.abs(dx) / width);
        var distance = halfWidth + halfWidth *
                distanceInfluenceForSnapDuration(distanceRatio);

        var duration = 0;
        velocity = Math.abs(velocity);
        if (velocity > 0) {
            duration = 4 * Math.round(1000 * Math.abs(distance / velocity));
        } else {
            var pageWidth = width * this.mAdapter.GetPageWidth(this.mCurItem);
            var pageDelta = Math.abs(dx) / (pageWidth + this.mPageMargin);
            duration = (pageDelta + 1) * 100;
        }
        duration = Math.min(duration, ViewPager.MAX_SETTLE_DURATION);

        this.mScroller.StartScroll(sx, sy, dx, dy, duration);
        ViewCompat.postInvalidateOnAnimation(this);
    }

//     ItemInfo addNewItem(int position, int index) {
//         ItemInfo ii = new ItemInfo();
//         ii.position = position;
//         ii.object = mAdapter.instantiateItem(this, position);
//         ii.widthFactor = mAdapter.getPageWidth(position);
//         if (index < 0 || index >= mItems.size()) {
//             mItems.add(ii);
//         } else {
//             mItems.add(index, ii);
//         }
//         return ii;
//     }
    AddNewItem(position, index) {
        elog("====ViewPager::AddNewItem====begin====position:"+position+"==index:"+index);

        var _this = this.obj;

        elog("====ViewPager::AddNewItem====0====");
        var ii = new ViewPager.ItemInfo();
        elog("====ViewPager::AddNewItem====1====");

ii.tttt=1234;
        ii.position = position;
        ii.object = this.mAdapter.InstantiateItem(_this, position);
        ii.widthFactor = this.mAdapter.GetPageWidth(position);
        elog("====ViewPager::AddNewItem====1.0====widthFactor:"+ii.widthFactor);

        if (index < 0 || index >= this.mItems.GetSize()) {
elog("====ViewPager::AddNewItem====2.1====ii:"+ii.tttt);
            this.mItems.Add(ii);
        } else {
elog("====ViewPager::AddNewItem====2.2====ii:"+ii.tttt);
            this.mItems.Add(index, ii);
        }

elog("====ViewPager::AddNewItem====3.1====ii:"+ii.tttt);
elog("====ViewPager::AddNewItem====3.2====index:"+index);
elog("====ViewPager::AddNewItem====3.3====size:"+this.mItems.GetSize());

var jj =  this.mItems.Get(index);

elog("====ii class:"+CObject.getClassName(ii) );
elog("====jj class:"+CObject.getClassName(jj) );

elog("====ViewPager::AddNewItem====4====jj:"+jj.tttt);
//ss.tt();Assert(0);


        elog("====ViewPager::AddNewItem====item class name:" + CObject.getClassName(ii.object) );

        elog("====ViewPager::AddNewItem====end====");
        return ii;
    }

//     void dataSetChanged() {
//         // This method only gets called if our observer is attached, so mAdapter is non-null.

//         final int adapterCount = mAdapter.getCount();
//         mExpectedAdapterCount = adapterCount;
//         boolean needPopulate = mItems.size() < mOffscreenPageLimit * 2 + 1 &&
//                 mItems.size() < adapterCount;
//         int newCurrItem = mCurItem;

//         boolean isUpdating = false;
//         for (int i = 0; i < mItems.size(); i++) {
//             final ItemInfo ii = mItems.get(i);
//             final int newPos = mAdapter.getItemPosition(ii.object);

//             if (newPos == PagerAdapter.POSITION_UNCHANGED) {
//                 continue;
//             }

//             if (newPos == PagerAdapter.POSITION_NONE) {
//                 mItems.remove(i);
//                 i--;

//                 if (!isUpdating) {
//                     mAdapter.startUpdate(this);
//                     isUpdating = true;
//                 }

//                 mAdapter.destroyItem(this, ii.position, ii.object);
//                 needPopulate = true;

//                 if (mCurItem == ii.position) {
//                     // Keep the current item in the valid range
//                     newCurrItem = Math.max(0, Math.min(mCurItem, adapterCount - 1));
//                     needPopulate = true;
//                 }
//                 continue;
//             }

//             if (ii.position != newPos) {
//                 if (ii.position == mCurItem) {
//                     // Our current item changed position. Follow it.
//                     newCurrItem = newPos;
//                 }

//                 ii.position = newPos;
//                 needPopulate = true;
//             }
//         }

//         if (isUpdating) {
//             mAdapter.finishUpdate(this);
//         }

//         Collections.sort(mItems, COMPARATOR);

//         if (needPopulate) {
//             // Reset our known page widths; populate will recompute them.
//             final int childCount = getChildCount();
//             for (int i = 0; i < childCount; i++) {
//                 final View child = getChildAt(i);
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 if (!lp.isDecor) {
//                     lp.widthFactor = 0.f;
//                 }
//             }

//             setCurrentItemInternal(newCurrItem, false, true);
//             requestLayout();
//         }
//     }   //function dataSetChanged
    DataSetChanged() {
        // This method only gets called if our observer is attached, so mAdapter is non-null.

        elog("====ViewPager::DataSetChanged====begin====");

        var _this = this._obj;

        var adapterCount = this.mAdapter.GetCount();
        this.mExpectedAdapterCount = adapterCount;
        var needPopulate = this.mItems.GetSize() < this.mOffscreenPageLimit * 2 + 1 &&
                this.mItems.GetSize() < adapterCount;
        var newCurrItem = this.mCurItem;

        var isUpdating = false;
        for (var i = 0; i < this.mItems.GetSize(); i++) {
            var ii = this.mItems.Get(i);
            var newPos = this.mAdapter.GetItemPosition(ii.object);

            if (newPos == PagerAdapter.POSITION_UNCHANGED) {
                continue;
            }

            if (newPos == PagerAdapter.POSITION_NONE) {
                this.mItems.Remove(i);
                i--;

                if (!isUpdating) {
                    this.mAdapter.StartUpdate(this);
                    isUpdating = true;
                }

                this.mAdapter.DestroyItem(this, ii.position, ii.object);
                needPopulate = true;

                if (this.mCurItem == ii.position) {
                    // Keep the current item in the valid range
                    newCurrItem = Math.max(0, Math.min(this.mCurItem, adapterCount - 1));
                    needPopulate = true;
                }
                continue;
            }

            if (ii.position != newPos) {
                if (ii.position == this.mCurItem) {
                    // Our current item changed position. Follow it.
                    newCurrItem = newPos;
                }

                ii.position = newPos;
                needPopulate = true;
            }
        }

        if (isUpdating) {
            this.mAdapter.FinishUpdate(this);
        }

        Collections.Sort(this.mItems, ViewPager.COMPARATOR);

        if (needPopulate) {
            // Reset our known page widths; populate will recompute them.
            var childCount = _this.GetChildCount();
            for (var i = 0; i < childCount; i++) {
                var child = _this.GetChildAt(i);
                var lp = child.GetLayoutParams();
                if (!lp.isDecor) {
                    lp.widthFactor = 0;
                }
            }

            this.SetCurrentItemInternal(newCurrItem, false, true);
            _this.RequestLayout();
        }

        elog("====ViewPager::DataSetChanged====end====");
    }   //function dataSetChanged

//     void populate() {
//         populate(mCurItem);
//     }

//     void populate(int newCurrentItem) {

//         ItemInfo oldCurInfo = null;
//         int focusDirection = View.FOCUS_FORWARD;
//         if (mCurItem != newCurrentItem) {
//             focusDirection = mCurItem < newCurrentItem ? View.FOCUS_RIGHT : View.FOCUS_LEFT;
//             oldCurInfo = infoForPosition(mCurItem);
//             mCurItem = newCurrentItem;
//         }

//         if (mAdapter == null) {
//             sortChildDrawingOrder();
//             return;
//         }

        // Bail now if we are waiting to populate.  This is to hold off
        // on creating views from the time the user releases their finger to
        // fling to a new position until we have finished the scroll to
        // that position, avoiding glitches from happening at that point.
//         if (mPopulatePending) {
//             if (DEBUG) Log.i(TAG, "populate is pending, skipping for now...");
//             sortChildDrawingOrder();
//             return;
//         }

        // Also, don't populate until we are attached to a window.  This is to
        // avoid trying to populate before we have restored our view hierarchy
        // state and conflicting with what is restored.
//         if (getWindowToken() == null) {
//             return;
//         }

//         mAdapter.startUpdate(this);

//         final int pageLimit = mOffscreenPageLimit;
//         final int startPos = Math.max(0, mCurItem - pageLimit);
//         final int N = mAdapter.getCount();
//         final int endPos = Math.min(N-1, mCurItem + pageLimit);

//         if (N != mExpectedAdapterCount) {
//             String resName;
//             try {
//                 resName = getResources().getResourceName(getId());
//             } catch (Resources.NotFoundException e) {
//                 resName = Integer.toHexString(getId());
//             }
//             throw new IllegalStateException("The application's PagerAdapter changed the adapter's" +
//                     " contents without calling PagerAdapter#notifyDataSetChanged!" +
//                     " Expected adapter item count: " + mExpectedAdapterCount + ", found: " + N +
//                     " Pager id: " + resName +
//                     " Pager class: " + getClass() +
//                     " Problematic adapter: " + mAdapter.getClass());
//         }

        // Locate the currently focused item or add it if needed.
//         int curIndex = -1;
//         ItemInfo curItem = null;
//         for (curIndex = 0; curIndex < mItems.size(); curIndex++) {
//             final ItemInfo ii = mItems.get(curIndex);
//             if (ii.position >= mCurItem) {
//                 if (ii.position == mCurItem) curItem = ii;
//                 break;
//             }
//         }

//         if (curItem == null && N > 0) {
//             curItem = addNewItem(mCurItem, curIndex);
//         }

        // Fill 3x the available width or up to the number of offscreen
        // pages requested to either side, whichever is larger.
        // If we have no current item we have no work to do.
//         if (curItem != null) {
//             float extraWidthLeft = 0.f;
//             int itemIndex = curIndex - 1;
//             ItemInfo ii = itemIndex >= 0 ? mItems.get(itemIndex) : null;
//             final int clientWidth = getClientWidth();
//             final float leftWidthNeeded = clientWidth <= 0 ? 0 :
//                     2.f - curItem.widthFactor + (float) getPaddingLeft() / (float) clientWidth;

//             for (int pos = mCurItem - 1; pos >= 0; pos--) {
//                 if (extraWidthLeft >= leftWidthNeeded && pos < startPos) {
//                     if (ii == null) {
//                         break;
//                     }
//                     if (pos == ii.position && !ii.scrolling) {
//                         mItems.remove(itemIndex);
//                         mAdapter.destroyItem(this, pos, ii.object);
//                         if (DEBUG) {
//                             Log.i(TAG, "populate() - destroyItem() with pos: " + pos +
//                                     " view: " + ((View) ii.object));
//                         }
//                         itemIndex--;
//                         curIndex--;
//                         ii = itemIndex >= 0 ? mItems.get(itemIndex) : null;
//                     }
//                 } else if (ii != null && pos == ii.position) {
//                     extraWidthLeft += ii.widthFactor;
//                     itemIndex--;
//                     ii = itemIndex >= 0 ? mItems.get(itemIndex) : null;
//                 } else {
//                     ii = addNewItem(pos, itemIndex + 1);
//                     extraWidthLeft += ii.widthFactor;
//                     curIndex++;
//                     ii = itemIndex >= 0 ? mItems.get(itemIndex) : null;
//                 }
//             }

//             float extraWidthRight = curItem.widthFactor;
//             itemIndex = curIndex + 1;

//             if (extraWidthRight < 2.f) {
//                 ii = itemIndex < mItems.size() ? mItems.get(itemIndex) : null;
//                 final float rightWidthNeeded = clientWidth <= 0 ? 0 :
//                         (float) getPaddingRight() / (float) clientWidth + 2.f;
//                 for (int pos = mCurItem + 1; pos < N; pos++) {
//                     if (extraWidthRight >= rightWidthNeeded && pos > endPos) {
//                         if (ii == null) {
//                             break;
//                         }
//                         if (pos == ii.position && !ii.scrolling) {
//                             mItems.remove(itemIndex);
//                             mAdapter.destroyItem(this, pos, ii.object);
//                             if (DEBUG) {
//                                 Log.i(TAG, "populate() - destroyItem() with pos: " + pos +
//                                         " view: " + ((View) ii.object));
//                             }
//                             ii = itemIndex < mItems.size() ? mItems.get(itemIndex) : null;
//                         }
//                     } else if (ii != null && pos == ii.position) {
//                         extraWidthRight += ii.widthFactor;
//                         itemIndex++;
//                         ii = itemIndex < mItems.size() ? mItems.get(itemIndex) : null;
//                     } else {
//                         ii = addNewItem(pos, itemIndex);
//                         itemIndex++;
//                         extraWidthRight += ii.widthFactor;
//                         ii = itemIndex < mItems.size() ? mItems.get(itemIndex) : null;
//                     }
//                 }
//             }

//             calculatePageOffsets(curItem, curIndex, oldCurInfo);

//         if (DEBUG) {
//             Log.i(TAG, "Current page list:");
//             for (int i=0; i<mItems.size(); i++) {
//                 Log.i(TAG, "#" + i + ": page " + mItems.get(i).position);
//             }
//         }

//         mAdapter.setPrimaryItem(this, mCurItem, curItem != null ? curItem.object : null);

//         mAdapter.finishUpdate(this);

        // Check width measurement of current pages and drawing sort order.
        // Update LayoutParams as needed.
//         final int childCount = getChildCount();
//         for (int i = 0; i < childCount; i++) {
//             final View child = getChildAt(i);
//             final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//             lp.childIndex = i;
//             if (!lp.isDecor && lp.widthFactor == 0.f) {
//                 // 0 means requery the adapter for this, it doesn't have a valid width.
//                 final ItemInfo ii = infoForChild(child);
//                 if (ii != null) {
//                     lp.widthFactor = ii.widthFactor;
//                     lp.position = ii.position;
//                 }
//             }
//         }
//         sortChildDrawingOrder();

//         if (hasFocus()) {
//             View currentFocused = findFocus();
//             ItemInfo ii = currentFocused != null ? infoForAnyChild(currentFocused) : null;
//             if (ii == null || ii.position != mCurItem) {
//                 for (int i=0; i<getChildCount(); i++) {
//                     View child = getChildAt(i);
//                     ii = infoForChild(child);
//                     if (ii != null && ii.position == mCurItem) {
//                         if (child.requestFocus(focusDirection)) {
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//     }
    Populate(newCurrentItem) {
        elog("====ViewPager::Populate====begin====");

        var _this = this._obj;

        newCurrentItem = newCurrentItem || this.mCurItem;

        var oldCurInfo = null;
        var focusDirection = IView__FOCUS_FORWARD;
        if (this.mCurItem != newCurrentItem) {
            focusDirection = this.mCurItem < newCurrentItem ? IView__FOCUS_RIGHT : IView__FOCUS_LEFT;
            oldCurInfo = this.InfoForPosition(this.mCurItem);
            this.mCurItem = newCurrentItem;
        }
        elog("====ViewPager::Populate====1====");

        if (this.mAdapter == null) {
            //对子View的绘制顺序进行排序，优先绘制Decor View
            //再按照position从小到大排序
            this.SortChildDrawingOrder();
            elog("====ViewPager::Populate====end====0====");
            return;
        }
        elog("====ViewPager::Populate====2====");

        //如果我们正在等待populate,那么在用户手指抬起切换到新的位置期间应该推迟创建子View，
        // 直到滚动到最终位置再去创建，以免在这个期间出现差错
        if (this.mPopulatePending) {
            if (ViewPager.DEBUG) Log.i(TAG, "populate is pending, skipping for now...");
            //对子View的绘制顺序进行排序，优先绘制Decor View
            //再按照position从小到大排序
            this.SortChildDrawingOrder();
            elog("====ViewPager::Populate====end====1====");
            return;
        }
        elog("====ViewPager::Populate====3====");

        //同样，在ViewPager没有attached到window之前，不要populate.
        // 这是因为如果我们在恢复View的层次结构之前进行populate，可能会与要恢复的内容有冲突
        if (_this.GetWindowToken() == null) {
            elog("====ViewPager::Populate====end====2====");
            return;
        }
        elog("====ViewPager::Populate====4====");

        //回调PagerAdapter的startUpdate函数，
        // 告诉PagerAdapter开始更新要显示的页面
        this.mAdapter.StartUpdate(_this);

        var pageLimit = this.mOffscreenPageLimit;
        //确保起始位置大于等于0，如果用户设置了缓存页面数量，第一个页面为当前页面减去缓存页面数量
        var startPos = Math.max(0, this.mCurItem - pageLimit);
        //保存数据源中的数据个数
        var N = this.mAdapter.GetCount();
        //确保最后的位置小于等于数据源中数据个数-1，
        // 如果用户设置了缓存页面数量，第一个页面为当前页面加缓存页面数量
        var endPos = Math.min(N-1, this.mCurItem + pageLimit);
        elog("====ViewPager::Populate====5====");

        elog(`====ViewPager::Populate====5.0====N:${N}====Cnt:${this.mExpectedAdapterCount}`);
        //Assert(0);

        //判断用户是否增减了数据源的元素，如果增减了且没有调用notifyDataSetChanged，则抛出异常
        if (N != this.mExpectedAdapterCount) {
            //resName用于抛异常显示
            var resName;
            try {
                resName = _this.GetResources().GetResourceName(_this.GetId());
            } catch (e) {
                resName = Integer.toHexString(_this.GetId());
            }
            elog("The application's PagerAdapter changed the adapter's" +
                    " contents without calling PagerAdapter#notifyDataSetChanged!" +
                    " Expected adapter item count: " + this.mExpectedAdapterCount + ", found: " + N +
                    " Pager id: " + resName +
                    " Pager class: " + _this.getClass().GetName() +
                    " Problematic adapter: " + this.mAdapter.name);
            elog("====ViewPager::Populate====end====3====");
            ss.tt();Assert(0);
            return;
            //this.mExpectedAdapterCount = N;
        }
        elog("====ViewPager::Populate====6====");

// CObject.showMethods(this.mItems);
// elog("====ViewPager::Populate====6.1====GetSize:"+this.mItems.GetSize());
// if(this.mItems.GetSize()>0){
// elog("====ViewPager::Populate====6.1====Get:"+this.mItems.Get(0));
// }

        var maxIndex = this.mItems.GetSize();
        elog("====ViewPager::Populate====6.1====GetSize:"+maxIndex);

        //定位到当前获焦的页面，如果没有的话，则添加一个
        var curIndex = -1;
        var curItem = null;
        //遍历每个页面对应的ItemInfo，找出获焦页面
        for (curIndex = 0; curIndex < maxIndex; curIndex++) {
            var ii = this.mItems.Get(curIndex);

//CObject.showMethods(ii);
elog("====ViewPager::Populate====6.2====ii.position:"+ii.position);
//ii.position = 2;
//elog("====ViewPager::Populate====6.3====ii.position:"+ii.position);

            //找到当前页面对应的ItemInfo后，跳出循环
            if (ii.position >= this.mCurItem) {
                if (ii.position == this.mCurItem) curItem = ii;
                break;
            }
        }
        elog("====ViewPager::Populate====7====");

        //如果没有找到获焦的页面，说明mItems列表里面没有保存获焦页面，
        // 需要将获焦页面加入到mItems里面
        if (curItem == null && N > 0) {
            elog("====ViewPager::Populate====7.1====");
            curItem = this.AddNewItem(this.mCurItem, curIndex);
        }
        elog("====ViewPager::Populate====8====");

        //默认缓存当前页面的左右两边的页面，如果用户设定了缓存页面数量，
        // 则将当前页面两边都缓存用户指定的数量的页面
        //如果当前没有页面，则我们啥也不需要做
        if (curItem != null) {
            elog("====ViewPager::Populate====8.1====curItem className:"+CObject.getClassName(curItem.object) );
            elog("====ViewPager::Populate====8.2====curItem JSON:"+JSON.stringify(curItem) );
            CObject.showMethods(curItem);

            var extraWidthLeft = 0;
            //左边的页面
            var itemIndex = curIndex - 1;
            elog("====ViewPager::Populate====8.3====itemIndex:"+itemIndex);
            //如果当前页面左边有页面，则将左边页面对应的ItemInfo取出，否则左边页面的ItemInfo为null
            var ii = itemIndex >= 0 ? this.mItems.Get(itemIndex) : null;
            //保存显示区域的宽度
            var clientWidth = this.GetClientWidth();
            elog("====ViewPager::Populate====8.4====clientWidth:"+clientWidth);
            //算出左边页面需要的宽度，注意，这里的宽度是指实际宽度与可视区域宽度比例，
            // 即实际宽度=leftWidthNeeded*clientWidth
            var leftWidthNeeded = clientWidth <= 0 ? 0 :
                    2 - curItem.widthFactor + _this.GetPaddingLeft() / clientWidth;
            elog("====ViewPager::Populate====8.5====leftWidthNeeded:"+leftWidthNeeded);

            elog("====ViewPager::Populate====9====mCurItem:"+this.mCurItem);
            //从当前页面左边第一个页面开始，左边的页面进行遍历
            for (var pos = this.mCurItem - 1; pos >= 0; pos--) {
                elog("====ViewPager::Populate====9.1====");
                //如果左边的宽度超过了所需的宽度，并且当前当前页面位置比第一个缓存页面位置小
                //这说明这个页面需要Destroy掉
                if (extraWidthLeft >= leftWidthNeeded && pos < startPos) {
                    //如果左边已经没有页面了，跳出循环
                    if (ii == null) {
                        break;
                    }
                    //将当前页面destroy掉
                    if (pos == ii.position && !ii.scrolling) {
                        this.mItems.Remove(itemIndex);
                        //回调PagerAdapter的destroyItem
                        this.mAdapter.DestroyItem(this, pos, ii.object);
                        if (ViewPager.DEBUG) {
                            elog(TAG + "populate() - destroyItem() with pos: " + pos +
                                    " view: " + ii.object);
                        }
                        //由于mItems删除了一个元素, 需要将索引减一
                        itemIndex--;
                        curIndex--;
                        ii = itemIndex >= 0 ? this.mItems.Get(itemIndex) : null;
                    }
                } else if (ii != null && pos == ii.position) {
                    //如果当前位置是需要缓存的位置，并且这个位置上的页面已经存在
                    //则将左边宽度加上当前位置的页面
                    extraWidthLeft += ii.widthFactor;
                    //mItems往左遍历
                    itemIndex--;
                    //ii设置为当前遍历的页面的左边一个页面
                    ii = itemIndex >= 0 ? this.mItems.Get(itemIndex) : null;
                } else {
                    //如果当前位置是需要缓存，并且这个位置没有页面
                    //需要添加一个ItemInfo,而addNewItem是通过PagerAdapter的instantiateItem获取对象
                    ii = this.AddNewItem(pos, itemIndex + 1);
                    //将左边宽度加上当前位置的页面
                    extraWidthLeft += ii.widthFactor;
                    //由于新加了一个元素，当前的索引号需要加1
                    curIndex++;
                    //ii设置为当前遍历的页面的左边一个页面
                    ii = itemIndex >= 0 ? this.mItems.Get(itemIndex) : null;
                }
            }
            elog("====ViewPager::Populate====10====");

            var extraWidthRight = curItem.widthFactor;
            itemIndex = curIndex + 1;

            //同理，右边需要添加缓存的页面
            if (extraWidthRight < 2) {
                elog("====ViewPager::Populate====10.1====");
                ii = itemIndex < this.mItems.GetSize() ? this.mItems.Get(itemIndex) : null;
                var rightWidthNeeded = clientWidth <= 0 ? 0 :
                        _this.GetPaddingRight() / clientWidth + 2.0;
                for (var pos = this.mCurItem + 1; pos < N; pos++) {
                    elog("====ViewPager::Populate====10.2====");
                    if (extraWidthRight >= rightWidthNeeded && pos > endPos) {
                        if (ii == null) {
                            break;
                        }
                        if (pos == ii.position && !ii.scrolling) {
                            this.mItems.Remove(itemIndex);
                            this.mAdapter.DestroyItem(this, pos, ii.object);
                            if (ViewPager.DEBUG) {
                                elog(TAG + "populate() - destroyItem() with pos: " + pos +
                                        " view: " + ii.object);
                            }
                            ii = itemIndex < this.mItems.GetSize() ? this.mItems.Get(itemIndex) : null;
                        }
                    } else if (ii != null && pos == ii.position) {
                        extraWidthRight += ii.widthFactor;
                        itemIndex++;
                        ii = itemIndex < this.mItems.GetSize() ? this.mItems.Get(itemIndex) : null;
                    } else {
                        ii = this.AddNewItem(pos, itemIndex);
                        itemIndex++;
                        extraWidthRight += ii.widthFactor;
                        ii = itemIndex < this.mItems.GetSize() ? this.mItems.Get(itemIndex) : null;
                    }
                }
            }
            elog("====ViewPager::Populate====11====");

            this.CalculatePageOffsets(curItem, curIndex, oldCurInfo);
        }
        elog("====ViewPager::Populate====12====");

        if (ViewPager.DEBUG) {
            elog(TAG + "Current page list:");
            for (var i=0; i<this.mItems.GetSize(); i++) {
                elog(TAG + "#" + i + ": page " + this.mItems.Get(i).position);
            }
        }

        //回调PagerAdapter的setPrimaryItem，告诉PagerAdapter当前显示的页面
        this.mAdapter.SetPrimaryItem(this, this.mCurItem, curItem != null ? curItem.object : null);
        //回调PagerAdapter的finishUpdate，告诉PagerAdapter页面更新结束
        this.mAdapter.FinishUpdate(this);

        //检查页面的宽度是否测量，如果页面的LayoutParams数据没有设定，则去重新设定好
        var childCount = _this.GetChildCount();
        elog("====ViewPager::Populate====13====childCount:"+childCount);

        for (var i = 0; i < childCount; i++) {
            elog("====ViewPager::Populate====13.1===="+i+"/"+childCount);
            var child = _this.GetChildAt(i);
            var lp = child.GetLayoutParams();
            lp.childIndex = i;

            elog("====ViewPager::Populate====13.2====isDecor:"+lp.isDecor+"====widthFacor:"+lp.widthFactor);
            if (!lp.isDecor && lp.widthFactor == 0) {
                elog("====ViewPager::Populate====13.3====");
                // 0 means requery the adapter for this, it doesn't have a valid width.
                var ii = this.InfoForChild(child);
                if (ii != null) {
                    elog("====ViewPager::Populate====13.4====");
                    lp.widthFactor = ii.widthFactor;
                    lp.position = ii.position;
                    elog("====ViewPager::Populate====13.5====widthFactor:"+lp.widthFactor+"==lp.position:"+lp.position);
                }
            }
        }
        //重新对页面排序
        this.SortChildDrawingOrder();
        elog("====ViewPager::Populate====14====");

        //如果ViewPager被设定为可获焦的，则将当前显示的页面设定为获焦
        if (_this.HasFocus()) {
            elog("====ViewPager::Populate====14.1====");
            var currentFocused = _this.FindFocus();
            var ii = currentFocused != null ? this.InfoForAnyChild(currentFocused) : null;
            if (ii == null || ii.position != this.mCurItem) {
                for (var i=0; i<_this.GetChildCount(); i++) {
                    var child = _this.GetChildAt(i);
                    ii = this.InfoForChild(child);
                    if (ii != null && ii.position == this.mCurItem) {
                        if (child.RequestFocus(focusDirection)) {
                            break;
                        }
                    }
                }
            }
        }

        elog("====ViewPager::Populate====end====");
    }   //Populate

//     private void sortChildDrawingOrder() {
//         if (mDrawingOrder != DRAW_ORDER_DEFAULT) {
//             if (mDrawingOrderedChildren == null) {
//                 mDrawingOrderedChildren = new ArrayList<View>();
//             } else {
//                 mDrawingOrderedChildren.clear();
//             }
//             final int childCount = getChildCount();
//             for (int i = 0; i < childCount; i++) {
//                 final View child = getChildAt(i);
//                 mDrawingOrderedChildren.add(child);
//             }
//             Collections.sort(mDrawingOrderedChildren, sPositionComparator);
//         }
//     }
    SortChildDrawingOrder() {
        elog("====ViewPager::SortChildDrawingOrder====begin====");

        var _this = this._obj;

        if (this.mDrawingOrder != this.DRAW_ORDER_DEFAULT) {
            if (this.mDrawingOrderedChildren == null) {
                this.mDrawingOrderedChildren = Core_New("Elastos.Utility.CArrayList");
            } else {
                this.mDrawingOrderedChildren.Clear();
            }
            var childCount = _this.GetChildCount();

            elog("====ViewPager::SortChildDrawingOrder====1====childCount:"+childCount);
            for (var i = 0; i < childCount; i++) {
                var child = _this.GetChildAt(i);
                this.mDrawingOrderedChildren.Add(child);
            }
            Collections.Sort(this.mDrawingOrderedChildren, ViewPager.sPositionComparator);
        }

        elog("====ViewPager::SortChildDrawingOrder====end====");
    }   //sortChildDrawingOrder

//     private void calculatePageOffsets(ItemInfo curItem, int curIndex, ItemInfo oldCurInfo) {
//         final int N = mAdapter.getCount();
//         final int width = getClientWidth();
//         final float marginOffset = width > 0 ? (float) mPageMargin / width : 0;
//         // Fix up offsets for later layout.
//         if (oldCurInfo != null) {
//             final int oldCurPosition = oldCurInfo.position;
//             // Base offsets off of oldCurInfo.
//             if (oldCurPosition < curItem.position) {
//                 int itemIndex = 0;
//                 ItemInfo ii = null;
//                 float offset = oldCurInfo.offset + oldCurInfo.widthFactor + marginOffset;
//                 for (int pos = oldCurPosition + 1;
//                         pos <= curItem.position && itemIndex < mItems.size(); pos++) {
//                     ii = mItems.get(itemIndex);
//                     while (pos > ii.position && itemIndex < mItems.size() - 1) {
//                         itemIndex++;
//                         ii = mItems.get(itemIndex);
//                     }
//                     while (pos < ii.position) {
//                         // We don't have an item populated for this,
//                         // ask the adapter for an offset.
//                         offset += mAdapter.getPageWidth(pos) + marginOffset;
//                         pos++;
//                     }
//                     ii.offset = offset;
//                     offset += ii.widthFactor + marginOffset;
//                 }
//             } else if (oldCurPosition > curItem.position) {
//                 int itemIndex = mItems.size() - 1;
//                 ItemInfo ii = null;
//                 float offset = oldCurInfo.offset;
//                 for (int pos = oldCurPosition - 1;
//                         pos >= curItem.position && itemIndex >= 0; pos--) {
//                     ii = mItems.get(itemIndex);
//                     while (pos < ii.position && itemIndex > 0) {
//                         itemIndex--;
//                         ii = mItems.get(itemIndex);
//                     }
//                     while (pos > ii.position) {
//                         // We don't have an item populated for this,
//                         // ask the adapter for an offset.
//                         offset -= mAdapter.getPageWidth(pos) + marginOffset;
//                         pos--;
//                     }
//                     offset -= ii.widthFactor + marginOffset;
//                     ii.offset = offset;
//                 }
//             }
//         }

//         // Base all offsets off of curItem.
//         final int itemCount = mItems.size();
//         float offset = curItem.offset;
//         int pos = curItem.position - 1;
//         mFirstOffset = curItem.position == 0 ? curItem.offset : -Float.MAX_VALUE;
//         mLastOffset = curItem.position == N - 1 ?
//                 curItem.offset + curItem.widthFactor - 1 : Float.MAX_VALUE;
//         // Previous pages
//         for (int i = curIndex - 1; i >= 0; i--, pos--) {
//             final ItemInfo ii = mItems.get(i);
//             while (pos > ii.position) {
//                 offset -= mAdapter.getPageWidth(pos--) + marginOffset;
//             }
//             offset -= ii.widthFactor + marginOffset;
//             ii.offset = offset;
//             if (ii.position == 0) mFirstOffset = offset;
//         }
//         offset = curItem.offset + curItem.widthFactor + marginOffset;
//         pos = curItem.position + 1;
//         // Next pages
//         for (int i = curIndex + 1; i < itemCount; i++, pos++) {
//             final ItemInfo ii = mItems.get(i);
//             while (pos < ii.position) {
//                 offset += mAdapter.getPageWidth(pos++) + marginOffset;
//             }
//             if (ii.position == N - 1) {
//                 mLastOffset = offset + ii.widthFactor - 1;
//             }
//             ii.offset = offset;
//             offset += ii.widthFactor + marginOffset;
//         }

//         mNeedCalculatePageOffsets = false;
//     }   //CalculatePageOffsets

    CalculatePageOffsets(curItem, curIndex, oldCurInfo) {
        elog("====ViewPager::CalculatePageOffsets====begin====TODO====");

        var N = this.mAdapter.GetCount();
        var width = this.GetClientWidth();
        var marginOffset = width > 0 ? this.mPageMargin / width : 0;
        // Fix up offsets for later layout.
        if (oldCurInfo != null) {
            var oldCurPosition = oldCurInfo.position;
            // Base offsets off of oldCurInfo.
            if (oldCurPosition < curItem.position) {
                var itemIndex = 0;
                var ii = null;
                var offset = oldCurInfo.offset + oldCurInfo.widthFactor + marginOffset;
                for (var pos = oldCurPosition + 1;
                        pos <= curItem.position && itemIndex < mItems.GetSize(); pos++) {
                    ii = mItems.Get(itemIndex);
                    while (pos > ii.position && itemIndex < mItems.GetSize() - 1) {
                        itemIndex++;
                        ii = mItems.Get(itemIndex);
                    }
                    while (pos < ii.position) {
                        // We don't have an item populated for this,
                        // ask the adapter for an offset.
                        offset += this.mAdapter.GetPageWidth(pos) + marginOffset;
                        pos++;
                    }
                    ii.offset = offset;
                    offset += ii.widthFactor + marginOffset;
                }
            } else if (oldCurPosition > curItem.position) {
                var itemIndex = mItems.GetSize() - 1;
                var ii = null;
                var offset = oldCurInfo.offset;
                for (var pos = oldCurPosition - 1;
                        pos >= curItem.position && itemIndex >= 0; pos--) {
                    ii = mItems.Get(itemIndex);
                    while (pos < ii.position && itemIndex > 0) {
                        itemIndex--;
                        ii = mItems.Get(itemIndex);
                    }
                    while (pos > ii.position) {
                        // We don't have an item populated for this,
                        // ask the adapter for an offset.
                        offset -= this.mAdapter.GetPageWidth(pos) + marginOffset;
                        pos--;
                    }
                    offset -= ii.widthFactor + marginOffset;
                    ii.offset = offset;
                }
            }
        }

        // Base all offsets off of curItem.
        var itemCount = this.mItems.GetSize();
        var offset = curItem.offset;
        var pos = curItem.position - 1;

        //TODO
        var Float__MAX_VALUE = 0xEFFFFFFF;

        this.mFirstOffset = curItem.position == 0 ? curItem.offset : -Float__MAX_VALUE;
        this.mLastOffset = curItem.position == N - 1 ?
                curItem.offset + curItem.widthFactor - 1 : Float__MAX_VALUE;
        // Previous pages
        for (var i = curIndex - 1; i >= 0; i--, pos--) {
            var ii = this.mItems.Get(i);
            while (pos > ii.position) {
                offset -= this.mAdapter.GetPageWidth(pos--) + marginOffset;
            }
            offset -= ii.widthFactor + marginOffset;
            ii.offset = offset;
            if (ii.position == 0) this.mFirstOffset = offset;
        }
        offset = curItem.offset + curItem.widthFactor + marginOffset;
        pos = curItem.position + 1;
        // Next pages
        for (var i = curIndex + 1; i < itemCount; i++, pos++) {
            var ii = this.mItems.Get(i);
            while (pos < ii.position) {
                offset += this.mAdapter.GetPageWidth(pos++) + marginOffset;
            }
            if (ii.position == N - 1) {
                this.mLastOffset = offset + ii.widthFactor - 1;
            }
            ii.offset = offset;
            offset += ii.widthFactor + marginOffset;
        }

        this.mNeedCalculatePageOffsets = false;

        elog("====ViewPager::CalculatePageOffsets====end====");
    }   //CalculatePageOffsets

    /**
     * This is the persistent state that is saved by ViewPager.  Only needed
     * if you are creating a sublass of ViewPager that must save its own
     * state, in which case it should implement a subclass of this which
     * contains that state.
     */
//     public static class SavedState extends BaseSavedState {
    static get SavedState() {
        if (ViewPager._SavedState_) return ViewPager._SavedState;

        elog("====ViewPager::SavedState====begin====TODO====");

        ViewPager._SavedState = new class _ extends BaseSavedState {

//         int position;
//         Parcelable adapterState;
//         ClassLoader loader;

//         public SavedState(Parcelable superState) {
//             super(superState);
//         }
            SavedState(superState) {
                _super(superState);
            }

//         @Override
//         public void writeToParcel(Parcel out, int flags) {
//             super.writeToParcel(out, flags);
//             out.writeInt(position);
//             out.writeParcelable(adapterState, flags);
//         }
            WriteToParcel(out, flags) {
                super.writeToParcel(out, flags);
                out.writeInt(position);
                out.writeParcelable(adapterState, flags);
            }

//         @Override
//         public String toString() {
//             return "FragmentPager.SavedState{"
//                     + Integer.toHexString(System.identityHashCode(this))
//                     + " position=" + position + "}";
//         }
            ToString() {
                return "FragmentPager.SavedState{"
                        + Integer.toHexString(System.identityHashCode(this))
                        + " position=" + position + "}";
            }

//         public static final Parcelable.Creator<SavedState> CREATOR
//                 = ParcelableCompat.newCreator(new ParcelableCompatCreatorCallbacks<SavedState>() {
//                     @Override
//                     public SavedState createFromParcel(Parcel in, ClassLoader loader) {
//                         return new SavedState(in, loader);
//                     }
//                     @Override
//                     public SavedState[] newArray(int size) {
//                         return new SavedState[size];
//                     }
//                 });
            static get CREATOR() {
                if (_._CREATOR_) return _._CREATOR_;
                _._CREATOR_ = true;
                _._CREATOR = ParcelableCompat.newCreator(
                    new class _ extends ParcelableCompatCreatorCallbacks {
                        //@Override
                        CreateFromParcel(_in, loader) {
                            return new SavedState(_in, loader);
                        }
                        //@Override
                        NewArray(size) {
                            return new SavedState[size];
                        }
                    }()
                );
                return _._CREATOR;
            }

//         SavedState(Parcel in, ClassLoader loader) {
//             super(in);
//             if (loader == null) {
//                 loader = getClass().getClassLoader();
//             }
//             position = in.readInt();
//             adapterState = in.readParcelable(loader);
//             this.loader = loader;
//         }
            SavedState(_in, loader) {
                _super(_in);
                if (loader == null) {
                    loader = getClass().getClassLoader();
                }
                position = _in.readInt();
                adapterState = _in.readParcelable(loader);
                this.loader = loader;
            }
        }

        elog("====ViewPager::SavedState====end====");

        return ViewPager._SavedState;
    }   //static get SavedState

//     @Override
//     public Parcelable onSaveInstanceState() {
//         Parcelable superState = super.onSaveInstanceState();
//         SavedState ss = new SavedState(superState);
//         ss.position = mCurItem;
//         if (mAdapter != null) {
//             ss.adapterState = mAdapter.saveState();
//         }
//         return ss;
//     }
    OnSaveInstanceState(_this) {
        var superState = _this.OnSaveInstanceState();
        var ss = new SavedState(superState);
        ss.position = this.mCurItem;
        if (this.mAdapter != null) {
            ss.adapterState = this.mAdapter.saveState();
        }
        return ss;
    }

//     @Override
//     public void onRestoreInstanceState(Parcelable state) {
//         if (!(state instanceof SavedState)) {
//             super.onRestoreInstanceState(state);
//             return;
//         }

//         SavedState ss = (SavedState)state;
//         super.onRestoreInstanceState(ss.getSuperState());

//         if (mAdapter != null) {
//             mAdapter.restoreState(ss.adapterState, ss.loader);
//             setCurrentItemInternal(ss.position, false, true);
//         } else {
//             mRestoredCurItem = ss.position;
//             mRestoredAdapterState = ss.adapterState;
//             mRestoredClassLoader = ss.loader;
//         }
//     }
    OnRestoreInstanceState(_this, state) {
        if (!(state instanceof SavedState)) {
            _this._OnRestoreInstanceState(state);
            return;
        }

        var ss = state;
        _this._OnRestoreInstanceState(ss.getSuperState());

        if (this.mAdapter != null) {
            this.mAdapter.restoreState(ss.adapterState, ss.loader);
            setCurrentItemInternal(ss.position, false, true);
        } else {
            mRestoredCurItem = ss.position;
            mRestoredAdapterState = ss.adapterState;
            mRestoredClassLoader = ss.loader;
        }
    }

//     @Override
//     public void addView(View child, int index, ViewGroup.LayoutParams params) {
//         if (!checkLayoutParams(params)) {
//             params = generateLayoutParams(params);
//         }
//         final LayoutParams lp = (LayoutParams) params;
//         lp.isDecor |= child instanceof Decor;
//         if (mInLayout) {
//             if (lp != null && lp.isDecor) {
//                 throw new IllegalStateException("Cannot add pager decor view during layout");
//             }
//             lp.needsMeasure = true;
//             addViewInLayout(child, index, params);
//         } else {
//             super.addView(child, index, params);
//         }

//         if (USE_CACHE) {
//             if (child.getVisibility() != GONE) {
//                 child.setDrawingCacheEnabled(mScrollingCacheEnabled);
//             } else {
//                 child.setDrawingCacheEnabled(false);
//             }
//         }
//     }
    AddView(_this, child, index, params) {
        elog("====ViewPager::AddView====begin====");

elog("====ViewPager::AddView====_this className====" + _this.getClass().GetName());
elog("====ViewPager::AddView====child className====" + child.getClass().GetName());
CObject.showMethods(params);
elog("====ViewPager::AddView====0.1====");
if(params._obj){
    elog("====ViewPager::AddView====0.2====");
    CObject.showMethods(params._obj);
}
elog("====ViewPager::AddView====0.3====");
var lm = params.GetWidth();
elog("====ViewPager::AddView====0.4====width:"+lm);

        if (!this.CheckLayoutParams(params)) {
elog("====ViewPager::AddView====0====");
            var o = {};
            this.GenerateLayoutParams(_this, params, o);
            params = o.data;
elog("====ViewPager::AddView====1====");
        }

elog("====ViewPager::AddView====2====");

CObject.showMethods(params);

elog("====ViewPager::AddView====2.1====");

var oo = params._obj;

elog("====ViewPager::AddView====2.2====");

CObject.showMethods(oo);

elog("====ViewPager::AddView====2.3====");

        var lp = params;
        //lp.isDecor |= child instanceof this.Decor;
        //lp.isDecor = !!this.Decor;
        lp.isDecor = false;

elog("====ViewPager::AddView====3====lp.isDecor:"+lp.isDecor);

        if (this.mInLayout) {
elog("====ViewPager::AddView====4====");
            if (lp != null && lp.isDecor) {
                throw new IllegalStateException("Cannot add pager decor view during layout");
            }
elog("====ViewPager::AddView====5====");
            lp.needsMeasure = true;

            _this.AddViewInLayout(child, index, params);
elog("====ViewPager::AddView====6====");
        } else {
elog("====ViewPager::AddView====7====");
elog("====ViewPager::AddView====child ClassName====" + child.getClass().GetName());
elog("====ViewPager::AddView====8.0====");



var ooo = params._obj;

elog("====ViewPager::AddView====8.1====");

CObject.showMethods(ooo);

elog("====ViewPager::AddView====8.2====");


//s.t();
//Assert(0);


            _this._AddView(child, index, params);
            //this.Test_AddView(child, index, params);
elog("====ViewPager::AddView====8.1====");
        }

elog("====ViewPager::AddView====9====");

        if (ViewPager.USE_CACHE) {
elog("====ViewPager::AddView====10====");
            if (child.GetVisibility() != IView__GONE) {
elog("====ViewPager::AddView====11====");
                child.SetDrawingCacheEnabled(this.mScrollingCacheEnabled);
elog("====ViewPager::AddView====12====");
            } else {
elog("====ViewPager::AddView====13====");
                child.SetDrawingCacheEnabled(false);
elog("====ViewPager::AddView====14====");
            }
        }

        elog("====ViewPager::AddView====end====");
    }

//-----------test begin-----------
// ECode ViewGroup::AddView(
//     /* [in] */ IView* child,
//     /* [in] */ Int32 index,
//     /* [in] */ IViewGroupLayoutParams* params)
// {
//     //if (DBG) {
//     //    System.out.println(this + " addView");
//     //}

//     // addViewInner() will call child.requestLayout() when setting the new LayoutParams
//     // therefore, we call requestLayout() on ourselves before, so that the child's request
//     // will be blocked at our level
//     RequestLayout();
//     Invalidate(TRUE);
//     return AddViewInner(child, index, params, FALSE);
// }

Test_AddView(child, index, params){
    elog("====Test_AddView====begin====");

    var _this = this._obj;




var ooo = params._obj;

elog("====ViewPager::Test_AddView====0.1====");

CObject.showMethods(ooo);

elog("====ViewPager::Test_AddView====0.2====");


    _this.RequestLayout();
    elog("====Test_AddView====0====");


var ooo = params._obj;

elog("====ViewPager::Test_AddView====0.3====");

CObject.showMethods(ooo);

elog("====ViewPager::Test_AddView====0.4====");



    //CObject.showMethods(_this,"nvalidate");

    _this.Invalidate();

    elog("====Test_AddView====1====");


CObject.showMethods(ooo);

elog("====ViewPager::Test_AddView====1.1====");

var oooo = params._obj;

elog("====ViewPager::Test_AddView====1.2====");

CObject.showMethods(oooo);

elog("====ViewPager::Test_AddView====1.3====");

//s.t();
//Assert(0);


    var oRet = this.AddViewInner(child, index, params, false);

    elog("====Test_AddView====end====");
}

// ECode ViewGroup::AddViewInner(
//     /* [in] */ IView* child,
//     /* [in] */ Int32 index,
//     /* [in] */ IViewGroupLayoutParams* _params,
//     /* [in] */ Boolean preventRequestLayout)
// {
//     assert(child != NULL && "Child view cannot be NULL.");

//     if (mTransition != NULL) {
//         // Don't prevent other add transitions from completing, but cancel remove
//         // transitions to let them complete the process before we add to the container
//         mTransition->Cancel(ILayoutTransition::DISAPPEARING);
//     }

//     AutoPtr<IViewGroupLayoutParams> params = _params;
//     View* v = ((View*)child);
//     AutoPtr<IViewParent> parent;
//     v->GetParent((IViewParent**)&parent);
//     if (parent != NULL) {
// //        throw new IllegalStateException("The specified child already has a parent. " +
// //                "You must call removeView() on the child's parent first.");
//         Slogger::E("ViewGroup", "The specified child already has a parent. " \
//             "You must call removeView() on the child's parent first.");
//         return E_ILLEGAL_STATE_EXCEPTION;
//     }

//     if (mTransition != NULL) {
//         mTransition->AddChild(this, child);
//     }

//     if (!CheckLayoutParams(params)) {
//         params = GenerateLayoutParams(params);
//     }

//     if (preventRequestLayout) {
//         v->mLayoutParams = params;
//     }
//     else {
//         v->SetLayoutParams(params);
//     }

//     if (index < 0) {
//         index = mChildrenCount;
//     }

//     AddInArray(child, index);

//     // tell our children
//     if (preventRequestLayout) {
//         v->AssignParent(this);
//     }
//     else {
//         v->mParent = this;
//     }
//     Boolean hasFocus;
//     if (v->HasFocus(&hasFocus), hasFocus) {
//         AutoPtr<IView> focus;
//         v->FindFocus((IView**)&focus);
//         RequestChildFocus(child, focus);
//     }

//     AttachInfo* ai = mAttachInfo;
//     if (ai != NULL && (mGroupFlags & FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW) == 0) {
//         Boolean lastKeepOn = ai->mKeepScreenOn;
//         ai->mKeepScreenOn = FALSE;
//         v->DispatchAttachedToWindow(mAttachInfo, (mViewFlags & VISIBILITY_MASK));
//         if (ai->mKeepScreenOn) {
//             NeedGlobalAttributesUpdate(TRUE);
//         }
//         ai->mKeepScreenOn = lastKeepOn;
//     }
//     Boolean isLayoutDirectionInherited;
//     v->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
//     if (isLayoutDirectionInherited) {
//         child->ResetRtlProperties();
//     }

//     OnViewAdded(child);

//     if ((v->mViewFlags & DUPLICATE_PARENT_STATE) == DUPLICATE_PARENT_STATE) {
//         mGroupFlags |= FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE;
//     }
//     Boolean hasTransientState;
//     v->HasTransientState(&hasTransientState);
//     if (hasTransientState) {
//         ChildHasTransientStateChanged(child, TRUE);
//     }

//     Int32 lastVisibility;
//     child->GetVisibility(&lastVisibility);
//     if (lastVisibility != IView::GONE) {
//         NotifySubtreeAccessibilityStateChangedIfNeeded();
//     }

//     return NOERROR;
// }

AddViewInner(child, index, _params, preventRequestLayout) {
//     /* [in] */ IView* child,
//     /* [in] */ Int32 index,
//     /* [in] */ IViewGroupLayoutParams* _params,
//     /* [in] */ Boolean preventRequestLayout)
// {
    elog("====AddViewInner====begin====");

    CObject.showMethods(_params);
    elog("====AddViewInner====000====");
    var oo = _params._obj;
    elog("====AddViewInner====001====");
    CObject.showMethods(oo);
    elog("====AddViewInner====002====");

//s.t();
//Assert(0);


//     assert(child != NULL && "Child view cannot be NULL.");
    if (!child) {
        elog("====ViewPager::AddViewInner====error: Child view cannot be NULL.");
        return;
    }

    var _this = this._obj;

//     if (mTransition != NULL) {
//         // Don't prevent other add transitions from completing, but cancel remove
//         // transitions to let them complete the process before we add to the container
//         mTransition->Cancel(ILayoutTransition::DISAPPEARING);
//     }
    var mTransition = _this.GetLayoutTransition();
    var ILayoutTransition__DISAPPEARING = 3;
    elog("====AddViewInner====00====");
    if (mTransition) {
    elog("====AddViewInner====01====");
        // Don't prevent other add transitions from completing, but cancel remove
        // transitions to let them complete the process before we add to the container
        mTransition.Cancel(ILayoutTransition__DISAPPEARING);
    elog("====AddViewInner====02====");
    }
    elog("====AddViewInner====03====");

//     AutoPtr<IViewGroupLayoutParams> params = _params;
//     View* v = ((View*)child);
//     AutoPtr<IViewParent> parent;
//     v->GetParent((IViewParent**)&parent);
//     if (parent != NULL) {
// //        throw new IllegalStateException("The specified child already has a parent. " +
// //                "You must call removeView() on the child's parent first.");
//         Slogger::E("ViewGroup", "The specified child already has a parent. " \
//             "You must call removeView() on the child's parent first.");
//         return E_ILLEGAL_STATE_EXCEPTION;
//     }
    var params = _params;
    var v = child;
    var parent;
    elog("====AddViewInner====04====");
    parent = v.GetParent();
    elog("====AddViewInner====05====");
    if (parent) {
        elog("====parent className:"+parent.getClass().GetName());
        CObject.showMethods(parent);

//        throw new IllegalStateException("The specified child already has a parent. " +
//                "You must call removeView() on the child's parent first.");
        elog("ViewGroup==The specified child already has a parent. " +
            "You must call removeView() on the child's parent first.");
        //Assert(0);
        //return false;

//------------test begin-----------

//var info = parent.getClass();
var info = v.Probe();
CObject.showMethods(info);

var module = info.GetModuleInfo();
CObject.showMethods(module);

var obj = v.GetObject();
CObject.showMethods(obj);

var className = obj.ToString();
elog("====className:" + className);

Assert(0);

//------------test end-----------

    }
    elog("====AddViewInner====06====");

//     if (mTransition != NULL) {
//         mTransition->AddChild(this, child);
//     }
    if (mTransition) {
        elog("====AddViewInner====07====");
        mTransition.AddChild(_this, child);
        elog("====AddViewInner====08====");
    }

//     if (!CheckLayoutParams(params)) {
//         params = GenerateLayoutParams(params);
//     }
    if (!this.CheckLayoutParams(params)) {
        elog("====AddViewInner====09====");
        params = this.GenerateLayoutParams(params);
        elog("====AddViewInner====10====");
    }

//     if (preventRequestLayout) {
//         v->mLayoutParams = params;
//     }
//     else {
//         v->SetLayoutParams(params);
//     }
    elog("====AddViewInner====11====preventRequestLayout:"+preventRequestLayout);
    if (preventRequestLayout) {
        //TODO
        v.mLayoutParams = params;
    }
    else {
        elog("====AddViewInner====12===="+v.getClass().GetName());

        var oo = params._obj;
        CObject.showMethods(oo);
        elog("====AddViewInner====13====");

        v.SetLayoutParams(params);
        elog("====AddViewInner====14====");
    }
    elog("====AddViewInner====15====");

//     if (index < 0) {
//         index = mChildrenCount;
//     }
    if (index < 0) {
        index = _this.GetChildCount();
    }

//     AddInArray(child, index);
    _this.AddInArray(child, index);

    Assert(0);

//     // tell our children
//     if (preventRequestLayout) {
//         v->AssignParent(this);
//     }
//     else {
//         v->mParent = this;
//     }
//     Boolean hasFocus;
//     if (v->HasFocus(&hasFocus), hasFocus) {
//         AutoPtr<IView> focus;
//         v->FindFocus((IView**)&focus);
//         RequestChildFocus(child, focus);
//     }

//     AttachInfo* ai = mAttachInfo;
//     if (ai != NULL && (mGroupFlags & FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW) == 0) {
//         Boolean lastKeepOn = ai->mKeepScreenOn;
//         ai->mKeepScreenOn = FALSE;
//         v->DispatchAttachedToWindow(mAttachInfo, (mViewFlags & VISIBILITY_MASK));
//         if (ai->mKeepScreenOn) {
//             NeedGlobalAttributesUpdate(TRUE);
//         }
//         ai->mKeepScreenOn = lastKeepOn;
//     }
//     Boolean isLayoutDirectionInherited;
//     v->IsLayoutDirectionInherited(&isLayoutDirectionInherited);
//     if (isLayoutDirectionInherited) {
//         child->ResetRtlProperties();
//     }

//     OnViewAdded(child);

//     if ((v->mViewFlags & DUPLICATE_PARENT_STATE) == DUPLICATE_PARENT_STATE) {
//         mGroupFlags |= FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE;
//     }
//     Boolean hasTransientState;
//     v->HasTransientState(&hasTransientState);
//     if (hasTransientState) {
//         ChildHasTransientStateChanged(child, TRUE);
//     }

//     Int32 lastVisibility;
//     child->GetVisibility(&lastVisibility);
//     if (lastVisibility != IView::GONE) {
//         NotifySubtreeAccessibilityStateChangedIfNeeded();
//     }

//     return NOERROR;
}

//-----------test end-------------

//     @Override
//     public void removeView(View view) {
//         if (mInLayout) {
//             removeViewInLayout(view);
//         } else {
//             super.removeView(view);
//         }
//     }
    RemoveView(_this, view) {
        if (this.mInLayout) {
            removeViewInLayout(view);
        } else {
            _this._RemoveView(view);
        }
    }

//     ItemInfo infoForChild(View child) {
//         for (int i=0; i<mItems.size(); i++) {
//             ItemInfo ii = mItems.get(i);
//             if (mAdapter.isViewFromObject(child, ii.object)) {
//                 return ii;
//             }
//         }
//         return null;
//     }
    InfoForChild(child) {
        elog("====ViewPager::InfoForChild====begin====");

        var im = this.mItems.GetSize();
        elog("====ViewPager::InfoForChild====0====im:"+im);
        for (var i=0; i<im; i++) {
            var ii = this.mItems.Get(i);

            var n1 = ii.object.getClass().GetName();
            var n2 = child.getClass().GetName();
            var b1 = (child==ii.object);
            var b2 = this.mAdapter.IsViewFromObject(child, ii.object);
            elog("====ViewPager::InfoForChild====1====n1:"+n1);
            elog("====ViewPager::InfoForChild====2====n2:"+n2);
            elog("====ViewPager::InfoForChild====3====equal:"+b1);
            elog("====ViewPager::InfoForChild====4====equal:"+b2);

            //CObject.showMethods(ii);

            if (this.mAdapter.IsViewFromObject(child, ii.object)) {
                elog("====ViewPager::InfoForChild====end====0====");
                return ii;
            }
        }
        elog("====ViewPager::InfoForChild====end====1====");
        return null;
    }

//     ItemInfo infoForAnyChild(View child) {
//         ViewParent parent;
//         while ((parent=child.getParent()) != this) {
//             if (parent == null || !(parent instanceof View)) {
//                 return null;
//             }
//             child = (View)parent;
//         }
//         return infoForChild(child);
//     }
    InfoForAnyChild(child) {
        var parent;
        while ((parent=child.getParent()) != this) {
            if (parent == null || !(parent instanceof View)) {
                return null;
            }
            child = parent;
        }
        return this.InfoForChild(child);
    }

//     ItemInfo infoForPosition(int position) {
//         for (int i = 0; i < mItems.size(); i++) {
//             ItemInfo ii = mItems.get(i);
//             if (ii.position == position) {
//                 return ii;
//             }
//         }
//         return null;
//     }
    InfoForPosition(position) {
        for (var i = 0; i < this.mItems.GetSize(); i++) {
            var ii = this.mItems.Get(i);
            if (ii.position == position) {
                return ii;
            }
        }
        return null;
    }

//     @Override
//     protected void onAttachedToWindow() {
//         super.onAttachedToWindow();
//         mFirstLayout = true;
//     }
    OnAttachedToWindow(_this) {
        elog("====ViewPager::OnAttachedToWindow====begin====");

        _this._OnAttachedToWindow();
        this.mFirstLayout = true;

        elog("====ViewPager::OnAttachedToWindow====end====");
    }

//     @Override
//     protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
//         // For simple implementation, our internal size is always 0.
//         // We depend on the container to specify the layout size of
//         // our view.  We can't really know what it is since we will be
//         // adding and removing different arbitrary views and do not
//         // want the layout to change as this happens.
//         setMeasuredDimension(getDefaultSize(0, widthMeasureSpec),
//                 getDefaultSize(0, heightMeasureSpec));

//         final int measuredWidth = getMeasuredWidth();
//         final int maxGutterSize = measuredWidth / 10;
//         mGutterSize = Math.min(maxGutterSize, mDefaultGutterSize);

//         // Children are just made to fill our space.
//         int childWidthSize = measuredWidth - getPaddingLeft() - getPaddingRight();
//         int childHeightSize = getMeasuredHeight() - getPaddingTop() - getPaddingBottom();

//         /*
//          * Make sure all children have been properly measured. Decor views first.
//          * Right now we cheat and make this less complicated by assuming decor
//          * views won't intersect. We will pin to edges based on gravity.
//          */
//         int size = getChildCount();
//         for (int i = 0; i < size; ++i) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() != GONE) {
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 if (lp != null && lp.isDecor) {
//                     final int hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
//                     final int vgrav = lp.gravity & Gravity.VERTICAL_GRAVITY_MASK;
//                     int widthMode = MeasureSpec.AT_MOST;
//                     int heightMode = MeasureSpec.AT_MOST;
//                     boolean consumeVertical = vgrav == Gravity.TOP || vgrav == Gravity.BOTTOM;
//                     boolean consumeHorizontal = hgrav == Gravity.LEFT || hgrav == Gravity.RIGHT;

//                     if (consumeVertical) {
//                         widthMode = MeasureSpec.EXACTLY;
//                     } else if (consumeHorizontal) {
//                         heightMode = MeasureSpec.EXACTLY;
//                     }

//                     int widthSize = childWidthSize;
//                     int heightSize = childHeightSize;
//                     if (lp.width != LayoutParams.WRAP_CONTENT) {
//                         widthMode = MeasureSpec.EXACTLY;
//                         if (lp.width != LayoutParams.FILL_PARENT) {
//                             widthSize = lp.width;
//                         }
//                     }
//                     if (lp.height != LayoutParams.WRAP_CONTENT) {
//                         heightMode = MeasureSpec.EXACTLY;
//                         if (lp.height != LayoutParams.FILL_PARENT) {
//                             heightSize = lp.height;
//                         }
//                     }
//                     final int widthSpec = MeasureSpec.makeMeasureSpec(widthSize, widthMode);
//                     final int heightSpec = MeasureSpec.makeMeasureSpec(heightSize, heightMode);
//                     child.measure(widthSpec, heightSpec);

//                     if (consumeVertical) {
//                         childHeightSize -= child.getMeasuredHeight();
//                     } else if (consumeHorizontal) {
//                         childWidthSize -= child.getMeasuredWidth();
//                     }
//                 }
//             }
//         }

        // var ms = _this.GetMeasureSpec();
        // var ee = ms.GetEXACTLY();
        // elog("====ee:"+ee);
        // Assert(0);

//         mChildWidthMeasureSpec = MeasureSpec.makeMeasureSpec(childWidthSize, MeasureSpec.EXACTLY);
//         mChildHeightMeasureSpec = MeasureSpec.makeMeasureSpec(childHeightSize, MeasureSpec.EXACTLY);

//         // Make sure we have created all fragments that we need to have shown.
//         mInLayout = true;
//         populate();
//         mInLayout = false;

//         // Page views next.
//         size = getChildCount();
//         for (int i = 0; i < size; ++i) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() != GONE) {
//                 if (DEBUG) Log.v(TAG, "Measuring #" + i + " " + child
//                         + ": " + mChildWidthMeasureSpec);

//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 if (lp == null || !lp.isDecor) {
//                     final int widthSpec = MeasureSpec.makeMeasureSpec(
//                             (int) (childWidthSize * lp.widthFactor), MeasureSpec.EXACTLY);
//                     child.measure(widthSpec, mChildHeightMeasureSpec);
//                 }
//             }
//         }
//     }
    //ViewPager将子View分为两种，
    //一种是Decor View 用于装饰ViewPager，它可能需要占用一些空间；
    //另一种是普通的子View，也就是我们横滑时显示的各个View。
    //onMeasure首先是对Decor View进行测量，然后再对普通的子View进行测量。
    OnMeasure(_this, widthMeasureSpec, heightMeasureSpec) {

        elog("====ViewPager::OnMeasure====begin====");

var www = widthMeasureSpec & 0x3FFFFFFF;
var hhh = heightMeasureSpec & 0x3FFFFFFF;
elog(`====ViewPager::OnMeasure====www:${www}===hhh:${hhh}==`);

        //根据布局文件，设置尺寸信息，默认大小为0
        _this._SetMeasuredDimension(_this._GetDefaultSize(0, widthMeasureSpec),
                _this._GetDefaultSize(0, heightMeasureSpec));

        var measuredWidth = _this.GetMeasuredWidth();
        var maxGutterSize = measuredWidth / 10;
        //设置mGutterSize的值，后面再讲mGutterSize
        this.mGutterSize = Math.min(maxGutterSize, this.mDefaultGutterSize);

        // ViewPager的显示区域只能显示对于一个View
        //childWidthSize和childHeightSize为一个View的可用宽高大小
        //即去除了ViewPager内边距后的宽高
        var childWidthSize = measuredWidth - _this.GetPaddingLeft() - _this.GetPaddingRight();
        var childHeightSize = _this.GetMeasuredHeight() - _this.GetPaddingTop() - _this.GetPaddingBottom();
elog(`====ViewPager::OnMeasure====childWidthSize:${childWidthSize}==childHeightSize:${childHeightSize}==`);

        //1.先对Decor View进行测量
        //下面这个循环是只针对Decor View的，即用于装饰ViewPager的View
elog("====ViewPager::OnMeasure====_this.name:"+_this.getClass().GetName());
        var size = _this.GetChildCount();
elog("====ViewPager::OnMeasure====size:"+size);
        for (var i = 0; i < size; ++i) {
elog("====ViewPager::OnMeasure====i:"+i);
            var child = _this.GetChildAt(i);
elog("====ViewPager::OnMeasure====child.GetVisibility():"+child.GetVisibility());
            if (child.GetVisibility() != IView__GONE) {
                var lp = child.GetLayoutParams();
elog("====ViewPager::OnMeasure====child.name:"+child.getClass().GetName());
//CObject.showMethods(lp);
elog("====ViewPager::OnMeasure====lp.name:"+lp.getClass().GetName());
elog("====ViewPager::OnMeasure====lp.isDecor:"+lp.isDecor);
                //1.1 如果该View是Decor View,即用于装饰ViewPager的View
                if (lp != null && lp.isDecor) {
                    elog("====ViewPager::OnMeasure====isDecor====");
                    //Assert(0);
                    //1.2 获取Decor View的在水平方向和竖直方向上的Gravity
                    var hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
                    var vgrav = lp.gravity & Gravity.VERTICAL_GRAVITY_MASK;
                    //1.3 默认Dedor View模式对应的宽高是wrap_content
                    var widthMode = MeasureSpec.AT_MOST;
                    var heightMode = MeasureSpec.AT_MOST;
                    //1.4 记录Decor View是在垂直方向上还是在水平方向上占用空间
                    var consumeVertical = vgrav == Gravity.TOP || vgrav == Gravity.BOTTOM;
                    var consumeHorizontal = hgrav == Gravity.LEFT || hgrav == Gravity.RIGHT;
                    //1.5 consumeHorizontal：如果是在垂直方向上占用空间，
                    if (consumeVertical) {
                        // 那么水平方向就是match_parent，即EXACTLY
                        //而垂直方向上具体占用多少空间，还得由Decor View决定
                        widthMode = MeasureSpec.GetEXACTLY();
                    } else if (consumeHorizontal) {
                        //垂直方向也是同理
                        heightMode = MeasureSpec.GetEXACTLY();
                    }
                    //1.6 宽高大小，初始化为ViewPager可视区域中子View可用空间
                    var widthSize = childWidthSize;
                    var heightSize = childHeightSize;
                    //1.7 如果宽度不是wrap_content
                    if (lp.width != LayoutParams__WRAP_CONTENT) {
                        //，那么width的测量模式就是EXACTLY
                        widthMode = MeasureSpec.GetEXACTLY();
                        //如果宽度既不是wrap_content又不是match_parent，
                        if (lp.width != LayoutParams__FILL_PARENT) {
                            //那么说明是用户在布局文件写的具体的尺寸，直接将widthSize设置为这个具体尺寸
                            widthSize = lp.width;
                        }
                    }
                    //1.8 高度, 同1.7
                    if (lp.height != LayoutParams__WRAP_CONTENT) {
                        heightMode = MeasureSpec.GetEXACTLY();
                        if (lp.height != LayoutParams__FILL_PARENT) {
                            heightSize = lp.height;
                        }
                    }
                    //1.9 合成Decor View的宽高specification（包含尺寸和模式的整数）
                    var widthSpec = MeasureSpec.MakeMeasureSpec(widthSize, widthMode);
                    var heightSpec = MeasureSpec.MakeMeasureSpec(heightSize, heightMode);
                    //1.10 对DecorView进行测量
                    child.Measure(widthSpec, heightSpec);
                    //1.11 如果Decor View占用了ViewPager的垂直方向的空间
                    if (consumeVertical) {
                        //需要将子View的竖直方向可用的空间减去DecorView的高度，
                        childHeightSize -= child.GetMeasuredHeight();
                    //同理，水平方向上也做同样的处理
                    } else if (consumeHorizontal) {
                        childWidthSize -= child.GetMeasuredWidth();
                    }
                }
            }
        }
        //2.子View默认宽高的specification（包含尺寸和模式的整数）
        //（PS：mChildWidthMeasureSpec并没有再次用到，个人感觉有点多余）
        this.mChildWidthMeasureSpec = MeasureSpec.MakeMeasureSpec(childWidthSize, MeasureSpec.GetEXACTLY());
        this.mChildHeightMeasureSpec = MeasureSpec.MakeMeasureSpec(childHeightSize, MeasureSpec.GetEXACTLY());

        //3.确保我们需要显示的fragment已经被我们创建好了
        this.mInLayout = true;
        elog("====ViewPager::OnMeasure====Populate====call====3===");
        //populate()比较复杂，后面再详细介绍
        this.Populate();
        this.mInLayout = false;

        // 4.再对子View进行测量
        size = _this.GetChildCount();
        for (var i = 0; i < size; ++i) {
            elog("====ViewPager::OnMeasure====Populate====child===="+i+"/"+size);
            var child = _this.GetChildAt(i);
            //4.1 visibility为GONE的无需测量
            if (child.GetVisibility() != IView__GONE) {
                elog("====ViewPager::OnMeasure====Populate====call====4===");
                elog("====ViewPager::OnMeasure====Measuring #" + i + " " + child
                        + ": " + this.mChildWidthMeasureSpec);
                //4.2 获取子View的LayoutParams
                var lp = child.GetLayoutParams();
                //4.3 只针对子View而不对Decor View测量
                if (lp == null || !lp.isDecor) {
                    elog("====ViewPager::OnMeasure====Populate====call====5===childWidthSize:"+childWidthSize+"====widthFactor:"+lp.widthFactor);

CObject.showMethods(lp);
CObject.showMethods(lp._obj);

                    //4.4 LayoutParams的widthFactor是取值为[0,1]的浮点数，
                    // 用于表示子View占ViewPager显示区域中子View可用宽度的比例，
                    // 即(childWidthSize * lp.widthFactor)表示当前子View的实际宽度
                    var widthSpec = MeasureSpec.MakeMeasureSpec(
                            childWidthSize * lp.widthFactor, MeasureSpec.GetEXACTLY());
                    //4.5 对当前子View进行测量
                    child.Measure(widthSpec, this.mChildHeightMeasureSpec);
                }
            }
        }

        elog("====ViewPager::OnMeasure====end====");
    }

//     @Override
//     protected void onSizeChanged(int w, int h, int oldw, int oldh) {
//         super.onSizeChanged(w, h, oldw, oldh);

//         // Make sure scroll position is set correctly.
//         if (w != oldw) {
//             recomputeScrollPosition(w, oldw, mPageMargin, mPageMargin);
//         }
//     }
    OnSizeChanged(_this, w, h, oldw, oldh) {
        elog("====ViewPager::OnSizeChanged====TODO====");

        _this._OnSizeChanged(w, h, oldw, oldh);
        if (w != oldw) {
            this.RecomputeScrollPosition(w, oldw, this.mPageMargin, this.mPageMargin);
        }
    }

//     private void recomputeScrollPosition(int width, int oldWidth, int margin, int oldMargin) {
//         if (oldWidth > 0 && !mItems.isEmpty()) {
//             final int widthWithMargin = width - getPaddingLeft() - getPaddingRight() + margin;
//             final int oldWidthWithMargin = oldWidth - getPaddingLeft() - getPaddingRight()
//                                            + oldMargin;
//             final int xpos = getScrollX();
//             final float pageOffset = (float) xpos / oldWidthWithMargin;
//             final int newOffsetPixels = (int) (pageOffset * widthWithMargin);

//             scrollTo(newOffsetPixels, getScrollY());
//             if (!mScroller.isFinished()) {
//                 // We now return to your regularly scheduled scroll, already in progress.
//                 final int newDuration = mScroller.getDuration() - mScroller.timePassed();
//                 ItemInfo targetInfo = infoForPosition(mCurItem);
//                 mScroller.startScroll(newOffsetPixels, 0,
//                         (int) (targetInfo.offset * width), 0, newDuration);
//             }
//         } else {
//             final ItemInfo ii = infoForPosition(mCurItem);
//             final float scrollOffset = ii != null ? Math.min(ii.offset, mLastOffset) : 0;
//             final int scrollPos = (int) (scrollOffset *
//                                          (width - getPaddingLeft() - getPaddingRight()));
//             if (scrollPos != getScrollX()) {
//                 completeScroll(false);
//                 scrollTo(scrollPos, getScrollY());
//             }
//         }
//     }
    RecomputeScrollPosition(width, oldWidth, margin, oldMargin) {
        elog("====ViewPager::RecomputeScrollPosition====begin====");

        var _this = this._obj;

        if (oldWidth > 0 && !this.mItems.IsEmpty()) {
            var widthWithMargin = width - this._obj.GetPaddingLeft() - _this.GetPaddingRight() + margin;
            var oldWidthWithMargin = oldWidth - this._obj.GetPaddingLeft() - _this.GetPaddingRight()
                                    + oldMargin;
            var xpos = _this.GetScrollX();
            var pageOffset = xpos / oldWidthWithMargin;
            var newOffsetPixels = pageOffset * widthWithMargin;

            _this.ScrollTo(newOffsetPixels, _this.GetScrollY());
            if (!this.mScroller.IsFinished()) {
                // We now return to your regularly scheduled scroll, already in progress.
                newDuration = this.mScroller.GetDuration() - this.mScroller.TimePassed();
                var targetInfo = this.InfoForPosition(this.mCurItem);
                this.mScroller.startScroll(newOffsetPixels, 0,
                        targetInfo.offset * width, 0, newDuration);
            }
        } else {
            var ii = this.InfoForPosition(this.mCurItem);
            var scrollOffset = ii != null ? Math.min(ii.offset, this.mLastOffset) : 0;
            var scrollPos = scrollOffset *
                                         (width - _this.GetPaddingLeft() - _this.GetPaddingRight());
            if (scrollPos != _this.GetScrollX()) {
                this.CompleteScroll(false);
                _this.ScrollTo(scrollPos, _this.GetScrollY());
            }
        }

        elog("====ViewPager::RecomputeScrollPosition====end====");
    }

//     protected void onLayout(boolean changed, int l, int t, int r, int b) {
//         final int count = getChildCount();
//         int width = r - l;
//         int height = b - t;
//         int paddingLeft = getPaddingLeft();
//         int paddingTop = getPaddingTop();
//         int paddingRight = getPaddingRight();
//         int paddingBottom = getPaddingBottom();
//         final int scrollX = getScrollX();

//         int decorCount = 0;

//         // First pass - decor views. We need to do this in two passes so that
//         // we have the proper offsets for non-decor views later.
//         for (int i = 0; i < count; i++) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() != GONE) {
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 int childLeft = 0;
//                 int childTop = 0;
//                 if (lp.isDecor) {
//                     final int hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
//                     final int vgrav = lp.gravity & Gravity.VERTICAL_GRAVITY_MASK;
//                     switch (hgrav) {
//                         default:
//                             childLeft = paddingLeft;
//                             break;
//                         case Gravity.LEFT:
//                             childLeft = paddingLeft;
//                             paddingLeft += child.getMeasuredWidth();
//                             break;
//                         case Gravity.CENTER_HORIZONTAL:
//                             childLeft = Math.max((width - child.getMeasuredWidth()) / 2,
//                                     paddingLeft);
//                             break;
//                         case Gravity.RIGHT:
//                             childLeft = width - paddingRight - child.getMeasuredWidth();
//                             paddingRight += child.getMeasuredWidth();
//                             break;
//                     }
//                     switch (vgrav) {
//                         default:
//                             childTop = paddingTop;
//                             break;
//                         case Gravity.TOP:
//                             childTop = paddingTop;
//                             paddingTop += child.getMeasuredHeight();
//                             break;
//                         case Gravity.CENTER_VERTICAL:
//                             childTop = Math.max((height - child.getMeasuredHeight()) / 2,
//                                     paddingTop);
//                             break;
//                         case Gravity.BOTTOM:
//                             childTop = height - paddingBottom - child.getMeasuredHeight();
//                             paddingBottom += child.getMeasuredHeight();
//                             break;
//                     }
//                     childLeft += scrollX;
//                     child.layout(childLeft, childTop,
//                             childLeft + child.getMeasuredWidth(),
//                             childTop + child.getMeasuredHeight());
//                     decorCount++;
//                 }
//             }
//         }

//         final int childWidth = width - paddingLeft - paddingRight;

//         // Page views. Do this once we have the right padding offsets from above.
//         for (int i = 0; i < count; i++) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() != GONE) {
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 ItemInfo ii;
//                 if (!lp.isDecor && (ii = infoForChild(child)) != null) {
//                     int loff = (int) (childWidth * ii.offset);
//                     int childLeft = paddingLeft + loff;
//                     int childTop = paddingTop;
//                     if (lp.needsMeasure) {
//                         // This was added during layout and needs measurement.
//                         // Do it now that we know what we're working with.
//                         lp.needsMeasure = false;
//                         final int widthSpec = MeasureSpec.makeMeasureSpec(
//                                 (int) (childWidth * lp.widthFactor),
//                                 MeasureSpec.EXACTLY);
//                         final int heightSpec = MeasureSpec.makeMeasureSpec(
//                                 (int) (height - paddingTop - paddingBottom),
//                                 MeasureSpec.EXACTLY);
//                         child.measure(widthSpec, heightSpec);
//                     }
//                     if (DEBUG) Log.v(TAG, "Positioning #" + i + " " + child + " f=" + ii.object
//                             + ":" + childLeft + "," + childTop + " " + child.getMeasuredWidth()
//                             + "x" + child.getMeasuredHeight());
//                     child.layout(childLeft, childTop,
//                             childLeft + child.getMeasuredWidth(),
//                             childTop + child.getMeasuredHeight());
//                 }
//             }
//         }

//         mTopPageBounds = paddingTop;
//         mBottomPageBounds = height - paddingBottom;
//         mDecorChildCount = decorCount;

//         if (mFirstLayout) {
//             scrollToItem(mCurItem, false, 0, false);
//         }
//         mFirstLayout = false;
//     }
    //ViewPager的子View是水平摆放的，
    //所以在onLayout中，大部分工作的就是计算childLeft，
    //即子View的左边位置，而顶部位置基本上是一样的，
    OnLayout(_this, changed, l, t, r, b) {
        elog("====ViewPager::OnLayout====begin====");

        elog(`====ViewPager::OnLayout====0.0====l:${l}==t:${t}==r:${r}==b:${b}==`);

        //1.简单局部变量
        var count = _this.GetChildCount();
        var width = r - l;
        var height = b - t;
        var paddingLeft = _this.GetPaddingLeft();
        var paddingTop = _this.GetPaddingTop();
        var paddingRight = _this.GetPaddingRight();
        var paddingBottom = _this.GetPaddingBottom();
        var scrollX = _this.GetScrollX();

        //2.Decor View 数量
        var decorCount = 0;

//         // First pass - decor views. We need to do this in two passes so that
//         // we have the proper offsets for non-decor views later.
        //3.首先对Decor View进行layout，再对普通子View进行layout，
        // 之所以先对Decor View布局，是为了让普通子View能有合适的偏移
        //下面循环主要是针对Decor View
        for (var i = 0; i < count; i++) {
            var child = _this.GetChildAt(i);
            //3.1 visibility不为GONE才layout
            if (child.GetVisibility() != IView__GONE) {
                var lp = child.GetLayoutParams();
                //3.2 左边和顶部的边距初始化为0
                var childLeft = 0;
                var childTop = 0;
                //3.3 只针对Decor View
                if (lp.isDecor) {
                    //3.4 获取水平或垂直方向上的Gravity
                    var hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
                    var vgrav = lp.gravity & Gravity.VERTICAL_GRAVITY_MASK;
                    //3.5 根据水平方向上的Gravity，确定childLeft以及paddingLeft/Right
                    switch (hgrav) {
                        default:
                            //没有设置水平方向Gravity时（左中右），childLeft就取paddingLeft
                            childLeft = paddingLeft;
                            break;
                        case Gravity.LEFT:
                            //水平方向Gravity为left，Decor View往最左边靠
                            childLeft = paddingLeft;
                            paddingLeft += child.GetMeasuredWidth();
                            break;
                        case Gravity.CENTER_HORIZONTAL:
                            //将Decor View居中摆放
                            childLeft = Math.max((width - child.GetMeasuredWidth()) / 2,
                                    paddingLeft);
                            break;
                        case Gravity.RIGHT:
                            //将Decor View往最右边靠
                            childLeft = width - paddingRight - child.GetMeasuredWidth();
                            paddingRight += child.GetMeasuredWidth();
                            break;
                    }
                    //3.6 根据垂直方向上的Gravity，确定childTop以及paddingTop/Bottom
                    switch (vgrav) {
                        default:
                            childTop = paddingTop;
                            break;
                        case Gravity.TOP:
                            childTop = paddingTop;
                            paddingTop += child.GetMeasuredHeight();
                            break;
                        case Gravity.CENTER_VERTICAL:
                            childTop = Math.max((height - child.GetMeasuredHeight()) / 2,
                                    paddingTop);
                            break;
                        case Gravity.BOTTOM:
                            childTop = height - paddingBottom - child.GetMeasuredHeight();
                            paddingBottom += child.GetMeasuredHeight();
                            break;
                    }
                    //3.7 上面计算的childLeft是相对ViewPager的左边计算的，
                    //还需要加上x方向已经滑动的距离scrollX
                    childLeft += scrollX;
                    //3.8 对Decor View布局
                    child.Layout(childLeft, childTop,
                            childLeft + child.GetMeasuredWidth(),
                            childTop + child.GetMeasuredHeight());
                    //3.9 将Decor View数量+1
                    decorCount++;
                }
            }
        }

        //4.普通子View的宽度
        var childWidth = width - paddingLeft - paddingRight;

        elog("====ViewPager::OnLayout====1====");

        // Page views. Do this once we have the right padding offsets from above.
        //5.下面针对普通子View布局，在此之前我们已经得到正确的偏移量了
        for (var i = 0; i < count; i++) {
            elog(`====ViewPager::OnLayout====1====${i}/${count}`);

            var child = _this.GetChildAt(i);
            if (child.GetVisibility() != IView__GONE) {
                elog("====ViewPager::OnLayout====2====");

                var lp = child.GetLayoutParams();
                //5.1 ItemInfo 是ViewPager静态内部类，
                // 它保存了普通子View的position、offset等信息，是对普通子View的一个抽象描述
                var ii;
                //5.2 infoForChild通过传入View查询对应的ItemInfo对象
                if (!lp.isDecor && (ii = this.InfoForChild(child)) != null) {
                    elog("====ViewPager::OnLayout====3====");
                    //计算当前子View的左边偏移量
                    var loff = childWidth * ii.offset;
                    //将左边距+左边偏移量得到最终子View左边位置
                    var childLeft = paddingLeft + loff;
                    var childTop = paddingTop;
                    //5.3 如果当前子View需要进行测量（measure），当这个子View是在Layout期间新添加新的，
                    // 那么这个子View需要进行测量，即needsMeasure为true
                    if (lp.needsMeasure) {
                        elog("====ViewPager::OnLayout====4====");
                        // This was added during layout and needs measurement.
                        // Do it now that we know what we're working with.
                        //5.4 标记已经测量过了
                        lp.needsMeasure = false;
                        //5.5 下面过程跟onMeasure类似
                        var widthSpec = MeasureSpec.MakeMeasureSpec(
                                childWidth * lp.widthFactor,
                                MeasureSpec.GetEXACTLY());
                        var heightSpec = MeasureSpec.MakeMeasureSpec(
                                height - paddingTop - paddingBottom,
                                MeasureSpec.GetEXACTLY());
                        child.Measure(widthSpec, heightSpec);
                    }
                    if (ViewPager.DEBUG) elog(TAG + "Positioning #" + i + " " + child + " f=" + ii.object
                            + ":" + childLeft + "," + childTop + " " + child.GetMeasuredWidth()
                            + "x" + child.GetMeasuredHeight());
                    elog("====ViewPager::OnLayout====5====");
                    //5.6 对普通子View进行layout
                    child.Layout(childLeft, childTop,
                            childLeft + child.GetMeasuredWidth(),
                            childTop + child.GetMeasuredHeight());
                    elog("====ViewPager::OnLayout====6====");
                }
            }
        }

        elog("====ViewPager::OnLayout====7====");
        //6. 将部分局部变量保存到实例变量中
        this.mTopPageBounds = paddingTop;
        this.mBottomPageBounds = height - paddingBottom;
        this.mDecorChildCount = decorCount;

        elog("====ViewPager::OnLayout====8====");
        //7. 如果是第一次layout，则将ViewPager滑动到第一个子View的位置
        if (this.mFirstLayout) {
            elog("====ViewPager::OnLayout====9====");
            this.ScrollToItem(this.mCurItem, false, 0, false);
        }
        //8. 标记已经布局过了，即不再是第一次布局了
        this.mFirstLayout = false;

        elog("====ViewPager::OnLayout====end====");
    }

//     @Override
//     public void computeScroll() {
//         if (!mScroller.isFinished() && mScroller.computeScrollOffset()) {
//             int oldX = getScrollX();
//             int oldY = getScrollY();
//             int x = mScroller.getCurrX();
//             int y = mScroller.getCurrY();

//             if (oldX != x || oldY != y) {
//                 scrollTo(x, y);
//                 if (!pageScrolled(x)) {
//                     mScroller.abortAnimation();
//                     scrollTo(0, y);
//                 }
//             }

//             // Keep on drawing until the animation has finished.
//             ViewCompat.postInvalidateOnAnimation(this);
//             return;
//         }

//         // Done with scroll, clean up state.
//         completeScroll(true);
//     }
    ComputeScroll(_this) {
        elog("====ViewPager::ComputeScroll====begin====");

        if (!this.mScroller.IsFinished() && this.mScroller.ComputeScrollOffset()) {
            var oldX = _this.GetScrollX();
            var oldY = _this.GetScrollY();
            var x = this.mScroller.GetCurrX();
            var y = this.mScroller.GetCurrY();

            if (oldX != x || oldY != y) {
                _this.ScrollTo(x, y);
                if (!this.PageScrolled(x)) {
                    this.mScroller.AbortAnimation();
                    _this.ScrollTo(0, y);
                }
            }

            // Keep on drawing until the animation has finished.
            ViewCompat.postInvalidateOnAnimation(this);

            elog("====ViewPager::ComputeScroll====end====0====");

            return;
        }

        // Done with scroll, clean up state.
        this.CompleteScroll(true);

        elog("====ViewPager::ComputeScroll====end====1====");
    }

//     private boolean pageScrolled(int xpos) {
//         if (mItems.size() == 0) {
//             mCalledSuper = false;
//             onPageScrolled(0, 0, 0);
//             if (!mCalledSuper) {
//                 throw new IllegalStateException(
//                         "onPageScrolled did not call superclass implementation");
//             }
//             return false;
//         }
//         final ItemInfo ii = infoForCurrentScrollPosition();
//         final int width = getClientWidth();
//         final int widthWithMargin = width + mPageMargin;
//         final float marginOffset = (float) mPageMargin / width;
//         final int currentPage = ii.position;
//         final float pageOffset = (((float) xpos / width) - ii.offset) /
//                 (ii.widthFactor + marginOffset);
//         final int offsetPixels = (int) (pageOffset * widthWithMargin);

//         mCalledSuper = false;
//         onPageScrolled(currentPage, pageOffset, offsetPixels);
//         if (!mCalledSuper) {
//             throw new IllegalStateException(
//                     "onPageScrolled did not call superclass implementation");
//         }
//         return true;
//     }
    PageScrolled(xpos) {
        var _this = this._obj;

        if (this.mItems.GetSize() == 0) {
            this.mCalledSuper = false;
            this.OnPageScrolled(0, 0, 0);
            if (!this.mCalledSuper) {
                throw new IllegalStateException(
                        "onPageScrolled did not call superclass implementation");
            }
            return false;
        }
        var ii = this.InfoForCurrentScrollPosition();
        var width = this.GetClientWidth();
        var widthWithMargin = width + this.mPageMargin;
        var marginOffset = this.mPageMargin / width;
        var currentPage = ii.position;
        var pageOffset = ((xpos / width) - ii.offset) /
                (ii.widthFactor + marginOffset);
        var offsetPixels = pageOffset * widthWithMargin;

        this.mCalledSuper = false;
        this.OnPageScrolled(currentPage, pageOffset, offsetPixels);
        if (!this.mCalledSuper) {
            throw new IllegalStateException(
                    "onPageScrolled did not call superclass implementation");
        }
        return true;
    }

    /**
     * This method will be invoked when the current page is scrolled, either as part
     * of a programmatically initiated smooth scroll or a user initiated touch scroll.
     * If you override this method you must call through to the superclass implementation
     * (e.g. super.onPageScrolled(position, offset, offsetPixels)) before onPageScrolled
     * returns.
     *
     * @param position Position index of the first page currently being displayed.
     *                 Page position+1 will be visible if positionOffset is nonzero.
     * @param offset Value from [0, 1) indicating the offset from the page at position.
     * @param offsetPixels Value in pixels indicating the offset from position.
     */
//     protected void onPageScrolled(int position, float offset, int offsetPixels) {
//         // Offset any decor views if needed - keep them on-screen at all times.
//         if (mDecorChildCount > 0) {
//             final int scrollX = getScrollX();
//             int paddingLeft = getPaddingLeft();
//             int paddingRight = getPaddingRight();
//             final int width = getWidth();
//             final int childCount = getChildCount();
//             for (int i = 0; i < childCount; i++) {
//                 final View child = getChildAt(i);
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();
//                 if (!lp.isDecor) continue;

//                 final int hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
//                 int childLeft = 0;
//                 switch (hgrav) {
//                     default:
//                         childLeft = paddingLeft;
//                         break;
//                     case Gravity.LEFT:
//                         childLeft = paddingLeft;
//                         paddingLeft += child.getWidth();
//                         break;
//                     case Gravity.CENTER_HORIZONTAL:
//                         childLeft = Math.max((width - child.getMeasuredWidth()) / 2,
//                                 paddingLeft);
//                         break;
//                     case Gravity.RIGHT:
//                         childLeft = width - paddingRight - child.getMeasuredWidth();
//                         paddingRight += child.getMeasuredWidth();
//                         break;
//                 }
//                 childLeft += scrollX;

//                 final int childOffset = childLeft - child.getLeft();
//                 if (childOffset != 0) {
//                     child.offsetLeftAndRight(childOffset);
//                 }
//             }
//         }

//         if (mOnPageChangeListener != null) {
//             mOnPageChangeListener.onPageScrolled(position, offset, offsetPixels);
//         }
//         if (mInternalPageChangeListener != null) {
//             mInternalPageChangeListener.onPageScrolled(position, offset, offsetPixels);
//         }

//         if (mPageTransformer != null) {
//             final int scrollX = getScrollX();
//             final int childCount = getChildCount();
//             for (int i = 0; i < childCount; i++) {
//                 final View child = getChildAt(i);
//                 final LayoutParams lp = (LayoutParams) child.getLayoutParams();

//                 if (lp.isDecor) continue;

//                 final float transformPos = (float) (child.getLeft() - scrollX) / getClientWidth();
//                 mPageTransformer.transformPage(child, transformPos);
//             }
//         }

//         mCalledSuper = true;
//     }
    OnPageScrolled(position, offset, offsetPixels) {
        // Offset any decor views if needed - keep them on-screen at all times.

        elog("====ViewPager::OnPageScrolled====begin====");

        var _this = this._obj;

        if (this.mDecorChildCount > 0) {
            var scrollX = _this.GetScrollX();
            var paddingLeft = _this.GetPaddingLeft();
            var paddingRight = _this.GetPaddingRight();
            var width = _this.GetWidth();
            var childCount = _this.GetChildCount();
            for (var i = 0; i < childCount; i++) {
                var child = _this.GetChildAt(i);
                var lp = child.GetLayoutParams();
                if (!lp.isDecor) continue;

                var hgrav = lp.gravity & Gravity.HORIZONTAL_GRAVITY_MASK;
                var childLeft = 0;
                switch (hgrav) {
                    default:
                        childLeft = paddingLeft;
                        break;
                    case Gravity.LEFT:
                        childLeft = paddingLeft;
                        paddingLeft += child.GetWidth();
                        break;
                    case Gravity.CENTER_HORIZONTAL:
                        childLeft = Math.max((width - child.getMeasuredWidth()) / 2,
                                paddingLeft);
                        break;
                    case Gravity.RIGHT:
                        childLeft = width - paddingRight - child.GetMeasuredWidth();
                        paddingRight += child.GetMeasuredWidth();
                        break;
                }
                childLeft += scrollX;

                var childOffset = childLeft - child.GetLeft();
                if (childOffset != 0) {
                    child.OffsetLeftAndRight(childOffset);
                }
            }
        }

        if (this.mOnPageChangeListener != null) {
            this.mOnPageChangeListener.OnPageScrolled(position, offset, offsetPixels);
        }
        if (this.mInternalPageChangeListener != null) {
            this.mInternalPageChangeListener.OnPageScrolled(position, offset, offsetPixels);
        }

        if (this.mPageTransformer != null) {
            var scrollX = _this.GetScrollX();
            var childCount = _this.GetChildCount();
            for (var i = 0; i < childCount; i++) {
                var child = _this.GetChildAt(i);
                var lp = child.GetLayoutParams();

                if (lp.isDecor) continue;

                var transformPos = (child.GetLeft() - scrollX) / this.GetClientWidth();
                this.mPageTransformer.TransformPage(child, transformPos);
            }
        }

        this.mCalledSuper = true;

        elog("====ViewPager::OnPageScrolled====end====");
    }

//     private void completeScroll(boolean postEvents) {
//         boolean needPopulate = mScrollState == SCROLL_STATE_SETTLING;
//         if (needPopulate) {
//             // Done with scroll, no longer want to cache view drawing.
//             setScrollingCacheEnabled(false);
//             mScroller.abortAnimation();
//             int oldX = getScrollX();
//             int oldY = getScrollY();
//             int x = mScroller.getCurrX();
//             int y = mScroller.getCurrY();
//             if (oldX != x || oldY != y) {
//                 scrollTo(x, y);
//             }
//         }
//         mPopulatePending = false;
//         for (int i=0; i<mItems.size(); i++) {
//             ItemInfo ii = mItems.get(i);
//             if (ii.scrolling) {
//                 needPopulate = true;
//                 ii.scrolling = false;
//             }
//         }
//         if (needPopulate) {
//             if (postEvents) {
//                 ViewCompat.postOnAnimation(this, mEndScrollRunnable);
//             } else {
//                 mEndScrollRunnable.run();
//             }
//         }
//     }
    CompleteScroll(postEvents) {
        elog("====ViewPager::CompleteScroll====begin====");

        var _this = this._obj;

        var needPopulate = ViewPager.mScrollState == ViewPager.SCROLL_STATE_SETTLING;
        if (needPopulate) {
            // Done with scroll, no longer want to cache view drawing.
            this.SetScrollingCacheEnabled(false);

//elog("====ViewPager::CompleteScroll====0====");
//CObject.showMethods(this.mScroller);

            this.mScroller.AbortAnimation();
            var oldX = _this.GetScrollX();
            var oldY = _this.GetScrollY();
            var x = this.mScroller.GetCurrX();
            var y = this.mScroller.GetCurrY();
            if (oldX != x || oldY != y) {
                _this.ScrollTo(x, y);
            }
        }
        this.mPopulatePending = false;
        for (var i=0; i<this.mItems.GetSize(); i++) {
            var ii = this.mItems.Get(i);
            if (ii.scrolling) {
                needPopulate = true;
                ii.scrolling = false;
            }
        }
        if (needPopulate) {
            if (postEvents) {
                ViewCompat.PostOnAnimation(this, this.mEndScrollRunnable);
            } else {
                this.mEndScrollRunnable.Run();
            }
        }

        elog("====ViewPager::CompleteScroll====end====");
    }

//     private boolean isGutterDrag(float x, float dx) {
//         return (x < mGutterSize && dx > 0) || (x > getWidth() - mGutterSize && dx < 0);
//     }
    IsGutterDrag(x, dx) {
        return (x < mGutterSize && dx > 0) || (x > getWidth() - mGutterSize && dx < 0);
    }

//     private void enableLayers(boolean enable) {
//         final int childCount = getChildCount();
//         for (int i = 0; i < childCount; i++) {
//             final int layerType = enable ?
//                     ViewCompat.LAYER_TYPE_HARDWARE : ViewCompat.LAYER_TYPE_NONE;
//             ViewCompat.setLayerType(getChildAt(i), layerType, null);
//         }
//     }
    EnableLayers(enable) {
        var childCount = getChildCount();
        for (var i = 0; i < childCount; i++) {
            var layerType = enable ?
                    ViewCompat.LAYER_TYPE_HARDWARE : ViewCompat.LAYER_TYPE_NONE;
            ViewCompat.setLayerType(getChildAt(i), layerType, null);
        }
    }

//     @Override
//     public boolean onInterceptTouchEvent(MotionEvent ev) {
//         /*
//          * This method JUST determines whether we want to intercept the motion.
//          * If we return true, onMotionEvent will be called and we do the actual
//          * scrolling there.
//          */

//         final int action = ev.getAction() & MotionEventCompat.ACTION_MASK;

//         // Always take care of the touch gesture being complete.
//         if (action == MotionEvent.ACTION_CANCEL || action == MotionEvent.ACTION_UP) {
//             // Release the drag.
//             if (DEBUG) Log.v(TAG, "Intercept done!");
//             mIsBeingDragged = false;
//             mIsUnableToDrag = false;
//             mActivePointerId = INVALID_POINTER;
//             if (mVelocityTracker != null) {
//                 mVelocityTracker.recycle();
//                 mVelocityTracker = null;
//             }
//             return false;
//         }

//         // Nothing more to do here if we have decided whether or not we
//         // are dragging.
//         if (action != MotionEvent.ACTION_DOWN) {
//             if (mIsBeingDragged) {
//                 if (DEBUG) Log.v(TAG, "Intercept returning true!");
//                 return true;
//             }
//             if (mIsUnableToDrag) {
//                 if (DEBUG) Log.v(TAG, "Intercept returning false!");
//                 return false;
//             }
//         }

//         switch (action) {
//             case MotionEvent.ACTION_MOVE: {
//                 /*
//                  * mIsBeingDragged == false, otherwise the shortcut would have caught it. Check
//                  * whether the user has moved far enough from his original down touch.
//                  */

//                 /*
//                 * Locally do absolute value. mLastMotionY is set to the y value
//                 * of the down event.
//                 */
//                 final int activePointerId = mActivePointerId;
//                 if (activePointerId == INVALID_POINTER) {
//                     // If we don't have a valid id, the touch down wasn't on content.
//                     break;
//                 }

//                 final int pointerIndex = MotionEventCompat.findPointerIndex(ev, activePointerId);
//                 final float x = MotionEventCompat.getX(ev, pointerIndex);
//                 final float dx = x - mLastMotionX;
//                 final float xDiff = Math.abs(dx);
//                 final float y = MotionEventCompat.getY(ev, pointerIndex);
//                 final float yDiff = Math.abs(y - mInitialMotionY);
//                 if (DEBUG) Log.v(TAG, "Moved x to " + x + "," + y + " diff=" + xDiff + "," + yDiff);

//                 if (dx != 0 && !isGutterDrag(mLastMotionX, dx) &&
//                         canScroll(this, false, (int) dx, (int) x, (int) y)) {
//                     // Nested view has scrollable area under this point. Let it be handled there.
//                     mLastMotionX = x;
//                     mLastMotionY = y;
//                     mIsUnableToDrag = true;
//                     return false;
//                 }
//                 if (xDiff > mTouchSlop && xDiff * 0.5f > yDiff) {
//                     if (DEBUG) Log.v(TAG, "Starting drag!");
//                     mIsBeingDragged = true;
//                     requestParentDisallowInterceptTouchEvent(true);
//                     setScrollState(SCROLL_STATE_DRAGGING);
//                     mLastMotionX = dx > 0 ? mInitialMotionX + mTouchSlop :
//                             mInitialMotionX - mTouchSlop;
//                     mLastMotionY = y;
//                     setScrollingCacheEnabled(true);
//                 } else if (yDiff > mTouchSlop) {
//                     // The finger has moved enough in the vertical
//                     // direction to be counted as a drag...  abort
//                     // any attempt to drag horizontally, to work correctly
//                     // with children that have scrolling containers.
//                     if (DEBUG) Log.v(TAG, "Starting unable to drag!");
//                     mIsUnableToDrag = true;
//                 }
//                 if (mIsBeingDragged) {
//                     // Scroll to follow the motion event
//                     if (performDrag(x)) {
//                         ViewCompat.postInvalidateOnAnimation(this);
//                     }
//                 }
//                 break;
//             }

//             case MotionEvent.ACTION_DOWN: {
//                 /*
//                  * Remember location of down touch.
//                  * ACTION_DOWN always refers to pointer index 0.
//                  */
//                 mLastMotionX = mInitialMotionX = ev.getX();
//                 mLastMotionY = mInitialMotionY = ev.getY();
//                 mActivePointerId = MotionEventCompat.getPointerId(ev, 0);
//                 mIsUnableToDrag = false;

//                 mScroller.computeScrollOffset();
//                 if (mScrollState == SCROLL_STATE_SETTLING &&
//                         Math.abs(mScroller.getFinalX() - mScroller.getCurrX()) > mCloseEnough) {
//                     // Let the user 'catch' the pager as it animates.
//                     mScroller.abortAnimation();
//                     mPopulatePending = false;
//                     populate();
//                     mIsBeingDragged = true;
//                     requestParentDisallowInterceptTouchEvent(true);
//                     setScrollState(SCROLL_STATE_DRAGGING);
//                 } else {
//                     completeScroll(false);
//                     mIsBeingDragged = false;
//                 }

//                 if (DEBUG) Log.v(TAG, "Down at " + mLastMotionX + "," + mLastMotionY
//                         + " mIsBeingDragged=" + mIsBeingDragged
//                         + "mIsUnableToDrag=" + mIsUnableToDrag);
//                 break;
//             }

//             case MotionEventCompat.ACTION_POINTER_UP:
//                 onSecondaryPointerUp(ev);
//                 break;
//         }

//         if (mVelocityTracker == null) {
//             mVelocityTracker = VelocityTracker.obtain();
//         }
//         mVelocityTracker.addMovement(ev);

//         /*
//          * The only time we want to intercept motion events is if we are in the
//          * drag mode.
//          */
//         return mIsBeingDragged;
//     }
    OnInterceptTouchEvent(_this, ev, result) {
        /*
         * This method JUST determines whether we want to intercept the motion.
         * If we return true, onMotionEvent will be called and we do the actual
         * scrolling there.
         */

        elog("====ViewPager::OnInterceptTouchEvent====begin====");

        CObject.showMethods(ev);
        elog("====ViewPager::OnInterceptTouchEvent====0====");
        elog("====ViewPager::OnInterceptTouchEvent====1====ev name:"+ev.getClass().GetName());

        var action = ev.GetAction() & MotionEventCompat.ACTION_MASK;

        // Always take care of the touch gesture being complete.
        if (action == MotionEvent.ACTION_CANCEL || action == MotionEvent.ACTION_UP) {
            // Release the drag.
            if (DEBUG) Log.v(TAG, "Intercept done!");
            mIsBeingDragged = false;
            mIsUnableToDrag = false;
            mActivePointerId = INVALID_POINTER;
            if (mVelocityTracker != null) {
                mVelocityTracker.recycle();
                mVelocityTracker = null;
            }
            return false;
        }

        // Nothing more to do here if we have decided whether or not we
        // are dragging.
        if (action != MotionEvent.ACTION_DOWN) {
            if (mIsBeingDragged) {
                if (DEBUG) Log.v(TAG, "Intercept returning true!");
                return true;
            }
            if (mIsUnableToDrag) {
                if (DEBUG) Log.v(TAG, "Intercept returning false!");
                return false;
            }
        }

        switch (action) {
            case MotionEvent.ACTION_MOVE: {
                /*
                 * mIsBeingDragged == false, otherwise the shortcut would have caught it. Check
                 * whether the user has moved far enough from his original down touch.
                 */

                /*
                * Locally do absolute value. mLastMotionY is set to the y value
                * of the down event.
                */
                var activePointerId = mActivePointerId;
                if (activePointerId == INVALID_POINTER) {
                    // If we don't have a valid id, the touch down wasn't on content.
                    break;
                }

                var pointerIndex = MotionEventCompat.findPointerIndex(ev, activePointerId);
                var x = MotionEventCompat.getX(ev, pointerIndex);
                var dx = x - mLastMotionX;
                var xDiff = Math.abs(dx);
                var y = MotionEventCompat.getY(ev, pointerIndex);
                var yDiff = Math.abs(y - mInitialMotionY);
                if (DEBUG) Log.v(TAG, "Moved x to " + x + "," + y + " diff=" + xDiff + "," + yDiff);

                if (dx != 0 && !isGutterDrag(mLastMotionX, dx) &&
                        canScroll(this, false, dx, x, y)) {
                    // Nested view has scrollable area under this point. Let it be handled there.
                    mLastMotionX = x;
                    mLastMotionY = y;
                    mIsUnableToDrag = true;
                    return false;
                }
                if (xDiff > mTouchSlop && xDiff * 0.5 > yDiff) {
                    if (DEBUG) Log.v(TAG, "Starting drag!");
                    mIsBeingDragged = true;
                    requestParentDisallowInterceptTouchEvent(true);
                    setScrollState(SCROLL_STATE_DRAGGING);
                    mLastMotionX = dx > 0 ? mInitialMotionX + mTouchSlop :
                            mInitialMotionX - mTouchSlop;
                    mLastMotionY = y;
                    setScrollingCacheEnabled(true);
                } else if (yDiff > mTouchSlop) {
                    // The finger has moved enough in the vertical
                    // direction to be counted as a drag...  abort
                    // any attempt to drag horizontally, to work correctly
                    // with children that have scrolling containers.
                    if (DEBUG) Log.v(TAG, "Starting unable to drag!");
                    mIsUnableToDrag = true;
                }
                if (mIsBeingDragged) {
                    // Scroll to follow the motion event
                    if (performDrag(x)) {
                        ViewCompat.postInvalidateOnAnimation(this);
                    }
                }
                break;
            }

            case MotionEvent.ACTION_DOWN: {
                /*
                 * Remember location of down touch.
                 * ACTION_DOWN always refers to pointer index 0.
                 */
                mLastMotionX = mInitialMotionX = ev.getX();
                mLastMotionY = mInitialMotionY = ev.getY();
                mActivePointerId = MotionEventCompat.getPointerId(ev, 0);
                mIsUnableToDrag = false;

                mScroller.computeScrollOffset();
                if (mScrollState == SCROLL_STATE_SETTLING &&
                        Math.abs(mScroller.getFinalX() - mScroller.getCurrX()) > mCloseEnough) {
                    // Let the user 'catch' the pager as it animates.
                    mScroller.abortAnimation();
                    mPopulatePending = false;
                    populate();
                    mIsBeingDragged = true;
                    requestParentDisallowInterceptTouchEvent(true);
                    setScrollState(SCROLL_STATE_DRAGGING);
                } else {
                    completeScroll(false);
                    mIsBeingDragged = false;
                }

                if (DEBUG) Log.v(TAG, "Down at " + mLastMotionX + "," + mLastMotionY
                        + " mIsBeingDragged=" + mIsBeingDragged
                        + "mIsUnableToDrag=" + mIsUnableToDrag);
                break;
            }

            case MotionEventCompat.ACTION_POINTER_UP:
                onSecondaryPointerUp(ev);
                break;
        }

        if (mVelocityTracker == null) {
            mVelocityTracker = VelocityTracker.obtain();
        }
        mVelocityTracker.addMovement(ev);

        /*
         * The only time we want to intercept motion events is if we are in the
         * drag mode.
         */
        result.data = mIsBeingDragged;
    }

//     @Override
//     public boolean onTouchEvent(MotionEvent ev) {
//         if (mFakeDragging) {
//             // A fake drag is in progress already, ignore this real one
//             // but still eat the touch events.
//             // (It is likely that the user is multi-touching the screen.)
//             return true;
//         }

//         if (ev.getAction() == MotionEvent.ACTION_DOWN && ev.getEdgeFlags() != 0) {
//             // Don't handle edge touches immediately -- they may actually belong to one of our
//             // descendants.
//             return false;
//         }

//         if (mAdapter == null || mAdapter.getCount() == 0) {
//             // Nothing to present or scroll; nothing to touch.
//             return false;
//         }

//         if (mVelocityTracker == null) {
//             mVelocityTracker = VelocityTracker.obtain();
//         }
//         mVelocityTracker.addMovement(ev);

//         final int action = ev.getAction();
//         boolean needsInvalidate = false;

//         switch (action & MotionEventCompat.ACTION_MASK) {
//             case MotionEvent.ACTION_DOWN: {
//                 mScroller.abortAnimation();
//                 mPopulatePending = false;
//                 populate();

//                 // Remember where the motion event started
//                 mLastMotionX = mInitialMotionX = ev.getX();
//                 mLastMotionY = mInitialMotionY = ev.getY();
//                 mActivePointerId = MotionEventCompat.getPointerId(ev, 0);
//                 break;
//             }
//             case MotionEvent.ACTION_MOVE:
//                 if (!mIsBeingDragged) {
//                     final int pointerIndex = MotionEventCompat.findPointerIndex(ev, mActivePointerId);
//                     final float x = MotionEventCompat.getX(ev, pointerIndex);
//                     final float xDiff = Math.abs(x - mLastMotionX);
//                     final float y = MotionEventCompat.getY(ev, pointerIndex);
//                     final float yDiff = Math.abs(y - mLastMotionY);
//                     if (DEBUG) Log.v(TAG, "Moved x to " + x + "," + y + " diff=" + xDiff + "," + yDiff);
//                     if (xDiff > mTouchSlop && xDiff > yDiff) {
//                         if (DEBUG) Log.v(TAG, "Starting drag!");
//                         mIsBeingDragged = true;
//                         requestParentDisallowInterceptTouchEvent(true);
//                         mLastMotionX = x - mInitialMotionX > 0 ? mInitialMotionX + mTouchSlop :
//                                 mInitialMotionX - mTouchSlop;
//                         mLastMotionY = y;
//                         setScrollState(SCROLL_STATE_DRAGGING);
//                         setScrollingCacheEnabled(true);

//                         // Disallow Parent Intercept, just in case
//                         ViewParent parent = getParent();
//                         if (parent != null) {
//                             parent.requestDisallowInterceptTouchEvent(true);
//                         }
//                     }
//                 }
//                 // Not else! Note that mIsBeingDragged can be set above.
//                 if (mIsBeingDragged) {
//                     // Scroll to follow the motion event
//                     final int activePointerIndex = MotionEventCompat.findPointerIndex(
//                             ev, mActivePointerId);
//                     final float x = MotionEventCompat.getX(ev, activePointerIndex);
//                     needsInvalidate |= performDrag(x);
//                 }
//                 break;
//             case MotionEvent.ACTION_UP:
//                 if (mIsBeingDragged) {
//                     final VelocityTracker velocityTracker = mVelocityTracker;
//                     velocityTracker.computeCurrentVelocity(1000, mMaximumVelocity);
//                     int initialVelocity = (int) VelocityTrackerCompat.getXVelocity(
//                             velocityTracker, mActivePointerId);
//                     mPopulatePending = true;
//                     final int width = getClientWidth();
//                     final int scrollX = getScrollX();
//                     final ItemInfo ii = infoForCurrentScrollPosition();
//                     final int currentPage = ii.position;
//                     final float pageOffset = (((float) scrollX / width) - ii.offset) / ii.widthFactor;
//                     final int activePointerIndex =
//                             MotionEventCompat.findPointerIndex(ev, mActivePointerId);
//                     final float x = MotionEventCompat.getX(ev, activePointerIndex);
//                     final int totalDelta = (int) (x - mInitialMotionX);
//                     int nextPage = determineTargetPage(currentPage, pageOffset, initialVelocity,
//                             totalDelta);
//                     setCurrentItemInternal(nextPage, true, true, initialVelocity);

//                     mActivePointerId = INVALID_POINTER;
//                     endDrag();
//                     needsInvalidate = mLeftEdge.onRelease() | mRightEdge.onRelease();
//                 }
//                 break;
//             case MotionEvent.ACTION_CANCEL:
//                 if (mIsBeingDragged) {
//                     scrollToItem(mCurItem, true, 0, false);
//                     mActivePointerId = INVALID_POINTER;
//                     endDrag();
//                     needsInvalidate = mLeftEdge.onRelease() | mRightEdge.onRelease();
//                 }
//                 break;
//             case MotionEventCompat.ACTION_POINTER_DOWN: {
//                 final int index = MotionEventCompat.getActionIndex(ev);
//                 final float x = MotionEventCompat.getX(ev, index);
//                 mLastMotionX = x;
//                 mActivePointerId = MotionEventCompat.getPointerId(ev, index);
//                 break;
//             }
//             case MotionEventCompat.ACTION_POINTER_UP:
//                 onSecondaryPointerUp(ev);
//                 mLastMotionX = MotionEventCompat.getX(ev,
//                         MotionEventCompat.findPointerIndex(ev, mActivePointerId));
//                 break;
//         }
//         if (needsInvalidate) {
//             ViewCompat.postInvalidateOnAnimation(this);
//         }
//         return true;
//     }
    OnTouchEvent(_this, ev, result) {
        if (mFakeDragging) {
            // A fake drag is in progress already, ignore this real one
            // but still eat the touch events.
            // (It is likely that the user is multi-touching the screen.)
            result.data = true;
            return;
        }

        if (ev.getAction() == MotionEvent.ACTION_DOWN && ev.getEdgeFlags() != 0) {
            // Don't handle edge touches immediately -- they may actually belong to one of our
            // descendants.
            result.data = false;
            return;
        }

        if (mAdapter == null || mAdapter.getCount() == 0) {
            // Nothing to present or scroll; nothing to touch.
            result.data = false;
            return;
        }

        if (mVelocityTracker == null) {
            mVelocityTracker = VelocityTracker.obtain();
        }
        mVelocityTracker.addMovement(ev);

        var action = ev.getAction();
        var needsInvalidate = false;

        switch (action & MotionEventCompat.ACTION_MASK) {
            case MotionEvent.ACTION_DOWN: {
                mScroller.abortAnimation();
                mPopulatePending = false;
                populate();

                // Remember where the motion event started
                mLastMotionX = mInitialMotionX = ev.getX();
                mLastMotionY = mInitialMotionY = ev.getY();
                mActivePointerId = MotionEventCompat.getPointerId(ev, 0);
                break;
            }
            case MotionEvent.ACTION_MOVE:
                if (!mIsBeingDragged) {
                    var pointerIndex = MotionEventCompat.findPointerIndex(ev, mActivePointerId);
                    var x = MotionEventCompat.getX(ev, pointerIndex);
                    var xDiff = Math.abs(x - mLastMotionX);
                    var y = MotionEventCompat.getY(ev, pointerIndex);
                    var yDiff = Math.abs(y - mLastMotionY);
                    if (DEBUG) Log.v(TAG, "Moved x to " + x + "," + y + " diff=" + xDiff + "," + yDiff);
                    if (xDiff > mTouchSlop && xDiff > yDiff) {
                        if (DEBUG) Log.v(TAG, "Starting drag!");
                        mIsBeingDragged = true;
                        requestParentDisallowInterceptTouchEvent(true);
                        mLastMotionX = x - mInitialMotionX > 0 ? mInitialMotionX + mTouchSlop :
                                mInitialMotionX - mTouchSlop;
                        mLastMotionY = y;
                        setScrollState(SCROLL_STATE_DRAGGING);
                        setScrollingCacheEnabled(true);

                        // Disallow Parent Intercept, just in case
                        var parent = getParent();
                        if (parent != null) {
                            parent.requestDisallowInterceptTouchEvent(true);
                        }
                    }
                }
                // Not else! Note that mIsBeingDragged can be set above.
                if (mIsBeingDragged) {
                    // Scroll to follow the motion event
                    var activePointerIndex = MotionEventCompat.findPointerIndex(
                            ev, mActivePointerId);
                    var x = MotionEventCompat.getX(ev, activePointerIndex);
                    needsInvalidate |= performDrag(x);
                }
                break;
            case MotionEvent.ACTION_UP:
                if (mIsBeingDragged) {
                    var velocityTracker = mVelocityTracker;
                    velocityTracker.computeCurrentVelocity(1000, mMaximumVelocity);
                    var initialVelocity = VelocityTrackerCompat.getXVelocity(
                            velocityTracker, mActivePointerId);
                    mPopulatePending = true;
                    var width = getClientWidth();
                    var scrollX = getScrollX();
                    var ii = infoForCurrentScrollPosition();
                    var currentPage = ii.position;
                    var pageOffset = ((scrollX / width) - ii.offset) / ii.widthFactor;
                    var activePointerIndex =
                            MotionEventCompat.findPointerIndex(ev, mActivePointerId);
                    var x = MotionEventCompat.getX(ev, activePointerIndex);
                    var totalDelta = x - mInitialMotionX;
                    var nextPage = determineTargetPage(currentPage, pageOffset, initialVelocity,
                            totalDelta);
                    setCurrentItemInternal(nextPage, true, true, initialVelocity);

                    mActivePointerId = INVALID_POINTER;
                    endDrag();
                    needsInvalidate = mLeftEdge.onRelease() | mRightEdge.onRelease();
                }
                break;
            case MotionEvent.ACTION_CANCEL:
                if (mIsBeingDragged) {
                    scrollToItem(mCurItem, true, 0, false);
                    mActivePointerId = INVALID_POINTER;
                    endDrag();
                    needsInvalidate = mLeftEdge.onRelease() | mRightEdge.onRelease();
                }
                break;
            case MotionEventCompat.ACTION_POINTER_DOWN: {
                var index = MotionEventCompat.getActionIndex(ev);
                var x = MotionEventCompat.getX(ev, index);
                mLastMotionX = x;
                mActivePointerId = MotionEventCompat.getPointerId(ev, index);
                break;
            }
            case MotionEventCompat.ACTION_POINTER_UP:
                onSecondaryPointerUp(ev);
                mLastMotionX = MotionEventCompat.getX(ev,
                        MotionEventCompat.findPointerIndex(ev, mActivePointerId));
                break;
        }
        if (needsInvalidate) {
            ViewCompat.postInvalidateOnAnimation(this);
        }
        result.data = true;
    }

//     private void requestParentDisallowInterceptTouchEvent(boolean disallowIntercept) {
//         final ViewParent parent = getParent();
//         if (parent != null) {
//             parent.requestDisallowInterceptTouchEvent(disallowIntercept);
//         }
//     }
    RequestParentDisallowInterceptTouchEvent(disallowIntercept) {
        var parent = getParent();
        if (parent != null) {
            parent.requestDisallowInterceptTouchEvent(disallowIntercept);
        }
    }

//     private boolean performDrag(float x) {
//         boolean needsInvalidate = false;

//         final float deltaX = mLastMotionX - x;
//         mLastMotionX = x;

//         float oldScrollX = getScrollX();
//         float scrollX = oldScrollX + deltaX;
//         final int width = getClientWidth();

//         float leftBound = width * mFirstOffset;
//         float rightBound = width * mLastOffset;
//         boolean leftAbsolute = true;
//         boolean rightAbsolute = true;

//         final ItemInfo firstItem = mItems.get(0);
//         final ItemInfo lastItem = mItems.get(mItems.size() - 1);
//         if (firstItem.position != 0) {
//             leftAbsolute = false;
//             leftBound = firstItem.offset * width;
//         }
//         if (lastItem.position != mAdapter.getCount() - 1) {
//             rightAbsolute = false;
//             rightBound = lastItem.offset * width;
//         }

//         if (scrollX < leftBound) {
//             if (leftAbsolute) {
//                 float over = leftBound - scrollX;
//                 needsInvalidate = mLeftEdge.onPull(Math.abs(over) / width);
//             }
//             scrollX = leftBound;
//         } else if (scrollX > rightBound) {
//             if (rightAbsolute) {
//                 float over = scrollX - rightBound;
//                 needsInvalidate = mRightEdge.onPull(Math.abs(over) / width);
//             }
//             scrollX = rightBound;
//         }
//         // Don't lose the rounded component
//         mLastMotionX += scrollX - (int) scrollX;
//         scrollTo((int) scrollX, getScrollY());
//         pageScrolled((int) scrollX);

//         return needsInvalidate;
//    }
    PerformDrag(x) {
        var needsInvalidate = false;

        var deltaX = mLastMotionX - x;
        mLastMotionX = x;

        var oldScrollX = getScrollX();
        var scrollX = oldScrollX + deltaX;
        var width = getClientWidth();

        var leftBound = width * mFirstOffset;
        var rightBound = width * mLastOffset;
        var leftAbsolute = true;
        var rightAbsolute = true;

        var firstItem = mItems.get(0);
        var lastItem = mItems.get(mItems.size() - 1);
        if (firstItem.position != 0) {
            leftAbsolute = false;
            leftBound = firstItem.offset * width;
        }
        if (lastItem.position != mAdapter.getCount() - 1) {
            rightAbsolute = false;
            rightBound = lastItem.offset * width;
        }

        if (scrollX < leftBound) {
            if (leftAbsolute) {
                var over = leftBound - scrollX;
                needsInvalidate = mLeftEdge.onPull(Math.abs(over) / width);
            }
            scrollX = leftBound;
        } else if (scrollX > rightBound) {
            if (rightAbsolute) {
                var over = scrollX - rightBound;
                needsInvalidate = mRightEdge.onPull(Math.abs(over) / width);
            }
            scrollX = rightBound;
        }
        // Don't lose the rounded component
        mLastMotionX += scrollX - scrollX;
        scrollTo(scrollX, getScrollY());
        pageScrolled(scrollX);

        return needsInvalidate;
   }

    /**
     * @return Info about the page at the current scroll position.
     *         This can be synthetic for a missing middle page; the 'object' field can be null.
     */
//     private ItemInfo infoForCurrentScrollPosition() {
//         final int width = getClientWidth();
//         final float scrollOffset = width > 0 ? (float) getScrollX() / width : 0;
//         final float marginOffset = width > 0 ? (float) mPageMargin / width : 0;
//         int lastPos = -1;
//         float lastOffset = 0.f;
//         float lastWidth = 0.f;
//         boolean first = true;

//         ItemInfo lastItem = null;
//         for (int i = 0; i < mItems.size(); i++) {
//             ItemInfo ii = mItems.get(i);
//             float offset;
//             if (!first && ii.position != lastPos + 1) {
//                 // Create a synthetic item for a missing page.
//                 ii = mTempItem;
//                 ii.offset = lastOffset + lastWidth + marginOffset;
//                 ii.position = lastPos + 1;
//                 ii.widthFactor = mAdapter.getPageWidth(ii.position);
//                 i--;
//             }
//             offset = ii.offset;

//             final float leftBound = offset;
//             final float rightBound = offset + ii.widthFactor + marginOffset;
//             if (first || scrollOffset >= leftBound) {
//                 if (scrollOffset < rightBound || i == mItems.size() - 1) {
//                     return ii;
//                 }
//             } else {
//                 return lastItem;
//             }
//             first = false;
//             lastPos = ii.position;
//             lastOffset = offset;
//             lastWidth = ii.widthFactor;
//             lastItem = ii;
//         }

//         return lastItem;
//     }
    InfoForCurrentScrollPosition() {
        var _this = this._obj;

        var width = this.GetClientWidth();
        var scrollOffset = width > 0 ? _this.GetScrollX() / width : 0;
        var marginOffset = width > 0 ? this.mPageMargin / width : 0;
        var lastPos = -1;
        var lastOffset = 0.0;
        var lastWidth = 0.0;
        var first = true;

        var lastItem = null;
        for (var i = 0; i < this.mItems.GetSize(); i++) {
            var ii = this.mItems.Get(i);
            var offset;
            if (!first && ii.position != lastPos + 1) {
                // Create a synthetic item for a missing page.
                ii = this.mTempItem;
                ii.offset = lastOffset + lastWidth + marginOffset;
                ii.position = lastPos + 1;
                ii.widthFactor = this.mAdapter.GetPageWidth(ii.position);
                i--;
            }
            offset = ii.offset;

            var leftBound = offset;
            var rightBound = offset + ii.widthFactor + marginOffset;
            if (first || scrollOffset >= leftBound) {
                if (scrollOffset < rightBound || i == this.mItems.GetSize() - 1) {
                    return ii;
                }
            } else {
                return lastItem;
            }
            first = false;
            lastPos = ii.position;
            lastOffset = offset;
            lastWidth = ii.widthFactor;
            lastItem = ii;
        }

        return lastItem;
    }

//     private int determineTargetPage(int currentPage, float pageOffset, int velocity, int deltaX) {
//         int targetPage;
//         if (Math.abs(deltaX) > mFlingDistance && Math.abs(velocity) > mMinimumVelocity) {
//             targetPage = velocity > 0 ? currentPage : currentPage + 1;
//         } else {
//             final float truncator = currentPage >= mCurItem ? 0.4f : 0.6f;
//             targetPage = (int) (currentPage + pageOffset + truncator);
//         }

//         if (mItems.size() > 0) {
//             final ItemInfo firstItem = mItems.get(0);
//             final ItemInfo lastItem = mItems.get(mItems.size() - 1);

//             // Only let the user target pages we have items for
//             targetPage = Math.max(firstItem.position, Math.min(targetPage, lastItem.position));
//         }

//         return targetPage;
//     }
    DetermineTargetPage(currentPage, pageOffset, velocity, deltaX) {
        var targetPage;
        if (Math.abs(deltaX) > mFlingDistance && Math.abs(velocity) > mMinimumVelocity) {
            targetPage = velocity > 0 ? currentPage : currentPage + 1;
        } else {
            var truncator = currentPage >= this.mCurItem ? 0.4 : 0.6;
            targetPage = currentPage + pageOffset + truncator;
        }

        if (this.mItems.GetSize() > 0) {
            var firstItem = mItems.get(0);
            var lastItem = mItems.get(mItems.size() - 1);

            // Only let the user target pages we have items for
            targetPage = Math.max(firstItem.position, Math.min(targetPage, lastItem.position));
        }

        return targetPage;
    }

//     @Override
//     public void draw(Canvas canvas) {
        //CObject.showMethods(canvas);
        //CObject.showMethods(_this);

//         super.draw(canvas);
//         boolean needsInvalidate = false;

//         final int overScrollMode = ViewCompat.getOverScrollMode(this);
//         if (overScrollMode == ViewCompat.OVER_SCROLL_ALWAYS ||
//                 (overScrollMode == ViewCompat.OVER_SCROLL_IF_CONTENT_SCROLLS &&
//                         mAdapter != null && mAdapter.getCount() > 1)) {
//             if (!mLeftEdge.isFinished()) {
//                 final int restoreCount = canvas.save();
//                 final int height = getHeight() - getPaddingTop() - getPaddingBottom();
//                 final int width = getWidth();

//                 canvas.rotate(270);
//                 canvas.translate(-height + getPaddingTop(), mFirstOffset * width);
//                 mLeftEdge.setSize(height, width);
//                 needsInvalidate |= mLeftEdge.draw(canvas);
//                 canvas.restoreToCount(restoreCount);
//             }
//             if (!mRightEdge.isFinished()) {
//                 final int restoreCount = canvas.save();
//                 final int width = getWidth();
//                 final int height = getHeight() - getPaddingTop() - getPaddingBottom();

//                 canvas.rotate(90);
//                 canvas.translate(-getPaddingTop(), -(mLastOffset + 1) * width);
//                 mRightEdge.setSize(height, width);
//                 needsInvalidate |= mRightEdge.draw(canvas);
//                 canvas.restoreToCount(restoreCount);
//             }
//         } else {
//             mLeftEdge.finish();
//             mRightEdge.finish();
//         }

//         if (needsInvalidate) {
//             // Keep animating
//             ViewCompat.postInvalidateOnAnimation(this);
//         }
//     }
    Draw(_this, canvas) {
        elog("====ViewPager::Draw====begin====");

        _this._Draw(canvas);
        var needsInvalidate = false;

        elog("====ViewPager::Draw====0====");
        var overScrollMode = ViewCompat.GetOverScrollMode(_this);
        elog("====ViewPager::Draw====1====");
        elog("====ViewPager::Draw====1.1====overScrollMode:"+overScrollMode);
        elog("====ViewPager::Draw====1.2====OVER_SCROLL_ALWAYS:"+ViewCompat.OVER_SCROLL_ALWAYS);
        elog("====ViewPager::Draw====1.3====OVER_SCROLL_IF_CONTENT_SCROLLS:"+ViewCompat.OVER_SCROLL_IF_CONTENT_SCROLLS);
        elog("====ViewPager::Draw====1.4====mAdapter:"+(this.mAdapter != null));
        elog("====ViewPager::Draw====1.5====count:"+this.mAdapter.GetCount());
        if (overScrollMode == ViewCompat.OVER_SCROLL_ALWAYS ||
                (overScrollMode == ViewCompat.OVER_SCROLL_IF_CONTENT_SCROLLS &&
                        this.mAdapter != null && this.mAdapter.GetCount() > 1)) {
            elog("====ViewPager::Draw====2====");
            if (!this.mLeftEdge.IsFinished()) {
                elog("====ViewPager::Draw====3====");
                var restoreCount = canvas.Save();
                var height = this.GetHeight() - this.GetPaddingTop() - this.GetPaddingBottom();
                var width = this.GetWidth();

                canvas.Rotate(270);
                canvas.Translate(-height + this.GetPaddingTop(), this.mFirstOffset * width);
                this.mLeftEdge.SetSize(height, width);
                needsInvalidate |= this.mLeftEdge.Draw(canvas);
                canvas.RestoreToCount(restoreCount);
                elog("====ViewPager::Draw====4====");
            }
            if (!this.mRightEdge.IsFinished()) {
                elog("====ViewPager::Draw====5====");
                var restoreCount = canvas.Save();
                var width = this.GetWidth();
                var height = this.GetHeight() - this.GetPaddingTop() - this.GetPaddingBottom();

                canvas.Rotate(90);
                canvas.Translate(-this.GetPaddingTop(), -(this.mLastOffset + 1) * width);
                this.mRightEdge.SetSize(height, width);
                needsInvalidate |= this.mRightEdge.Draw(canvas);
                canvas.RestoreToCount(restoreCount);
                elog("====ViewPager::Draw====6====");
            }
        } else {
            elog("====ViewPager::Draw====7====");
            this.mLeftEdge.Finish();
            elog("====ViewPager::Draw====8====");
            this.mRightEdge.Finish();
            elog("====ViewPager::Draw====9====");
        }

        if (needsInvalidate) {
            // Keep animating
            elog("====ViewPager::Draw====10====");
            ViewCompat.PostInvalidateOnAnimation(_this);
            elog("====ViewPager::Draw====11====");
        }

        elog("====ViewPager::Draw====end====");

elog("====ViewPager::Draw====end==0==");

var p = _this;  //CCalculatorPadViewPager->[CLinearLayout, CCalculatorPadLayout]

//p = p.GetParent();  //CLinearLayout->[CRelativeLayout, CCalculatorPadViewPager]
//p = p.GetParent();  //CFrameLayout->[CLinearLayout]
//p = p.GetParent();  //CLinearLayout->[CViewStub, CFrameLayout]
//p = p.GetParent();  //IViewParent->CLinearLayout

//p = p.GetChildAt(0);    //CLinearLayout->[CCalculatorPadLayout, CCalculatorPadLayout]
//p = p.GetChildAt(0);

//p.SetVisibility(IView__VISIBLE);

var name = p.getClass().GetName();
elog(`====name:${name}====`);

var cnt = p.GetChildCount();

//CObject.showMethods(p,"Parent");

p.SetBackgroundColor(1234567);

var a=[];
for(var i=0;i<cnt;i++){
    var o = p.GetChildAt(i);
    var n = o.getClass().GetName();
    var w = o.GetWidth();
    var h = o.GetHeight();
    var l = o.GetLeft();
    var t = o.GetTop();
    var v = o.GetVisibility();
    a.push(`[${n}]l:${l},t:${t},w:${w},h:${h},v:${v}`);
}
elog("====ViewPager::Draw====end==1==");

elog(`====cnt:${cnt}====name:${name}====`);

elog("===="+a.join("==")+"====");

elog("====ViewPager::Draw====end==2==");

    }

//----------------test begin-------------------

// ECode ViewGroup::DispatchDraw(
//     /* [in] */ ICanvas* canvas)
// {
//     Boolean usingRenderNodeProperties;
//     canvas->IsRecordingFor(mRenderNode, &usingRenderNodeProperties);
//     Int32 childrenCount = mChildrenCount;
//     AutoPtr<ArrayOf<IView*> > children = mChildren;
//     Int32 flags = mGroupFlags;

//     Boolean bval;
//     if ((flags & FLAG_RUN_ANIMATION) != 0 && CanAnimate()) {
//         Boolean cache = (mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE;
//         IsHardwareAccelerated(&bval);
//         Boolean buildCache = !bval;

//         for (Int32 i = 0; i < childrenCount; i++) {
//             View* child = (View*)(*children)[i];
//             if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
//                 AutoPtr<IViewGroupLayoutParams> params;
//                 child->GetLayoutParams((IViewGroupLayoutParams**)&params);
//                 AttachLayoutAnimationParameters((*children)[i], params, i, childrenCount);
//                 BindLayoutAnimation((*children)[i]);
//                 if (cache) {
//                     child->SetDrawingCacheEnabled(TRUE);
//                     if (buildCache) {
//                         child->BuildDrawingCache(TRUE);
//                     }
//                 }
//             }
//         }

//         mLayoutAnimationController->WillOverlap(&bval);
//         if (bval) {
//             mGroupFlags |= FLAG_OPTIMIZE_INVALIDATE;
//         }

//         mLayoutAnimationController->Start();

//         mGroupFlags &= ~FLAG_RUN_ANIMATION;
//         mGroupFlags &= ~FLAG_ANIMATION_DONE;

//         if (cache) {
//             mGroupFlags |= FLAG_CHILDREN_DRAWN_WITH_CACHE;
//         }

//         if (mAnimationListener != NULL) {
//             AutoPtr<IAnimation> animation;
//             mLayoutAnimationController->GetAnimation((IAnimation**)&animation);
//             mAnimationListener->OnAnimationStart(animation);
//         }
//     }

//     Int32 clipSaveCount = 0;
//     const Boolean clipToPadding = (flags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK;
//     if (clipToPadding) {
//         canvas->Save(&clipSaveCount);
//         canvas->ClipRect(mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
//                 mScrollX + mRight - mLeft - mPaddingRight,
//                 mScrollY + mBottom - mTop - mPaddingBottom, &bval);
//     }

//     // We will draw our child's animation, let's reset the flag
//     mPrivateFlags &= ~PFLAG_DRAW_ANIMATION;
//     mGroupFlags &= ~FLAG_INVALIDATE_REQUIRED;

//     Boolean more = FALSE;
//     Int64 drawingTime = 0;
//     GetDrawingTime(&drawingTime);

//     if (usingRenderNodeProperties) canvas->InsertReorderBarrier();
//     // Only use the preordered list if not HW accelerated, since the HW pipeline will do the
//     // draw reordering internally
//     AutoPtr<IList> preorderedList = usingRenderNodeProperties ? NULL : BuildOrderedChildList();
//     Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();
//     for (Int32 i = 0; i < childrenCount; i++) {
//         Int32 childIndex = customOrder ? GetChildDrawingOrder(childrenCount, i) : i;
//         AutoPtr<IView> child;
//         if (preorderedList == NULL) {
//             child = (*children)[childIndex];
//         }
//         else {
//             AutoPtr<IInterface> temp;
//             preorderedList->Get(childIndex, (IInterface**)&temp);
//             child = IView::Probe(temp);
//         }
//         AutoPtr<IAnimation> animation;
//         if ((((View*)child.Get())->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
//             || (child->GetAnimation((IAnimation**)&animation), animation) != NULL) {
//             more |= DrawChild(canvas, child, drawingTime);
//         }
//     }
//     if (preorderedList != NULL) preorderedList->Clear();

//     // Draw any disappearing views that have animations
//     if (mDisappearingChildren != NULL) {
//         Int32 disappearingCount;
//         mDisappearingChildren->GetSize(&disappearingCount);
//         // Go backwards -- we may delete as animations finish
//         for (Int32 i = disappearingCount - 1; i >= 0; i--) {
//             AutoPtr<IInterface> temp;
//             mDisappearingChildren->Get(i, (IInterface**)&temp);
//             AutoPtr<IView> child = IView::Probe(temp);
//             more |= DrawChild(canvas, child, drawingTime);
//         }
//     }
//     if (usingRenderNodeProperties) canvas->InsertInorderBarrier();

//     if (DebugDraw()) {
//         OnDebugDraw(canvas);
//     }

//     if (clipToPadding) {
//         canvas->RestoreToCount(clipSaveCount);
//     }

//     // mGroupFlags might have been updated by drawChild()
//     flags = mGroupFlags;

//     if ((flags & FLAG_INVALIDATE_REQUIRED) == FLAG_INVALIDATE_REQUIRED) {
//         Invalidate(TRUE);
//     }

//     Boolean isDone = FALSE;
//     if (mLayoutAnimationController) {
//         mLayoutAnimationController->IsDone(&isDone);
//     }
//     if ((flags & FLAG_ANIMATION_DONE) == 0 &&
//         (flags & FLAG_NOTIFY_ANIMATION_LISTENER) == 0 && isDone && !more) {
//         // We want to erase the drawing cache and notify the listener after the
//         // next frame is drawn because one extra invalidate() is caused by
//         // drawChild() after the animation is over
//         mGroupFlags |= FLAG_NOTIFY_ANIMATION_LISTENER;
//         AutoPtr<DispatchDrawRunnable> end = new DispatchDrawRunnable(this);
//         Post(end, &bval);
//     }

//     return NOERROR;
// }

// Boolean ViewGroup::DrawChild(
//     /* [in] */ ICanvas* canvas,
//     /* [in] */ IView* child,
//     /* [in] */ Int64 drawingTime)
// {
//     return ((View*)child)->Draw(canvas, this, drawingTime);
// }
DrawChild(_this, canvas, child, drawingTime, result)
{
    elog("====ViewPager::DrawChild====begin====");
    var name = child.getClass().GetName();
    elog(`====ViewPager::_DrawChild====name:${name}====`);

    //var _this = this._obj;

    //return child.Draw(canvas, _this, drawingTime);
    var bRet = child.Draw(canvas, _this, drawingTime);
    //child.Draw(canvas);

    //var bRet = _this._DrawChild(canvas, child, drawingTime);
    //var bRet = true;
    if(result){
        result.data = bRet;
        elog("====ViewPager::DrawChild====end====0====");
        return;
    }
    else {
        elog("====ViewPager::DrawChild====end====1====");
        return bRet;
    }
}

_DispatchDraw(_this, canvas)
{
    elog("====ViewPager::_DispatchDraw====begin====");

    // Boolean usingRenderNodeProperties;
    // canvas->IsRecordingFor(mRenderNode, &usingRenderNodeProperties);
    // Int32 childrenCount = mChildrenCount;
    // AutoPtr<ArrayOf<IView*> > children = mChildren;
    // Int32 flags = mGroupFlags;
    var usingRenderNodeProperties;
    //usingRenderNodeProperties = canvas.IsRecordingFor(mRenderNode);
    usingRenderNodeProperties = false;
    var childrenCount = _this.GetChildCount();
    var children = [];
    for(var i=0;i<childrenCount;i++)children.push(_this.GetChildAt(i));
    var flags = _this._GetGroupFlags();

    // Boolean bval;
    // if ((flags & FLAG_RUN_ANIMATION) != 0 && CanAnimate()) {
    var bval;
    var FLAG_RUN_ANIMATION = 0x8;
    if ((flags & FLAG_RUN_ANIMATION) != 0 && _this.CanAnimate()) {
    //     Boolean cache = (mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE;
    //     IsHardwareAccelerated(&bval);
    //     Boolean buildCache = !bval;
        var cache = (mGroupFlags & FLAG_ANIMATION_CACHE) == FLAG_ANIMATION_CACHE;
        bval = IsHardwareAccelerated();
        var buildCache = !bval;

    //     for (Int32 i = 0; i < childrenCount; i++) {
    //         View* child = (View*)(*children)[i];
    //         if ((child->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE) {
    //             AutoPtr<IViewGroupLayoutParams> params;
    //             child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    //             AttachLayoutAnimationParameters((*children)[i], params, i, childrenCount);
    //             BindLayoutAnimation((*children)[i]);
    //             if (cache) {
    //                 child->SetDrawingCacheEnabled(TRUE);
    //                 if (buildCache) {
    //                     child->BuildDrawingCache(TRUE);
    //                 }
    //             }
    //         }
    //     }
        for (var i = 0; i < childrenCount; i++) {
            var child = children[i];
            if ((child.mViewFlags & IView__VISIBILITY_MASK) == IView__VISIBLE) {
                var params;
                params = child.GetLayoutParams();   //IViewGroupLayoutParams
                AttachLayoutAnimationParameters(children[i], params, i, childrenCount);
                BindLayoutAnimation(children[i]);
                if (cache) {
                    child.SetDrawingCacheEnabled(true);
                    if (buildCache) {
                        child.BuildDrawingCache(true);
                    }
                }
            }
        }

    //     mLayoutAnimationController->WillOverlap(&bval);
    //     if (bval) {
    //         mGroupFlags |= FLAG_OPTIMIZE_INVALIDATE;
    //     }
        bval = mLayoutAnimationController.WillOverlap();
        if (bval) {
            mGroupFlags |= FLAG_OPTIMIZE_INVALIDATE;
        }

    //     mLayoutAnimationController->Start();

    //     mGroupFlags &= ~FLAG_RUN_ANIMATION;
    //     mGroupFlags &= ~FLAG_ANIMATION_DONE;

    //     if (cache) {
    //         mGroupFlags |= FLAG_CHILDREN_DRAWN_WITH_CACHE;
    //     }

    //     if (mAnimationListener != NULL) {
    //         AutoPtr<IAnimation> animation;
    //         mLayoutAnimationController->GetAnimation((IAnimation**)&animation);
    //         mAnimationListener->OnAnimationStart(animation);
    //     }
        mLayoutAnimationController.Start();

        mGroupFlags &= ~FLAG_RUN_ANIMATION;
        mGroupFlags &= ~FLAG_ANIMATION_DONE;

        if (cache) {
            mGroupFlags |= FLAG_CHILDREN_DRAWN_WITH_CACHE;
        }

        if (mAnimationListener != NULL) {
            var animation;
            animation = mLayoutAnimationController.GetAnimation();
            mAnimationListener.OnAnimationStart(animation);
        }
    }

    // Int32 clipSaveCount = 0;
    // const Boolean clipToPadding = (flags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK;
    // if (clipToPadding) {
    //     canvas->Save(&clipSaveCount);
    //     canvas->ClipRect(mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
    //             mScrollX + mRight - mLeft - mPaddingRight,
    //             mScrollY + mBottom - mTop - mPaddingBottom, &bval);
    // }

    var FLAG_CLIP_TO_PADDING = 0x2;
    var FLAG_RUN_ANIMATION = 0x8;
    var FLAG_PADDING_NOT_NULL = 0x20;
    var CLIP_TO_PADDING_MASK = FLAG_CLIP_TO_PADDING | FLAG_PADDING_NOT_NULL;

    var clipSaveCount = 0;
    var clipToPadding = (flags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK;
    if (clipToPadding) {
        clipSaveCount = canvas.Save();
        bval = canvas.ClipRect(mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
                mScrollX + mRight - mLeft - mPaddingRight,
                mScrollY + mBottom - mTop - mPaddingBottom);
    }

    // // We will draw our child's animation, let's reset the flag
    // mPrivateFlags &= ~PFLAG_DRAW_ANIMATION;
    // mGroupFlags &= ~FLAG_INVALIDATE_REQUIRED;

    // Boolean more = FALSE;
    // Int64 drawingTime = 0;
    // GetDrawingTime(&drawingTime);

    // We will draw our child's animation, let's reset the flag
//    mPrivateFlags &= ~PFLAG_DRAW_ANIMATION;
//    mGroupFlags &= ~FLAG_INVALIDATE_REQUIRED;

    var more = false;
    var drawingTime = 0;
    drawingTime = _this.GetDrawingTime();

    // if (usingRenderNodeProperties) canvas->InsertReorderBarrier();
    // // Only use the preordered list if not HW accelerated, since the HW pipeline will do the
    // // draw reordering internally
    // AutoPtr<IList> preorderedList = usingRenderNodeProperties ? NULL : BuildOrderedChildList();
    // Boolean customOrder = preorderedList == NULL && IsChildrenDrawingOrderEnabled();
    // for (Int32 i = 0; i < childrenCount; i++) {
    //     Int32 childIndex = customOrder ? GetChildDrawingOrder(childrenCount, i) : i;
    //     AutoPtr<IView> child;
    //     if (preorderedList == NULL) {
    //         child = (*children)[childIndex];
    //     }
    //     else {
    //         AutoPtr<IInterface> temp;
    //         preorderedList->Get(childIndex, (IInterface**)&temp);
    //         child = IView::Probe(temp);
    //     }
    //     AutoPtr<IAnimation> animation;
    //     if ((((View*)child.Get())->mViewFlags & VISIBILITY_MASK) == IView::VISIBLE
    //         || (child->GetAnimation((IAnimation**)&animation), animation) != NULL) {
    //         more |= DrawChild(canvas, child, drawingTime);
    //     }
    // }
    // if (preorderedList != NULL) preorderedList->Clear();

    if (usingRenderNodeProperties) canvas.InsertReorderBarrier();
    // Only use the preordered list if not HW accelerated, since the HW pipeline will do the
    // draw reordering internally
    //var preorderedList = usingRenderNodeProperties ? null : _this.BuildOrderedChildList();
    var preorderedList = null;
    //var customOrder = preorderedList == null && IsChildrenDrawingOrderEnabled();
    var customOrder = false;
    for (var i = 0; i < childrenCount; i++) {
        //var childIndex = customOrder ? GetChildDrawingOrder(childrenCount, i) : i;
        var childIndex = i;
        var child;
        if (preorderedList == null) {
            child = children[childIndex];
        }
        else {
            var temp;
            temp = preorderedList.Get(childIndex);
            child = IView.Probe(temp);
        }
        var animation;
var name=child.getClass().GetName();
elog(`====ViewPager::_DispatchDraw====name:${name}====`);
        //if ((child._GetViewFlags() & IView__VISIBILITY_MASK) == IView__VISIBLE
        //    || (animation = child.GetAnimation()) != NULL) {
            //more |= _this._DrawChild(canvas, child, drawingTime);
            more |= this.DrawChild(_this, canvas, child, drawingTime);
        //}
    }
    if (preorderedList != null) preorderedList.Clear();

    // // Draw any disappearing views that have animations
    // if (mDisappearingChildren != NULL) {
    //     Int32 disappearingCount;
    //     mDisappearingChildren->GetSize(&disappearingCount);
    //     // Go backwards -- we may delete as animations finish
    //     for (Int32 i = disappearingCount - 1; i >= 0; i--) {
    //         AutoPtr<IInterface> temp;
    //         mDisappearingChildren->Get(i, (IInterface**)&temp);
    //         AutoPtr<IView> child = IView::Probe(temp);
    //         more |= DrawChild(canvas, child, drawingTime);
    //     }
    // }
    // if (usingRenderNodeProperties) canvas->InsertInorderBarrier();

    // Draw any disappearing views that have animations
    //if (mDisappearingChildren != null) {
    if (false) {
        var disappearingCount;
        disappearingCount = mDisappearingChildren.GetSize();
        // Go backwards -- we may delete as animations finish
        for (var i = disappearingCount - 1; i >= 0; i--) {
            var temp;
            temp = mDisappearingChildren.Get(i);
            var child = IView.Probe(temp);
            more |= DrawChild(canvas, child, drawingTime);
        }
    }
    if (usingRenderNodeProperties) canvas.InsertInorderBarrier();

    // if (DebugDraw()) {
    //     OnDebugDraw(canvas);
    // }

    // if (clipToPadding) {
    //     canvas->RestoreToCount(clipSaveCount);
    // }

    // // mGroupFlags might have been updated by drawChild()
    // flags = mGroupFlags;

    // if ((flags & FLAG_INVALIDATE_REQUIRED) == FLAG_INVALIDATE_REQUIRED) {
    //     Invalidate(TRUE);
    // }

    //if (DebugDraw()) {
    if (false) {
        OnDebugDraw(canvas);
    }

    if (clipToPadding) {
        canvas.RestoreToCount(clipSaveCount);
    }

    // mGroupFlags might have been updated by drawChild()
    flags = _this._GetGroupFlags();

    if ((flags & ViewGroup__FLAG_INVALIDATE_REQUIRED) == ViewGroup__FLAG_INVALIDATE_REQUIRED) {
        _this.Invalidate(true);
    }

    // Boolean isDone = FALSE;
    // if (mLayoutAnimationController) {
    //     mLayoutAnimationController->IsDone(&isDone);
    // }
    // if ((flags & FLAG_ANIMATION_DONE) == 0 &&
    //     (flags & FLAG_NOTIFY_ANIMATION_LISTENER) == 0 && isDone && !more) {
    //     // We want to erase the drawing cache and notify the listener after the
    //     // next frame is drawn because one extra invalidate() is caused by
    //     // drawChild() after the animation is over
    //     mGroupFlags |= FLAG_NOTIFY_ANIMATION_LISTENER;
    //     AutoPtr<DispatchDrawRunnable> end = new DispatchDrawRunnable(this);
    //     Post(end, &bval);
    // }
    var isDone = false;
    //if (mLayoutAnimationController) {
    if (false) {
        isDone = mLayoutAnimationController.IsDone();
    }
    if ((flags & ViewGroup__FLAG_ANIMATION_DONE) == 0 &&
        (flags & ViewGroup__FLAG_NOTIFY_ANIMATION_LISTENER) == 0 && isDone && !more) {
        // We want to erase the drawing cache and notify the listener after the
        // next frame is drawn because one extra invalidate() is caused by
        // drawChild() after the animation is over
        mGroupFlags |= ViewGroup__FLAG_NOTIFY_ANIMATION_LISTENER;
        var end = new DispatchDrawRunnable(this);
        bval = Post(end);
    }

    elog("====ViewPager::_DispatchDraw====end====");

    // return NOERROR;
    return;
}

//----------------test end---------------------

//     @Override
    DispatchDraw(_this, canvas) {
        elog("====ViewPager::DispatchDraw====begin====");
        //_this._DispatchDraw(canvas);
        this._DispatchDraw(_this, canvas);
        elog("====ViewPager::DispatchDraw====end====");
    }

//     @Override
//     protected void onDraw(Canvas canvas) {
//         super.onDraw(canvas);

//         // Draw the margin drawable between pages if needed.
//         if (mPageMargin > 0 && mMarginDrawable != null && mItems.size() > 0 && mAdapter != null) {
//             final int scrollX = getScrollX();
//             final int width = getWidth();

//             final float marginOffset = (float) mPageMargin / width;
//             int itemIndex = 0;
//             ItemInfo ii = mItems.get(0);
//             float offset = ii.offset;
//             final int itemCount = mItems.size();
//             final int firstPos = ii.position;
//             final int lastPos = mItems.get(itemCount - 1).position;
//             for (int pos = firstPos; pos < lastPos; pos++) {
//                 while (pos > ii.position && itemIndex < itemCount) {
//                     ii = mItems.get(++itemIndex);
//                 }

//                 float drawAt;
//                 if (pos == ii.position) {
//                     drawAt = (ii.offset + ii.widthFactor) * width;
//                     offset = ii.offset + ii.widthFactor + marginOffset;
//                 } else {
//                     float widthFactor = mAdapter.getPageWidth(pos);
//                     drawAt = (offset + widthFactor) * width;
//                     offset += widthFactor + marginOffset;
//                 }

//                 if (drawAt + mPageMargin > scrollX) {
//                     mMarginDrawable.setBounds((int) drawAt, mTopPageBounds,
//                             (int) (drawAt + mPageMargin + 0.5f), mBottomPageBounds);
//                     mMarginDrawable.draw(canvas);
//                 }

//                 if (drawAt > scrollX + width) {
//                     break; // No more visible, no sense in continuing
//                 }
//             }
//         }
//     }
    OnDraw(_this, canvas) {
        elog("====ViewPager::OnDraw====begin====");

        _this._OnDraw(canvas);

        elog("====ViewPager::OnDraw====mPageMargin:"+this.mPageMargin);
        elog("====ViewPager::OnDraw====mMarginDrawable:"+this.mMarginDrawable);
        elog("====ViewPager::OnDraw====this.mItems.GetSize:"+this.mItems.GetSize());
        elog("====ViewPager::OnDraw====mAdapter:"+this.mAdapter);

        // Draw the margin drawable between pages if needed.
        if (this.mPageMargin > 0 && this.mMarginDrawable != null && this.mItems.GetSize() > 0 && this.mAdapter != null) {
            elog("====ViewPager::OnDraw====0====");

            var scrollX = getScrollX();
            var width = getWidth();

            var marginOffset = this.mPageMargin / width;
            var itemIndex = 0;
            var ii = this.mItems.Get(0);
            var offset = ii.offset;
            var itemCount = this.mItems.GetSize();
            var firstPos = ii.position;
            var lastPos = this.mItems.Get(itemCount - 1).position;
            elog("====ViewPager::OnDraw====1===="+pos+"/"+lastPos);
            for (var pos = firstPos; pos < lastPos; pos++) {
                while (pos > ii.position && itemIndex < itemCount) {
                    ii = this.mItems.Get(++itemIndex);
                }

                var drawAt;
                if (pos == ii.position) {
                    drawAt = (ii.offset + ii.widthFactor) * width;
                    offset = ii.offset + ii.widthFactor + marginOffset;
                } else {
                    var widthFactor = this.mAdapter.GetPageWidth(pos);
                    drawAt = (offset + widthFactor) * width;
                    offset += widthFactor + marginOffset;
                }

                if (drawAt + this.mPageMargin > scrollX) {
                    this.mMarginDrawable.setBounds(drawAt, mTopPageBounds,
                            drawAt + this.mPageMargin + 0.5, mBottomPageBounds);
                    this.mMarginDrawable.draw(canvas);
                }

                if (drawAt > scrollX + width) {
                    break; // No more visible, no sense in continuing
                }
            }
        }

        elog("====ViewPager::OnDraw====end====");
    }

    /**
     * Start a fake drag of the pager.
     *
     * <p>A fake drag can be useful if you want to synchronize the motion of the ViewPager
     * with the touch scrolling of another view, while still letting the ViewPager
     * control the snapping motion and fling behavior. (e.g. parallax-scrolling tabs.)
     * Call {@link #fakeDragBy(float)} to simulate the actual drag motion. Call
     * {@link #endFakeDrag()} to complete the fake drag and fling as necessary.
     *
     * <p>During a fake drag the ViewPager will ignore all touch events. If a real drag
     * is already in progress, this method will return false.
     *
     * @return true if the fake drag began successfully, false if it could not be started.
     *
     * @see #fakeDragBy(float)
     * @see #endFakeDrag()
     */
//     public boolean beginFakeDrag() {
//         if (mIsBeingDragged) {
//             return false;
//         }
//         mFakeDragging = true;
//         setScrollState(SCROLL_STATE_DRAGGING);
//         mInitialMotionX = mLastMotionX = 0;
//         if (mVelocityTracker == null) {
//             mVelocityTracker = VelocityTracker.obtain();
//         } else {
//             mVelocityTracker.clear();
//         }
//         final long time = SystemClock.uptimeMillis();
//         final MotionEvent ev = MotionEvent.obtain(time, time, MotionEvent.ACTION_DOWN, 0, 0, 0);
//         mVelocityTracker.addMovement(ev);
//         ev.recycle();
//         mFakeDragBeginTime = time;
//         return true;
//     }
    BeginFakeDrag() {
        if (mIsBeingDragged) {
            return false;
        }
        mFakeDragging = true;
        setScrollState(SCROLL_STATE_DRAGGING);
        mInitialMotionX = mLastMotionX = 0;
        if (mVelocityTracker == null) {
            mVelocityTracker = VelocityTracker.obtain();
        } else {
            mVelocityTracker.clear();
        }
        var time = SystemClock.uptimeMillis();
        var ev = MotionEvent.obtain(time, time, MotionEvent.ACTION_DOWN, 0, 0, 0);
        mVelocityTracker.addMovement(ev);
        ev.recycle();
        mFakeDragBeginTime = time;
        return true;
    }

    /**
     * End a fake drag of the pager.
     *
     * @see #beginFakeDrag()
     * @see #fakeDragBy(float)
     */
//     public void endFakeDrag() {
//         if (!mFakeDragging) {
//             throw new IllegalStateException("No fake drag in progress. Call beginFakeDrag first.");
//         }

//         final VelocityTracker velocityTracker = mVelocityTracker;
//         velocityTracker.computeCurrentVelocity(1000, mMaximumVelocity);
//         int initialVelocity = (int) VelocityTrackerCompat.getXVelocity(
//                 velocityTracker, mActivePointerId);
//         mPopulatePending = true;
//         final int width = getClientWidth();
//         final int scrollX = getScrollX();
//         final ItemInfo ii = infoForCurrentScrollPosition();
//         final int currentPage = ii.position;
//         final float pageOffset = (((float) scrollX / width) - ii.offset) / ii.widthFactor;
//         final int totalDelta = (int) (mLastMotionX - mInitialMotionX);
//         int nextPage = determineTargetPage(currentPage, pageOffset, initialVelocity,
//                 totalDelta);
//         setCurrentItemInternal(nextPage, true, true, initialVelocity);
//         endDrag();

//         mFakeDragging = false;
//     }
    EndFakeDrag() {
        if (!mFakeDragging) {
            throw new IllegalStateException("No fake drag in progress. Call beginFakeDrag first.");
        }

        var velocityTracker = mVelocityTracker;
        velocityTracker.computeCurrentVelocity(1000, mMaximumVelocity);
        var initialVelocity = VelocityTrackerCompat.getXVelocity(
                velocityTracker, mActivePointerId);
        mPopulatePending = true;
        var width = getClientWidth();
        var scrollX = getScrollX();
        var ii = infoForCurrentScrollPosition();
        var currentPage = ii.position;
        var pageOffset = ((scrollX / width) - ii.offset) / ii.widthFactor;
        var totalDelta = mLastMotionX - mInitialMotionX;
        var nextPage = determineTargetPage(currentPage, pageOffset, initialVelocity,
                totalDelta);
        setCurrentItemInternal(nextPage, true, true, initialVelocity);
        endDrag();

        mFakeDragging = false;
    }

    /**
     * Fake drag by an offset in pixels. You must have called {@link #beginFakeDrag()} first.
     *
     * @param xOffset Offset in pixels to drag by.
     * @see #beginFakeDrag()
     * @see #endFakeDrag()
     */
//     public void fakeDragBy(float xOffset) {
//         if (!mFakeDragging) {
//             throw new IllegalStateException("No fake drag in progress. Call beginFakeDrag first.");
//         }

//         mLastMotionX += xOffset;

//         float oldScrollX = getScrollX();
//         float scrollX = oldScrollX - xOffset;
//         final int width = getClientWidth();

//         float leftBound = width * mFirstOffset;
//         float rightBound = width * mLastOffset;

//         final ItemInfo firstItem = mItems.get(0);
//         final ItemInfo lastItem = mItems.get(mItems.size() - 1);
//         if (firstItem.position != 0) {
//             leftBound = firstItem.offset * width;
//         }
//         if (lastItem.position != mAdapter.getCount() - 1) {
//             rightBound = lastItem.offset * width;
//         }

//         if (scrollX < leftBound) {
//             scrollX = leftBound;
//         } else if (scrollX > rightBound) {
//             scrollX = rightBound;
//         }
//         // Don't lose the rounded component
//         mLastMotionX += scrollX - (int) scrollX;
//         scrollTo((int) scrollX, getScrollY());
//         pageScrolled((int) scrollX);

//         // Synthesize an event for the VelocityTracker.
//         final long time = SystemClock.uptimeMillis();
//         final MotionEvent ev = MotionEvent.obtain(mFakeDragBeginTime, time, MotionEvent.ACTION_MOVE,
//                 mLastMotionX, 0, 0);
//         mVelocityTracker.addMovement(ev);
//         ev.recycle();
//     }
    FakeDragBy(xOffset) {
        if (!mFakeDragging) {
            throw new IllegalStateException("No fake drag in progress. Call beginFakeDrag first.");
        }

        mLastMotionX += xOffset;

        var oldScrollX = getScrollX();
        var scrollX = oldScrollX - xOffset;
        var width = getClientWidth();

        var leftBound = width * mFirstOffset;
        var rightBound = width * mLastOffset;

        var firstItem = mItems.get(0);
        var lastItem = mItems.get(mItems.size() - 1);
        if (firstItem.position != 0) {
            leftBound = firstItem.offset * width;
        }
        if (lastItem.position != mAdapter.getCount() - 1) {
            rightBound = lastItem.offset * width;
        }

        if (scrollX < leftBound) {
            scrollX = leftBound;
        } else if (scrollX > rightBound) {
            scrollX = rightBound;
        }
        // Don't lose the rounded component
        mLastMotionX += scrollX - scrollX;
        scrollTo(scrollX, getScrollY());
        pageScrolled(scrollX);

        // Synthesize an event for the VelocityTracker.
        var time = SystemClock.uptimeMillis();
        var ev = MotionEvent.obtain(mFakeDragBeginTime, time, MotionEvent.ACTION_MOVE,
                mLastMotionX, 0, 0);
        mVelocityTracker.addMovement(ev);
        ev.recycle();
    }

    /**
     * Returns true if a fake drag is in progress.
     *
     * @return true if currently in a fake drag, false otherwise.
     *
     * @see #beginFakeDrag()
     * @see #fakeDragBy(float)
     * @see #endFakeDrag()
     */
//     public boolean isFakeDragging() {
//         return mFakeDragging;
//     }
    IsFakeDragging() {
        return mFakeDragging;
    }

//     private void onSecondaryPointerUp(MotionEvent ev) {
//         final int pointerIndex = MotionEventCompat.getActionIndex(ev);
//         final int pointerId = MotionEventCompat.getPointerId(ev, pointerIndex);
//         if (pointerId == mActivePointerId) {
//             // This was our active pointer going up. Choose a new
//             // active pointer and adjust accordingly.
//             final int newPointerIndex = pointerIndex == 0 ? 1 : 0;
//             mLastMotionX = MotionEventCompat.getX(ev, newPointerIndex);
//             mActivePointerId = MotionEventCompat.getPointerId(ev, newPointerIndex);
//             if (mVelocityTracker != null) {
//                 mVelocityTracker.clear();
//             }
//         }
//     }
    OnSecondaryPointerUp(ev) {
        var pointerIndex = MotionEventCompat.getActionIndex(ev);
        var pointerId = MotionEventCompat.getPointerId(ev, pointerIndex);
        if (pointerId == mActivePointerId) {
            // This was our active pointer going up. Choose a new
            // active pointer and adjust accordingly.
            var newPointerIndex = pointerIndex == 0 ? 1 : 0;
            mLastMotionX = MotionEventCompat.getX(ev, newPointerIndex);
            mActivePointerId = MotionEventCompat.getPointerId(ev, newPointerIndex);
            if (mVelocityTracker != null) {
                mVelocityTracker.clear();
            }
        }
    }

//     private void endDrag() {
//         mIsBeingDragged = false;
//         mIsUnableToDrag = false;

//         if (mVelocityTracker != null) {
//             mVelocityTracker.recycle();
//             mVelocityTracker = null;
//         }
//     }
    endDrag() {
        mIsBeingDragged = false;
        mIsUnableToDrag = false;

        if (mVelocityTracker != null) {
            mVelocityTracker.recycle();
            mVelocityTracker = null;
        }
    }

//     private void setScrollingCacheEnabled(boolean enabled) {
//         if (mScrollingCacheEnabled != enabled) {
//             mScrollingCacheEnabled = enabled;
//             if (USE_CACHE) {
//                 final int size = getChildCount();
//                 for (int i = 0; i < size; ++i) {
//                     final View child = getChildAt(i);
//                     if (child.getVisibility() != GONE) {
//                         child.setDrawingCacheEnabled(enabled);
//                     }
//                 }
//             }
//         }
//     }
    SetScrollingCacheEnabled(enabled) {
        elog("====ViewPager::SetScrollingCacheEnabled====begin====");

        var _this = this._obj;

        if (this.mScrollingCacheEnabled != enabled) {
            this.mScrollingCacheEnabled = enabled;
            if (ViewPager.USE_CACHE) {
                var size = _this.GetChildCount();
                for (var i = 0; i < size; ++i) {
                    var child = _this.GetChildAt(i);
                    if (child.GetVisibility() != IView__GONE) {
                        child.SetDrawingCacheEnabled(enabled);
                    }
                }
            }
        }

        elog("====ViewPager::SetScrollingCacheEnabled====end====");
    }

//     public boolean canScrollHorizontally(int direction) {
//         if (mAdapter == null) {
//             return false;
//         }

//         final int width = getClientWidth();
//         final int scrollX = getScrollX();
//         if (direction < 0) {
//             return (scrollX > (int) (width * mFirstOffset));
//         } else if (direction > 0) {
//             return (scrollX < (int) (width * mLastOffset));
//         } else {
//             return false;
//         }
//     }
    CanScrollHorizontally(direction) {
        if (mAdapter == null) {
            return false;
        }

        var width = getClientWidth();
        var scrollX = getScrollX();
        if (direction < 0) {
            return (scrollX > width * mFirstOffset);
        } else if (direction > 0) {
            return (scrollX < width * mLastOffset);
        } else {
            return false;
        }
    }

    /**
     * Tests scrollability within child views of v given a delta of dx.
     *
     * @param v View to test for horizontal scrollability
     * @param checkV Whether the view v passed should itself be checked for scrollability (true),
     *               or just its children (false).
     * @param dx Delta scrolled in pixels
     * @param x X coordinate of the active touch point
     * @param y Y coordinate of the active touch point
     * @return true if child views of v can be scrolled by delta of dx.
     */
//     protected boolean canScroll(View v, boolean checkV, int dx, int x, int y) {
//         if (v instanceof ViewGroup) {
//             final ViewGroup group = (ViewGroup) v;
//             final int scrollX = v.getScrollX();
//             final int scrollY = v.getScrollY();
//             final int count = group.getChildCount();
//             // Count backwards - let topmost views consume scroll distance first.
//             for (int i = count - 1; i >= 0; i--) {
//                 // TODO: Add versioned support here for transformed views.
//                 // This will not work for transformed views in Honeycomb+
//                 final View child = group.getChildAt(i);
//                 if (x + scrollX >= child.getLeft() && x + scrollX < child.getRight() &&
//                         y + scrollY >= child.getTop() && y + scrollY < child.getBottom() &&
//                         canScroll(child, true, dx, x + scrollX - child.getLeft(),
//                                 y + scrollY - child.getTop())) {
//                     return true;
//                 }
//             }
//         }

//         return checkV && ViewCompat.canScrollHorizontally(v, -dx);
//     }
    CanScroll(v, checkV, dx, x, y) {
        if (v instanceof ViewGroup) {
            var group = v;
            var scrollX = v.getScrollX();
            var scrollY = v.getScrollY();
            var count = group.getChildCount();
            // Count backwards - let topmost views consume scroll distance first.
            for (var i = count - 1; i >= 0; i--) {
                // TODO: Add versioned support here for transformed views.
                // This will not work for transformed views in Honeycomb+
                var child = group.getChildAt(i);
                if (x + scrollX >= child.getLeft() && x + scrollX < child.getRight() &&
                        y + scrollY >= child.getTop() && y + scrollY < child.getBottom() &&
                        canScroll(child, true, dx, x + scrollX - child.getLeft(),
                                y + scrollY - child.getTop())) {
                    return true;
                }
            }
        }

        return checkV && ViewCompat.canScrollHorizontally(v, -dx);
    }

//     @Override
//     public boolean dispatchKeyEvent(KeyEvent event) {
//         // Let the focused view and/or our descendants get the key first
//         return super.dispatchKeyEvent(event) || executeKeyEvent(event);
//     }
    DispatchKeyEvent(_this, event, result) {
        // Let the focused view and/or our descendants get the key first
        result.data = super.dispatchKeyEvent(event) || executeKeyEvent(event);
    }

    /**
     * You can call this function yourself to have the scroll view perform
     * scrolling from a key event, just as if the event had been dispatched to
     * it by the view hierarchy.
     *
     * @param event The key event to execute.
     * @return Return true if the event was handled, else false.
     */
//     public boolean executeKeyEvent(KeyEvent event) {
//         boolean handled = false;
//         if (event.getAction() == KeyEvent.ACTION_DOWN) {
//             switch (event.getKeyCode()) {
//                 case KeyEvent.KEYCODE_DPAD_LEFT:
//                     handled = arrowScroll(FOCUS_LEFT);
//                     break;
//                 case KeyEvent.KEYCODE_DPAD_RIGHT:
//                     handled = arrowScroll(FOCUS_RIGHT);
//                     break;
//                 case KeyEvent.KEYCODE_TAB:
//                     if (Build.VERSION.SDK_INT >= 11) {
//                         // The focus finder had a bug handling FOCUS_FORWARD and FOCUS_BACKWARD
//                         // before Android 3.0. Ignore the tab key on those devices.
//                         if (KeyEventCompat.hasNoModifiers(event)) {
//                             handled = arrowScroll(FOCUS_FORWARD);
//                         } else if (KeyEventCompat.hasModifiers(event, KeyEvent.META_SHIFT_ON)) {
//                             handled = arrowScroll(FOCUS_BACKWARD);
//                         }
//                     }
//                     break;
//             }
//         }
//         return handled;
//     }
    ExecuteKeyEvent(event) {
        var handled = false;
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            switch (event.getKeyCode()) {
                case KeyEvent.KEYCODE_DPAD_LEFT:
                    handled = arrowScroll(FOCUS_LEFT);
                    break;
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                    handled = arrowScroll(FOCUS_RIGHT);
                    break;
                case KeyEvent.KEYCODE_TAB:
                    if (Build.VERSION.SDK_INT >= 11) {
                        // The focus finder had a bug handling FOCUS_FORWARD and FOCUS_BACKWARD
                        // before Android 3.0. Ignore the tab key on those devices.
                        if (KeyEventCompat.hasNoModifiers(event)) {
                            handled = arrowScroll(FOCUS_FORWARD);
                        } else if (KeyEventCompat.hasModifiers(event, KeyEvent.META_SHIFT_ON)) {
                            handled = arrowScroll(FOCUS_BACKWARD);
                        }
                    }
                    break;
            }
        }
        return handled;
    }

//     public boolean arrowScroll(int direction) {
//         View currentFocused = findFocus();
//         if (currentFocused == this) {
//             currentFocused = null;
//         } else if (currentFocused != null) {
//             boolean isChild = false;
//             for (ViewParent parent = currentFocused.getParent(); parent instanceof ViewGroup;
//                     parent = parent.getParent()) {
//                 if (parent == this) {
//                     isChild = true;
//                     break;
//                 }
//             }
//             if (!isChild) {
//                 // This would cause the focus search down below to fail in fun ways.
//                 final StringBuilder sb = new StringBuilder();
//                 sb.append(currentFocused.getClass().getSimpleName());
//                 for (ViewParent parent = currentFocused.getParent(); parent instanceof ViewGroup;
//                         parent = parent.getParent()) {
//                     sb.append(" => ").append(parent.getClass().getSimpleName());
//                 }
//                 Log.e(TAG, "arrowScroll tried to find focus based on non-child " +
//                         "current focused view " + sb.toString());
//                 currentFocused = null;
//             }
//         }

//         boolean handled = false;

//         View nextFocused = FocusFinder.getInstance().findNextFocus(this, currentFocused,
//                 direction);
//         if (nextFocused != null && nextFocused != currentFocused) {
//             if (direction == View.FOCUS_LEFT) {
//                 // If there is nothing to the left, or this is causing us to
//                 // jump to the right, then what we really want to do is page left.
//                 final int nextLeft = getChildRectInPagerCoordinates(mTempRect, nextFocused).left;
//                 final int currLeft = getChildRectInPagerCoordinates(mTempRect, currentFocused).left;
//                 if (currentFocused != null && nextLeft >= currLeft) {
//                     handled = pageLeft();
//                 } else {
//                     handled = nextFocused.requestFocus();
//                 }
//             } else if (direction == View.FOCUS_RIGHT) {
//                 // If there is nothing to the right, or this is causing us to
//                 // jump to the left, then what we really want to do is page right.
//                 final int nextLeft = getChildRectInPagerCoordinates(mTempRect, nextFocused).left;
//                 final int currLeft = getChildRectInPagerCoordinates(mTempRect, currentFocused).left;
//                 if (currentFocused != null && nextLeft <= currLeft) {
//                     handled = pageRight();
//                 } else {
//                     handled = nextFocused.requestFocus();
//                 }
//             }
//         } else if (direction == FOCUS_LEFT || direction == FOCUS_BACKWARD) {
//             // Trying to move left and nothing there; try to page.
//             handled = pageLeft();
//         } else if (direction == FOCUS_RIGHT || direction == FOCUS_FORWARD) {
//             // Trying to move right and nothing there; try to page.
//             handled = pageRight();
//         }
//         if (handled) {
//             playSoundEffect(SoundEffectConstants.getContantForFocusDirection(direction));
//         }
//         return handled;
//     }
    ArrowScroll(direction) {
        var currentFocused = findFocus();
        if (currentFocused == this) {
            currentFocused = null;
        } else if (currentFocused != null) {
            var isChild = false;
            for (var parent = currentFocused.getParent(); parent instanceof ViewGroup;
                    parent = parent.getParent()) {
                if (parent == this) {
                    isChild = true;
                    break;
                }
            }
            if (!isChild) {
                // This would cause the focus search down below to fail in fun ways.
                var sb = new StringBuilder();
                sb.append(currentFocused.getClass().getSimpleName());
                for (var parent = currentFocused.getParent(); parent instanceof ViewGroup;
                        parent = parent.getParent()) {
                    sb.append(" => ").append(parent.getClass().getSimpleName());
                }
                Log.e(TAG, "arrowScroll tried to find focus based on non-child " +
                        "current focused view " + sb.toString());
                currentFocused = null;
            }
        }

        var handled = false;

        var nextFocused = FocusFinder.getInstance().findNextFocus(this, currentFocused,
                direction);
        if (nextFocused != null && nextFocused != currentFocused) {
            if (direction == View.FOCUS_LEFT) {
                // If there is nothing to the left, or this is causing us to
                // jump to the right, then what we really want to do is page left.
                var nextLeft = getChildRectInPagerCoordinates(mTempRect, nextFocused).left;
                var currLeft = getChildRectInPagerCoordinates(mTempRect, currentFocused).left;
                if (currentFocused != null && nextLeft >= currLeft) {
                    handled = pageLeft();
                } else {
                    handled = nextFocused.requestFocus();
                }
            } else if (direction == View.FOCUS_RIGHT) {
                // If there is nothing to the right, or this is causing us to
                // jump to the left, then what we really want to do is page right.
                var nextLeft = getChildRectInPagerCoordinates(mTempRect, nextFocused).left;
                var currLeft = getChildRectInPagerCoordinates(mTempRect, currentFocused).left;
                if (currentFocused != null && nextLeft <= currLeft) {
                    handled = pageRight();
                } else {
                    handled = nextFocused.requestFocus();
                }
            }
        } else if (direction == FOCUS_LEFT || direction == FOCUS_BACKWARD) {
            // Trying to move left and nothing there; try to page.
            handled = pageLeft();
        } else if (direction == FOCUS_RIGHT || direction == FOCUS_FORWARD) {
            // Trying to move right and nothing there; try to page.
            handled = pageRight();
        }
        if (handled) {
            playSoundEffect(SoundEffectConstants.getContantForFocusDirection(direction));
        }
        return handled;
    }

//     private Rect getChildRectInPagerCoordinates(Rect outRect, View child) {
//         if (outRect == null) {
//             outRect = new Rect();
//         }
//         if (child == null) {
//             outRect.set(0, 0, 0, 0);
//             return outRect;
//         }
//         outRect.left = child.getLeft();
//         outRect.right = child.getRight();
//         outRect.top = child.getTop();
//         outRect.bottom = child.getBottom();

//         ViewParent parent = child.getParent();
//         while (parent instanceof ViewGroup && parent != this) {
//             final ViewGroup group = (ViewGroup) parent;
//             outRect.left += group.getLeft();
//             outRect.right += group.getRight();
//             outRect.top += group.getTop();
//             outRect.bottom += group.getBottom();

//             parent = group.getParent();
//         }
//         return outRect;
//     }
    GetChildRectInPagerCoordinates(outRect, child) {
        if (outRect == null) {
            outRect = new Rect();
        }
        if (child == null) {
            outRect.set(0, 0, 0, 0);
            return outRect;
        }
        outRect.left = child.getLeft();
        outRect.right = child.getRight();
        outRect.top = child.getTop();
        outRect.bottom = child.getBottom();

        var parent = child.getParent();
        while (parent instanceof ViewGroup && parent != this) {
            var group = parent;
            outRect.left += group.getLeft();
            outRect.right += group.getRight();
            outRect.top += group.getTop();
            outRect.bottom += group.getBottom();

            parent = group.getParent();
        }
        return outRect;
    }

//     boolean pageLeft() {
//         if (mCurItem > 0) {
//             setCurrentItem(mCurItem-1, true);
//             return true;
//         }
//         return false;
//     }
    PageLeft() {
        if (this.mCurItem > 0) {
            setCurrentItem(this.mCurItem-1, true);
            return true;
        }
        return false;
    }

//     boolean pageRight() {
//         if (mAdapter != null && mCurItem < (mAdapter.getCount()-1)) {
//             setCurrentItem(mCurItem+1, true);
//             return true;
//         }
//         return false;
//     }
    PageRight() {
        if (this.mAdapter != null && this.mCurItem < (this.mAdapter.GetCount()-1)) {
            setCurrentItem(this.mCurItem+1, true);
            return true;
        }
        return false;
    }

//     /**
//      * We only want the current page that is being shown to be focusable.
//      */
//     @Override
//     public void addFocusables(ArrayList<View> views, int direction, int focusableMode) {
//         final int focusableCount = views.size();

//         final int descendantFocusability = getDescendantFocusability();

//         if (descendantFocusability != FOCUS_BLOCK_DESCENDANTS) {
//             for (int i = 0; i < getChildCount(); i++) {
//                 final View child = getChildAt(i);
//                 if (child.getVisibility() == VISIBLE) {
//                     ItemInfo ii = infoForChild(child);
//                     if (ii != null && ii.position == mCurItem) {
//                         child.addFocusables(views, direction, focusableMode);
//                     }
//                 }
//             }
//         }

//         // we add ourselves (if focusable) in all cases except for when we are
//         // FOCUS_AFTER_DESCENDANTS and there are some descendants focusable.  this is
//         // to avoid the focus search finding layouts when a more precise search
//         // among the focusable children would be more interesting.
//         if (
//             descendantFocusability != FOCUS_AFTER_DESCENDANTS ||
//                 // No focusable descendants
//                 (focusableCount == views.size())) {
//             // Note that we can't call the superclass here, because it will
//             // add all views in.  So we need to do the same thing View does.
//             if (!isFocusable()) {
//                 return;
//             }
//             if ((focusableMode & FOCUSABLES_TOUCH_MODE) == FOCUSABLES_TOUCH_MODE &&
//                     isInTouchMode() && !isFocusableInTouchMode()) {
//                 return;
//             }
//             if (views != null) {
//                 views.add(this);
//             }
//         }
//     }
    AddFocusables(_this, views, direction, focusableMode) {
        var focusableCount = views.size();

        var descendantFocusability = getDescendantFocusability();

        if (descendantFocusability != FOCUS_BLOCK_DESCENDANTS) {
            for (var i = 0; i < getChildCount(); i++) {
                var child = getChildAt(i);
                if (child.getVisibility() == VISIBLE) {
                    var ii = infoForChild(child);
                    if (ii != null && ii.position == mCurItem) {
                        child.addFocusables(views, direction, focusableMode);
                    }
                }
            }
        }

        // we add ourselves (if focusable) in all cases except for when we are
        // FOCUS_AFTER_DESCENDANTS and there are some descendants focusable.  this is
        // to avoid the focus search finding layouts when a more precise search
        // among the focusable children would be more interesting.
        if (
            descendantFocusability != FOCUS_AFTER_DESCENDANTS ||
                // No focusable descendants
                (focusableCount == views.size())) {
            // Note that we can't call the superclass here, because it will
            // add all views in.  So we need to do the same thing View does.
            if (!isFocusable()) {
                return;
            }
            if ((focusableMode & FOCUSABLES_TOUCH_MODE) == FOCUSABLES_TOUCH_MODE &&
                    isInTouchMode() && !isFocusableInTouchMode()) {
                return;
            }
            if (views != null) {
                views.add(this);
            }
        }
    }

//     /**
//      * We only want the current page that is being shown to be touchable.
//      */
//     @Override
//     public void addTouchables(ArrayList<View> views) {
//         // Note that we don't call super.addTouchables(), which means that
//         // we don't call View.addTouchables().  This is okay because a ViewPager
//         // is itself not touchable.
//         for (int i = 0; i < getChildCount(); i++) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() == VISIBLE) {
//                 ItemInfo ii = infoForChild(child);
//                 if (ii != null && ii.position == mCurItem) {
//                     child.addTouchables(views);
//                 }
//             }
//         }
//     }
    AddTouchables(_this, views) {
        // Note that we don't call super.addTouchables(), which means that
        // we don't call View.addTouchables().  This is okay because a ViewPager
        // is itself not touchable.
        for (var i = 0; i < getChildCount(); i++) {
            var child = getChildAt(i);
            if (child.getVisibility() == VISIBLE) {
                var ii = this.InfoForChild(child);
                if (ii != null && ii.position == this.mCurItem) {
                    child.addTouchables(views);
                }
            }
        }
    }

//     /**
//      * We only want the current page that is being shown to be focusable.
//      */
//     @Override
//     protected boolean onRequestFocusInDescendants(int direction,
//             Rect previouslyFocusedRect) {
//         int index;
//         int increment;
//         int end;
//         int count = getChildCount();
//         if ((direction & FOCUS_FORWARD) != 0) {
//             index = 0;
//             increment = 1;
//             end = count;
//         } else {
//             index = count - 1;
//             increment = -1;
//             end = -1;
//         }
//         for (int i = index; i != end; i += increment) {
//             View child = getChildAt(i);
//             if (child.getVisibility() == VISIBLE) {
//                 ItemInfo ii = infoForChild(child);
//                 if (ii != null && ii.position == mCurItem) {
//                     if (child.requestFocus(direction, previouslyFocusedRect)) {
//                         return true;
//                     }
//                 }
//             }
//         }
//         return false;
//     }
    OnRequestFocusInDescendants(_this, direction,
            previouslyFocusedRect, result) {
        var index;
        var increment;
        var end;
        var count = getChildCount();
        if ((direction & FOCUS_FORWARD) != 0) {
            index = 0;
            increment = 1;
            end = count;
        } else {
            index = count - 1;
            increment = -1;
            end = -1;
        }
        for (var i = index; i != end; i += increment) {
            var child = getChildAt(i);
            if (child.getVisibility() == VISIBLE) {
                var ii = this.InfoForChild(child);
                if (ii != null && ii.position == this.mCurItem) {
                    if (child.requestFocus(direction, previouslyFocusedRect)) {
                        return true;
                    }
                }
            }
        }
        result.data = false;
    }

//     @Override
//     public boolean dispatchPopulateAccessibilityEvent(AccessibilityEvent event) {
//         // Dispatch scroll events from this ViewPager.
//         if (event.getEventType() == AccessibilityEventCompat.TYPE_VIEW_SCROLLED) {
//             return super.dispatchPopulateAccessibilityEvent(event);
//         }

//         // Dispatch all other accessibility events from the current page.
//         final int childCount = getChildCount();
//         for (int i = 0; i < childCount; i++) {
//             final View child = getChildAt(i);
//             if (child.getVisibility() == VISIBLE) {
//                 final ItemInfo ii = infoForChild(child);
//                 if (ii != null && ii.position == mCurItem &&
//                         child.dispatchPopulateAccessibilityEvent(event)) {
//                     return true;
//                 }
//             }
//         }

//         return false;
//     }
    DispatchPopulateAccessibilityEvent(_this, event, result) {
        // Dispatch scroll events from this ViewPager.
        if (event.getEventType() == AccessibilityEventCompat.TYPE_VIEW_SCROLLED) {
            result.data = _super.dispatchPopulateAccessibilityEvent(event);
            return;
        }

        // Dispatch all other accessibility events from the current page.
        var childCount = getChildCount();
        for (var i = 0; i < childCount; i++) {
            var child = getChildAt(i);
            if (child.getVisibility() == VISIBLE) {
                var ii = this.InfoForChild(child);
                if (ii != null && ii.position == this.mCurItem &&
                        child.dispatchPopulateAccessibilityEvent(event)) {
                    result.data = true;
                    return;
                }
            }
        }

        result.data = false;
        return;
    }

//     @Override
//     protected ViewGroup.LayoutParams generateDefaultLayoutParams() {
//         return new LayoutParams();
//     }
    GenerateDefaultLayoutParams(_this, result) {
        elog("====ViewPager::GenerateDefaultLayoutParams====begin====");
        result.data = new ViewPager.LayoutParams(_this);
        elog("====ViewPager::GenerateDefaultLayoutParams====end====");
    }

//     @Override
//     protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
//         return generateDefaultLayoutParams();
//     }

//     @Override
//     protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
//         return p instanceof LayoutParams && super.checkLayoutParams(p);
//     }
    CheckLayoutParams(_this, p, result) {
        elog("====ViewPager::CheckLayoutParams====begin====");

        if (arguments.length == 1) {
            elog("====ViewPager::CheckLayoutParams====0====");
            p = _this;
            _this = this._obj;
            result = null;
        }

        var bRet = false;

function getClassName(o){
    var r = (o&&o.getClass&&o.getClass().GetName()) || typeof o;
    if(r == "function")r = o.name;
    return r;
}
var n1 = getClassName(p);
elog("====ssss==0=="+n1);
// if(n1=="object"){
//     CObject.showMethods(p);
// }
var n2 = getClassName(ViewPager.LayoutParams);
elog("====ssss==1=="+n1+"===="+n2);

// elog("====ssss====hasInterface:"+typeof ViewPager.LayoutParams.hasInterface);

//         var bRet = p instanceof ViewPager.LayoutParams
//         elog("====ViewPager::CheckLayoutParams====0===="+bRet);
//         bRet = bRet && super.CheckLayoutParams(p);

        // elog("====ViewPager::CheckLayoutParams==== p0 className===="+(p instanceof ViewPager.LayoutParams));
        // elog("====ViewPager::CheckLayoutParams==== p className===="+n1);
        // elog("====ViewPager::CheckLayoutParams==== _this className===="+getClassName(_this));


        // //if (n1 == "IViewGroupLayoutParams"){
        // if (n1 == "CViewGroupLayoutParams"){
        //     bRet = true;
        // }
// elog("====sssspppp===="+n1+"===="+(n1 == "IViewGroupLayoutParams")+"===="+bRet);

CObject.showMethods(p);

        bRet = p instanceof ViewPager.LayoutParams;

//if(n1 == "CViewGroupLayoutParams" || n1 == "CViewGroupMarginLayoutParams") {
if(n1.indexOf("LayoutParams")>-1) {
    elog("====ViewPager::CheckLayoutParams====0====result:"+bRet);
    bRet = true;
}
else {
    elog("====ViewPager::CheckLayoutParams====0====n1:"+n1);
    ss.tt();Assert(0);
}
        bRet = bRet && _this._CheckLayoutParams(p);

        elog("====ViewPager::CheckLayoutParams====end====result:"+bRet);

        if (result) {
            elog("====ViewPager::CheckLayoutParams====end====callback====");
            result.data = bRet;
            return bRet;
        }
        else {
            elog("====ViewPager::CheckLayoutParams====end====call====");
            return bRet;
        }
    }

//     @Override
//     public ViewGroup.LayoutParams generateLayoutParams(AttributeSet attrs) {
//         return new LayoutParams(getContext(), attrs);
//     }
    GenerateLayoutParams(_this, attrs, result) {
        elog("====ViewPager::GenerateLayoutParams====begin====");
        result.data = new ViewPager.LayoutParams(_this, _this.GetContext(), attrs);

        // elog("====ViewPager::GenerateLayoutParams====0====");
        // CObject.showMethods(result.data._obj);
        // elog("====ViewPager::GenerateLayoutParams====1====");

        // result.data.isDecor = true;
        // CObject.showMethods(result.data);
        // elog("====ViewPager::GenerateLayoutParams====2====");

        //elog("====ViewPager::GenerateLayoutParams====name:"+result.data.getClass().GetName());

        elog("====ViewPager::GenerateLayoutParams====end====");
    }

//     class MyAccessibilityDelegate extends AccessibilityDelegateCompat {
    get MyAccessibilityDelegate() {
        elog("====ViewPager::MyAccessibilityDelegate====begin====");
        if (this._MyAccessibilityDelegate_) return this._MyAccessibilityDelegate;
        this._MyAccessibilityDelegate_ = true;
        this._MyAccessibilityDelegate = class _tmp extends AccessibilityDelegateCompat {

//         @Override
//         public void onInitializeAccessibilityEvent(View host, AccessibilityEvent event) {
            OnInitializeAccessibilityEvent(host, event) {
                elog("====ViewPager::MyAccessibilityDelegate::OnInitializeAccessibilityEvent====TODO====");
                Assert(0);
//             super.onInitializeAccessibilityEvent(host, event);
//             event.setClassName(ViewPager.class.getName());
//             final AccessibilityRecordCompat recordCompat = AccessibilityRecordCompat.obtain();
//             recordCompat.setScrollable(canScroll());
//             if (event.getEventType() == AccessibilityEventCompat.TYPE_VIEW_SCROLLED
//                     && mAdapter != null) {
//                 recordCompat.setItemCount(mAdapter.getCount());
//                 recordCompat.setFromIndex(mCurItem);
//                 recordCompat.setToIndex(mCurItem);
//             }
            }

//         @Override
//         public void onInitializeAccessibilityNodeInfo(View host, AccessibilityNodeInfoCompat info) {
            OnInitializeAccessibilityNodeInfo(host, info) {
                elog("====ViewPager::MyAccessibilityDelegate::OnInitializeAccessibilityNodeInfo====TODO====");
                Assert(0);
//             super.onInitializeAccessibilityNodeInfo(host, info);
//             info.setClassName(ViewPager.class.getName());
//             info.setScrollable(canScroll());
//             if (canScrollHorizontally(1)) {
//                 info.addAction(AccessibilityNodeInfoCompat.ACTION_SCROLL_FORWARD);
//             }
//             if (canScrollHorizontally(-1)) {
//                 info.addAction(AccessibilityNodeInfoCompat.ACTION_SCROLL_BACKWARD);
//             }
            }

//         @Override
//         public boolean performAccessibilityAction(View host, int action, Bundle args) {
            PerformAccessibilityAction(host, action, args, result) {
                elog("====ViewPager::MyAccessibilityDelegate::PerformAccessibilityAction====TODO====");
                Assert(0);
//             if (super.performAccessibilityAction(host, action, args)) {
//                 return true;
//             }
//             switch (action) {
//                 case AccessibilityNodeInfoCompat.ACTION_SCROLL_FORWARD: {
//                     if (canScrollHorizontally(1)) {
//                         setCurrentItem(mCurItem + 1);
//                         return true;
//                     }
//                 } return false;
//                 case AccessibilityNodeInfoCompat.ACTION_SCROLL_BACKWARD: {
//                     if (canScrollHorizontally(-1)) {
//                         setCurrentItem(mCurItem - 1);
//                         return true;
//                     }
//                 } return false;
//             }
//             return false;
            }

//         private boolean canScroll() {
            CanScroll(result) {
                elog("====ViewPager::MyAccessibilityDelegate::CanScroll====TODO====");
                Assert(0);
//             return (mAdapter != null) && (mAdapter.getCount() > 1);
                return (this.mAdapter != null) && (this.mAdapter.GetCount() > 1);
            }
        };

        return this._MyAccessibilityDelegate;
    }

//     private class PagerObserver extends DataSetObserver {
//         @Override
//         public void onChanged() {
//             dataSetChanged();
//         }
//         @Override
//         public void onInvalidated() {
//             dataSetChanged();
//         }
//     }
    get PagerObserver() {
        if (this._PagerObserver) return this._PagerObserver;

        var _this = this;
        this._PagerObserver = class PagerObserver extends DataSetObserver {
            OnChanged() {
                elog("====PagerObserver::OnChanged====");
                _this.DataSetChanged();
            }
            OnInvalidated() {
                elog("====PagerObserver::OnInvalidated====");
                _this.DataSetChanged();
            }
        };
        return this._PagerObserver;
    }

    /**
     * Layout parameters that should be supplied for views added to a
     * ViewPager.
     */
//     public static class LayoutParams extends ViewGroup.LayoutParams {
    static get LayoutParams() {
        elog("====ViewPager::LayoutParams====begin====");
        if (ViewPager._LayoutParams) {
            elog("====ViewPager::LayoutParams====end====0====");
            return ViewPager._LayoutParams;
        }

        elog("====ViewPager::LayoutParams====0===="+ViewPager._LayoutParams_);
        ViewPager._LayoutParams = class LayoutParams extends ViewGroup.LayoutParams {

//         /**
//          * true if this view is a decoration on the pager itself and not
//          * a view supplied by the adapter.
//          */
//         public boolean isDecor;
            get isDecor() {
                elog("====get ViewPager::LayoutParams::isDecor====");
                return this._isDecor;
            }
            set isDecor(v) {
                elog("====set ViewPager::LayoutParams::isDecor====");
                this._isDecor = v;
                return v;
            }

//         /**
//          * Gravity setting for use on decor views only:
//          * Where to position the view page within the overall ViewPager
//          * container; constants are defined in {@link android.view.Gravity}.
//          */
//         public int gravity;
            get gravity() {
                elog("====get ViewPager::LayoutParams::gravity====");
                return this._gravity || 0;
            }
            set gravity(v) {
                elog("====set ViewPager::LayoutParams::gravity====");
                this._gravity = v;
                return v;
            }

//         /**
//          * Width as a 0-1 multiplier of the measured pager width
//          */
//         float widthFactor = 0.f;
            get widthFactor() {
                elog("====get ViewPager::LayoutParams::widthFactor====");
                return this._widthFactor || 0;
            }
            set widthFactor(v) {
                elog("====set ViewPager::LayoutParams::widthFactor====");
                this._widthFactor = v;
                return v;
            }

//         /**
//          * true if this view was added during layout and needs to be measured
//          * before being positioned.
//          */
//         boolean needsMeasure;
            get needsMeasure() {
                elog("====get ViewPager::LayoutParams::needsMeasure====");
                return this._needsMeasure || false;
            }
            set needsMeasure(v) {
                elog("====set ViewPager::LayoutParams::needsMeasure====");
                this._needsMeasure = v;
                return v;
            }

//         /**
//          * Adapter position this view is for if !isDecor
//          */
//         int position;
            get position() {
                elog("====get ViewPager::LayoutParams::position====");
                return this._position || 0;
            }
            set position(v) {
                elog("====set ViewPager::LayoutParams::position====");
                this._position = v;
                return v;
            }

//         /**
//          * Current child index within the ViewPager that this view occupies
//          */
//         int childIndex;
            get childIndex() {
                elog("====get ViewPager::LayoutParams::childIndex====");
                return this._childIndex || 0;
            }
            set childIndex(v) {
                elog("====set ViewPager::LayoutParams::childIndex====");
                this._childIndex = v;
                return v;
            }

//         public LayoutParams() {
//             super(FILL_PARENT, FILL_PARENT);
//         }

//         public LayoutParams(Context context, AttributeSet attrs) {
//             super(context, attrs);

//             final TypedArray a = context.obtainStyledAttributes(attrs, LAYOUT_ATTRS);
//             gravity = a.getInteger(0, Gravity.TOP);
//             a.recycle();
//         }
            constructor(_this, context, attrs) {
                elog("====ViewPager::LayoutParams::constructor====");

                if (arguments.length == 1) {
                    super(_this, LayoutParams__FILL_PARENT, LayoutParams__FILL_PARENT);
                }
                else {
                    var typeName = attrs.getClass?attrs.getClass().GetName():typeof attrs;

                    if (typeName == "IAttributeSet") {
                        super(_this, context, attrs);

                        elog("====ViewPager::LayoutParams::constructor====0====" + typeName);
                        var a = context.ObtainStyledAttributes(attrs, ViewPager.LAYOUT_ATTRS);
                        elog("====ViewPager::LayoutParams::constructor====1====");
                        this.gravity = a.GetInteger(0, Gravity.TOP);
                        elog("====ViewPager::LayoutParams::constructor====2====");
                        a.Recycle();
                        elog("====ViewPager::LayoutParams::constructor====3====");
                    }
                    else {
                        elog("====ViewPager::LayoutParams::constructor====4====" + typeName);
                        super(_this, context, attrs);
                        elog("====ViewPager::LayoutParams::constructor====5====");
                    }
                }   //else

                // return new Proxy(this._obj, {
                //     set
                // });

                elog("====ViewPager::LayoutParams::constructor====end====");
            }

        };

        elog("====ViewPager::LayoutParams====end====1====");
        return this._LayoutParams;
    }

//     static class ViewPositionComparator implements Comparator<View> {
    static get ViewPositionComparator() {
        elog("====ViewPager::ViewPositionComparator====");
        if (this._ViewPositionComparator_) return this._ViewPositionComparator;
        this._ViewPositionComparator_ = true;
        this._ViewPositionComparator = class _ {

//         @Override
//         public int compare(View lhs, View rhs) {
//             final LayoutParams llp = (LayoutParams) lhs.getLayoutParams();
//             final LayoutParams rlp = (LayoutParams) rhs.getLayoutParams();
//             if (llp.isDecor != rlp.isDecor) {
//                 return llp.isDecor ? 1 : -1;
//             }
//             return llp.position - rlp.position;
//         }
        Compare(lhs, rhs) {
            var llp = lhs.GetLayoutParams();
            var rlp = rhs.GetLayoutParams();
            if (llp.isDecor != rlp.isDecor) {
                return llp.isDecor ? 1 : -1;
            }
            return llp.position - rlp.position;
        }

        };
        return this._ViewPositionComparator;
    }

}   //class ViewPager

//--------.java----end----

    return ViewPager;
};  //module.exports

elog("====ViewPager.js====end====");