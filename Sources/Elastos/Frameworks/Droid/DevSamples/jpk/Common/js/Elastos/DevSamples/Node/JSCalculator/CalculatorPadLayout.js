elog("====CalculatorPadLayout.js====begin====");

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
class CalculatorPadLayout extends ViewGroup {
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
        elog("====CalculatorPadLayout::OnCreate====begin=======");

        this._obj = _this;

        attrs = attrs || null;
        defStyle = defStyle || 0;

        _this._constructor(context, attrs, defStyle);

        var Elastos__Droid__R__attr__rowCount = 0x01010375;
        var Elastos__Droid__R__attr__columnCount = 0x01010377;
        var a = context.ObtainStyledAttributes(attrs, [Elastos__Droid__R__attr__rowCount, Elastos__Droid__R__attr__columnCount], defStyle, 0);
        this.mRowCount = a.GetInt32(0, 1);
        this.mColumnCount = a.GetInt32(1, 1);

        elog(`====CalculatorPadLayout::OnCreate====mRowCount:${this.mRowCount};mColumnCount:${this.mColumnCount}===`);

        a.Recycle();

        elog("====CalculatorPadLayout::OnCreate====end=======");
    }

//     @Override
//     public boolean shouldDelayChildPressedState() {
//         return false;
//     }
    ShouldDelayChildPressedState(_this, result) {
        result.data = false;
    }

//     @Override
    OnMeasure(_this, widthMeasureSpec, heightMeasureSpec) {

        elog("====CalculatorPadLayout::OnMeasure====begin====");
        _this._OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

//     @Override
    Draw(_this, canvas) {
        elog("====CalculatorPadLayout::Draw====begin====");
        _this._Draw(canvas);
        elog("====CalculatorPadLayout::Draw====end====");
    }

//     @Override
    DrawChild(_this, canvas, child, drawingTime, result)
    {
        elog("====CalculatorPadLayout::DrawChild====begin====");
        var bRet = _this._DrawChild(canvas, child, drawingTime);
        if (result) {
            result.data = bRet;
            elog("====CalculatorPadLayout::DrawChild====end====0====");
            return; //ECode should be return
        }
        else {
            elog("====CalculatorPadLayout::DrawChild====end====1====");
            return bRet;
        }
    }

//     @Override
    DispatchDraw(_this, canvas) {
        elog("====CalculatorPadLayout::DispatchDraw====begin====");
        //ss.tt();Assert(0);

        var cnt = _this.GetChildCount();
        var pName = _this.GetParent().getClass().GetName();
        elog(`====CalculatorPadLayout::DispatchDraw====par:${pName},cnt:${cnt}====`);

        _this._DispatchDraw(canvas);
        elog("====CalculatorPadLayout::DispatchDraw====end====");
    }

//     @Override
    OnDraw(_this, canvas) {
        elog("====CalculatorPadLayout::OnDraw====begin====");
        //ss.tt();Assert(0);
        _this._OnDraw(canvas);
        elog("====CalculatorPadLayout::OnDraw====end====");
    }

//     @Override
//     protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
//         final int paddingLeft = getPaddingLeft();
//         final int paddingRight = getPaddingRight();
//         final int paddingTop = getPaddingTop();
//         final int paddingBottom = getPaddingBottom();

//         final boolean isRTL = getLayoutDirection() == LAYOUT_DIRECTION_RTL;
//         final int columnWidth =
//                 Math.round((float) (right - left - paddingLeft - paddingRight)) / mColumnCount;
//         final int rowHeight =
//                 Math.round((float) (bottom - top - paddingTop - paddingBottom)) / mRowCount;

//         int rowIndex = 0, columnIndex = 0;
//         for (int childIndex = 0; childIndex < getChildCount(); ++childIndex) {
//             final View childView = getChildAt(childIndex);
//             if (childView.getVisibility() == View.GONE) {
//                 continue;
//             }

//             final MarginLayoutParams lp = (MarginLayoutParams) childView.getLayoutParams();

//             final int childTop = paddingTop + lp.topMargin + rowIndex * rowHeight;
//             final int childBottom = childTop - lp.topMargin - lp.bottomMargin + rowHeight;
//             final int childLeft = paddingLeft + lp.leftMargin +
//                     (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
//             final int childRight = childLeft - lp.leftMargin - lp.rightMargin + columnWidth;

//             final int childWidth = childRight - childLeft;
//             final int childHeight = childBottom - childTop;
//             if (childWidth != childView.getMeasuredWidth() ||
//                     childHeight != childView.getMeasuredHeight()) {
//                 childView.measure(
//                         MeasureSpec.makeMeasureSpec(childWidth, MeasureSpec.EXACTLY),
//                         MeasureSpec.makeMeasureSpec(childHeight, MeasureSpec.EXACTLY));
//             }
//             childView.layout(childLeft, childTop, childRight, childBottom);

//             rowIndex = (rowIndex + (columnIndex + 1) / mColumnCount) % mRowCount;
//             columnIndex = (columnIndex + 1) % mColumnCount;
//         }
//     }
    OnLayout(_this, changed, left, top, right, bottom) {
        elog("====CalculatorPadLayout::OnLayout====begin=======");

        elog(`====CalculatorPadLayout::OnLayout====0.0===left:${left}==top:${top}==right:${right}==bottom:${bottom}==`);

        //var _this = this._obj;

        //CObject.showMethods(_this,"GetPadding");
        //ss.tt();Assert(0);

        var paddingLeft = _this.GetPaddingLeft();
        var paddingRight = _this.GetPaddingRight();
        var paddingTop = _this.GetPaddingTop();
        var paddingBottom = _this.GetPaddingBottom();

        elog(`====CalculatorPadLayout::OnLayout====0.1===padding:${paddingLeft}:${paddingRight}:${paddingTop}:${paddingBottom}==`);

        var isRTL = _this.GetLayoutDirection() == IView__LAYOUT_DIRECTION_RTL;
        var columnWidth = Math.round(right - left - paddingLeft - paddingRight) / this.mColumnCount;
        var rowHeight = Math.round(bottom - top - paddingTop - paddingBottom) / this.mRowCount;

        elog(`====CalculatorPadLayout::OnLayout====0.2===columnWidth:${columnWidth}==rowHeight:${rowHeight}==`);

        var rowIndex = 0, columnIndex = 0;
        var childCount = _this.GetChildCount();

        elog(`====CalculatorPadLayout::OnLayout====0.3===childCount:${childCount}==`);
        for (var childIndex = 0; childIndex < childCount; ++childIndex) {

            elog(`====CalculatorPadLayout::OnLayout====1====${childIndex}/${childCount}`);

            var childView = _this.GetChildAt(childIndex);
            if (childView.GetVisibility() == IView__GONE) {
                continue;
            }

            elog("====CalculatorPadLayout::OnLayout====2====");

            //var lp = childView.GetLayoutParams();
            var lp = childView.GetLayoutParams().Probe("Elastos.Droid.View.IViewGroupMarginLayoutParams");

var oo = childView.GetObject();
var ss = oo.ToString();
elog("====childView.ToString:===="+ss);
var oo = lp.GetObject();
var ss = oo.ToString();
elog("====lp.ToString:===="+ss);

CObject.showMethods(lp);

            var _lp = {
                leftMargin:0,
                topMargin:0,
                rightMargin:0,
                bottomMargin:0,
            }

            var childTop = paddingTop + _lp.topMargin + rowIndex * rowHeight;
            elog(`====CalculatorPadLayout::OnLayout====2.1====childTop:${childTop}==paddingTop:${paddingTop}==lp.topMargin:${_lp.topMargin}==rowIndex:${rowIndex}==rowHeight:${rowHeight}==`);

            var childBottom = childTop - _lp.topMargin - _lp.bottomMargin + rowHeight;
            var childLeft = paddingLeft + _lp.leftMargin +
                    (isRTL ? (this.mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
            var childRight = childLeft - _lp.leftMargin - _lp.rightMargin + columnWidth;

            var childWidth = childRight - childLeft;
            var childHeight = childBottom - childTop;
            if (childWidth != childView.GetMeasuredWidth() ||
                    childHeight != childView.GetMeasuredHeight()) {
                elog("====CalculatorPadLayout::OnLayout====3====");
                childView.Measure(
                        MeasureSpec.MakeMeasureSpec(childWidth, MeasureSpec.GetEXACTLY()),
                        MeasureSpec.MakeMeasureSpec(childHeight, MeasureSpec.GetEXACTLY()));
                elog("====CalculatorPadLayout::OnLayout====4====childWidth:"+childWidth+"===="+"childHeight:"+childHeight);
            }
            elog(`====CalculatorPadLayout::OnLayout====5====childLeft:${childLeft}==childTop:${childTop}==childRight:${childRight}==childBottom:${childBottom}==`);
            childView.Layout(childLeft, childTop, childRight, childBottom);
            elog("====CalculatorPadLayout::OnLayout====6====");

            rowIndex = (rowIndex + (columnIndex + 1) / this.mColumnCount) % this.mRowCount;
            columnIndex = (columnIndex + 1) % this.mColumnCount;
        }
        elog("====CalculatorPadLayout::OnLayout====end=======");
    }

//     @Override
//     public LayoutParams generateLayoutParams(AttributeSet attrs) {
//         return new MarginLayoutParams(getContext(), attrs);
//     }
    GenerateLayoutParams(_this, attrs, result) {
        elog("====CalculatorPadLayout::GenerateLayoutParams====begin=======");

        //var c1 = attrs.GetAttributeName()
        //CObject.showMethods(c1);
        //elog("====CalculatorPadLayout::GenerateLayoutParams====attrs name:"+);

        var attrsClassName = attrs.getClass().GetName();

        elog("====attrs.name:"+attrsClassName);

        if (attrsClassName == "IAttributeSet") {
            result.data = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", _this.GetContext(), attrs);
            //result.data = _this._GenerateLayoutParams(attrs);

            // CObject.showMethods(result.data);

            // var ww = result.data.GetWidth();
            // var hh = result.data.GetHeight();
            // elog(`====CalculatorPadLayout::GenerateLayoutParams==ww:${ww}====hh${hh}====`);

            //ss.tt();Assert(0);
        }
        else {
            Assert(0);
            this.GenerateLayoutParams_0(_this, p, result);
        }

        elog("====CalculatorPadLayout::GenerateLayoutParams====end=======");
    }


//     @Override
//     protected LayoutParams generateDefaultLayoutParams() {
//         return new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
//     }
    GenerateDefaultLayoutParams(_this, result) {
        elog("====CalculatorPadLayout::GenerateDefaultLayoutParams====begin=======");
        result.data = new MarginLayoutParams(_this.LayoutParams.WRAP_CONTENT, _this.LayoutParams.WRAP_CONTENT);
        elog("====CalculatorPadLayout::GenerateDefaultLayoutParams====end=======");
    }

//     @Override
//     protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
//         return new MarginLayoutParams(p);
//     }
    GenerateLayoutParams_0(_this, p, result) {
        elog("====CalculatorPadLayout::GenerateLayoutParams_0====begin=======");
        result.data = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", p);
        elog("====CalculatorPadLayout::GenerateLayoutParams_0====end=======");
    }


//     @Override
//     protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
//         return p instanceof MarginLayoutParams;
//     }
    CheckLayoutParams(_this, p, result) {
        elog("====CalculatorPadLayout::CheckLayoutParams====begin=======");
        result.data = p instanceof MarginLayoutParams;
        elog("====CalculatorPadLayout::CheckLayoutParams====end=======");
    }

}   //class _CalculatorPadLayout

//--------.java----end----

return new CalculatorPadLayout();

};  //module.exports

elog("====CalculatorPadLayout.js====end====");