elog("====NodeLinearLayout.js====begin====");

module.exports = function(aoElastos, aoActivity){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    var IView__VISIBLE = 0x00000000;

    var _this;

//--------common definition----end----

    var IView__LAYOUT_DIRECTION_RTL = 1;
    var IView__GONE = 0x00000008;  //IVew::GONE

//--------.java----begin----

// /*
//  * Copyright (C) 2014 The Android Open Source Project
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *   http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// package com.android.calculator2;

// import android.content.Context;
// import android.content.res.TypedArray;
// import android.util.AttributeSet;
// import android.view.View;
    var View;   // = Elastos.Droid.View.IView;
// import android.view.ViewGroup;
    var ViewGroup = class _ViewGroup {};  // = Elastos.Droid.View.IViewGroup;

/**
 * A layout that places children in an evenly distributed grid based on the specified
 *  {@link android.R.attr#columnCount} and {@link android.R.attr#rowCount} attributes.
 */
// public class CalculatorPadLayout extends ViewGroup {
class NodeLinearLayout extends ViewGroup {
    constructor() {
        super();
    }

//     private int mRowCount;
//     private int mColumnCount;

//     public CalculatorPadLayout(Context context) {
//         this(context, null);
//     }

//     public CalculatorPadLayout(Context context, AttributeSet attrs) {
//         this(context, attrs, 0);
//     }

//     public CalculatorPadLayout(Context context, AttributeSet attrs, int defStyle) {
//         super(context, attrs, defStyle);

//         final TypedArray a = context.obtainStyledAttributes(attrs,
//                 new int[] { android.R.attr.rowCount, android.R.attr.columnCount }, defStyle, 0);
//         mRowCount = a.getInt(0, 1);
//         mColumnCount = a.getInt(1, 1);

//         a.recycle();
//     }
    OnCreate(_this, context, attrs, defStyle) {
        elog("====NodeLinearLayout::OnCreate====begin===");

        this._obj = _this;

        attrs = attrs || null;
        defStyle = defStyle || 0;

        _this._constructor(context, attrs, defStyle);

        elog("====NodeLinearLayout::OnCreate====end===");
    }

//     @Override
    ShouldDelayChildPressedState(_this, result) {
        elog("====NodeLinearLayout::ShouldDelayChildPressedState====begin===");
        result.data = false;
        elog("====NodeLinearLayout::ShouldDelayChildPressedState====end===");
    }

//     @Override
    OnMeasure(_this, widthMeasureSpec, heightMeasureSpec) {
        elog("====NodeLinearLayout::OnMeasure====begin====");
        _this._OnMeasure(widthMeasureSpec, heightMeasureSpec);
        elog("====NodeLinearLayout::OnMeasure====end====");
    }

//     @Override
    Draw(_this, canvas) {
        elog("====NodeLinearLayout::Draw====begin====");
        elog("====NodeLinearLayout::Draw====0====args:"+arguments.length);

        _this._Draw(canvas);

        elog("====NodeLinearLayout::Draw====end====");
    }

//     @Override
    DrawChild(_this, canvas, child, drawingTime, result)
    {
        elog("====NodeLinearLayout::DrawChild====begin====");

        var bRet = false;

var n=child.getClass().GetName();
elog("====NodeLinearLayout::DrawChild====child name:===="+n);

        elog("====NodeLinearLayout::DrawChild====0.0====");
        bRet = _this._DrawChild(canvas, child, drawingTime);

CObject.showMethods(child,"Draw");
        //bRet = child.Draw(canvas, _this, drawingTime);
        elog("====NodeLinearLayout::DrawChild====0.1====");

        if (result) {
            result.data = bRet;
            elog("====NodeLinearLayout::DrawChild====end====0====");
            return; //ECode should be return
        }
        else {
            elog("====NodeLinearLayout::DrawChild====end====1====");
            return bRet;
        }
    }

//     @Override
    DispatchDraw(_this, canvas) {
        elog("====NodeLinearLayout::DispatchDraw====begin====");

_this.SetBackgroundColor(567);


        _this._DispatchDraw(canvas);
        elog("====NodeLinearLayout::DispatchDraw====end====");
    }

//     @Override
    OnDraw(_this, canvas) {
        elog("====NodeLinearLayout::OnDraw====begin====");
        //ss.tt();Assert(0);
        _this._OnDraw(canvas);
        elog("====NodeLinearLayout::OnDraw====end====");
    }

//     @Override
    OnLayout(_this, changed, left, top, right, bottom) {
        elog("====NodeLinearLayout::OnLayout====begin=======");

        elog(`====NodeLinearLayout::OnLayout====0.0===left:${left}==top:${top}==right:${right}==bottom:${bottom}==`);

        _this._OnLayout(changed, left, top, right, bottom);

        elog("====NodeLinearLayout::OnLayout====end=======");
    }

    get CLinearLayoutLayoutParams() {
        if (this._CLinearLayoutLayoutParams)return this._CLinearLayoutLayoutParams;
        this._CLinearLayoutLayoutParams = class CLinearLayoutLayoutParams {
            constructor(ctx, attrs) {
                var oRet;

                // oRet = Droid_New("Elastos.Droid.Widget.CLinearLayoutLayoutParams", ctx, attrs);
                // return oRet;

                var _obj;
                var attrsClassName = attrs.getClass().GetName();
                if (attrsClassName=="IAttributeSet") {
                    _obj = Droid_New("Elastos.Droid.Widget.CLinearLayoutLayoutParams", ctx, attrs);
                }
                else {
                    _obj = attrs;
                    elog("====NodeLinearLayout::CLinearLayoutLayoutParams::get====not IAttributeSet!");
                    ss.tt();
                }
                // oRet = _obj;
                // return oRet;

                var _prop = {
                    //
                };

                //var lp1 = new Proxy(_obj, {
                var lp1 = new Proxy({}, {
                    set:function(target, key, value, receiver){
                        elog("====NodeLinearLayout::CLinearLayoutLayoutParams::Proxy::set===="+key);
                        //return Reflect.set(target, key, value, receiver);
                        _prop[key] = value;
                        return value;
                    },
                    get:function(target, key, receiver){
                        elog("====NodeLinearLayout::CLinearLayoutLayoutParams::Proxy::get===="+key);
                        //return Reflect.get(target, key, receiver);

                        //TODO:first check target._prop, then check target._obj
                        if (typeof _prop[key] != "undefined") {
                            return _prop[key];
                        }

                        var oGetFn = function() {
                            elog("====GetFn====begin==01=="+key);

                            var args = [...arguments];
                            var len = args.length;

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

                            //var r = target[key].apply(target, args_in);
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
                        }
                        return oGetFn;
                    },
                });

                oRet = lp1;
                return oRet;
            }
        }
        return this._CLinearLayoutLayoutParams;
    }



//     @Override
    GenerateLayoutParams(_this, attrs, result) {
        elog("====NodeLinearLayout::GenerateLayoutParams====begin===attr");

        //result.data = _this._GenerateLayoutParams(attrs);
        //elog("====NodeLinearLayout::GenerateLayoutParams====end=======");

        var ctx = _this.GetContext();
        //var lp = Droid_New("Elastos.Droid.Widget.CLinearLayoutLayoutParams", ctx, attrs);
        var lp = new this.CLinearLayoutLayoutParams(ctx, attrs);

        if(result){
            result.data = lp;
            elog("====NodeLinearLayout::GenerateLayoutParams====end===0====attr");
            return; //return ECode, called from c++
        }
        elog("====NodeLinearLayout::GenerateLayoutParams====end===1====attr");
        return lp;  //return Object, called from js
    }


//     @Override
    GenerateDefaultLayoutParams(_this, result) {
        elog("====NodeLinearLayout::GenerateDefaultLayoutParams====begin=======");
        result.data = _this._GenerateDefaultLayoutParams();
        elog("====NodeLinearLayout::GenerateDefaultLayoutParams====end=======");
    }

//     @Override
    GenerateLayoutParams_0(_this, p, result) {
        elog("====NodeLinearLayout::GenerateLayoutParams_0====begin=======");
        result.data = _this._GenerateLayoutParams(p);
        elog("====NodeLinearLayout::GenerateLayoutParams_0====end=======");
    }


//     @Override
    CheckLayoutParams(_this, p, result) {
        elog("====NodeLinearLayout::CheckLayoutParams====begin=======");
        result.data = _this._CheckLayoutParams(p);
        elog("====NodeLinearLayout::CheckLayoutParams====end=======");
    }

}   //class _CalculatorPadLayout

//--------.java----end----

return new NodeLinearLayout();

};  //module.exports

elog("====NodeLinearLayout.js====end====");