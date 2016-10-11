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
class _CalculatorPadLayout extends ViewGroup {
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
        elog("====CalculatorPadLayout.js====super::OnCreate====begin=======");

        attrs = attrs || null;
        defStyle = defStyle || 0;

        _this._constructor(context, attrs, defStyle);

        var Elastos__Droid__R__attr__rowCount = 0x01010375;
        var Elastos__Droid__R__attr__columnCount = 0x01010377;
        var a = context.ObtainStyledAttributes(attrs, [Elastos__Droid__R__attr__rowCount, Elastos__Droid__R__attr__columnCount], defStyle, 0);
        this.mRowCount = a.GetInt32(0, 1);
        this.mColumnCount = a.GetInt32(1, 1);

        a.Recycle();

        elog("====CalculatorPadLayout.js====super::OnCreate====end=======");
    }

//     @Override
//     public boolean shouldDelayChildPressedState() {
//         return false;
//     }
    ShouldDelayChildPressedState(result) {
        result.data = false;
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
    OnLayout(changed, left, top, right, bottom) {
        elog("====CalculatorPadLayout.js====super::OnLayout====begin=======");

        var paddingLeft = _this.GetPaddingLeft();
        var paddingRight = _this.GetPaddingRight();
        var paddingTop = _this.GetPaddingTop();
        var paddingBottom = _this.GetPaddingBottom();

        var isRTL = _this.GetLayoutDirection() == LAYOUT_DIRECTION_RTL;
        var columnWidth = Math.round(right - left - paddingLeft - paddingRight) / mColumnCount;
        var rowHeight = Math.round(bottom - top - paddingTop - paddingBottom) / mRowCount;

        var rowIndex = 0, columnIndex = 0;
        for (var childIndex = 0; childIndex < _this.GetChildCount(); ++childIndex) {
            var childView = _this.GetChildAt(childIndex);
            if (childView.GetVisibility() == View.GONE) {
                continue;
            }

            var lp = childView.GetLayoutParams();

            var childTop = paddingTop + lp.topMargin + rowIndex * rowHeight;
            var childBottom = childTop - lp.topMargin - lp.bottomMargin + rowHeight;
            childLeft = paddingLeft + lp.leftMargin +
                    (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
            var childRight = childLeft - lp.leftMargin - lp.rightMargin + columnWidth;

            var childWidth = childRight - childLeft;
            var childHeight = childBottom - childTop;
            if (childWidth != childView.GetMeasuredWidth() ||
                    childHeight != childView.GetMeasuredHeight()) {
                childView.Measure(
                        MeasureSpec.MakeMeasureSpec(childWidth, MeasureSpec.EXACTLY),
                        MeasureSpec.MakeMeasureSpec(childHeight, MeasureSpec.EXACTLY));
            }
            childView.Layout(childLeft, childTop, childRight, childBottom);

            rowIndex = (rowIndex + (columnIndex + 1) / mColumnCount) % mRowCount;
            columnIndex = (columnIndex + 1) % mColumnCount;
        }
        elog("====CalculatorPadLayout.js====super::OnLayout====end=======");
    }

//     @Override
//     public LayoutParams generateLayoutParams(AttributeSet attrs) {
//         return new MarginLayoutParams(getContext(), attrs);
//     }
    GenerateLayoutParams(_this, attrs, result) {
        elog("====CalculatorPadLayout.js====super::GenerateLayoutParams====begin=======");

        var attrsClassName = attrs.getClass().GetName();

        elog("====attrs.name:"+attrsClassName);

        if (attrsClassName == "IAttributeSet") {
            result.data = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", _this.GetContext(), attrs);
        }
        else {
            Assert(0);
            this.GenerateLayoutParams_0(_this, p, result);
        }

        elog("====CalculatorPadLayout.js====super::GenerateLayoutParams====end=======");
    }


//     @Override
//     protected LayoutParams generateDefaultLayoutParams() {
//         return new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
//     }
    GenerateDefaultLayoutParams(_this, result) {
        result.data = new MarginLayoutParams(_this.LayoutParams.WRAP_CONTENT, _this.LayoutParams.WRAP_CONTENT);
    }

//     @Override
//     protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
//         return new MarginLayoutParams(p);
//     }
    GenerateLayoutParams_0(_this, p, result) {
        elog("====CalculatorPadLayout.js====super::GenerateLayoutParams_0====begin=======");
        result.data = Droid_New("Elastos.Droid.View.CViewGroupMarginLayoutParams", p);
        elog("====CalculatorPadLayout.js====super::GenerateLayoutParams_0====end=======");
    }


//     @Override
//     protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
//         return p instanceof MarginLayoutParams;
//     }
    CheckLayoutParams(p, result) {
        result.data = p instanceof MarginLayoutParams;
    }

}   //class _CalculatorPadLayout

//--------.java----end----

    class CalculatorPadLayout extends _CalculatorPadLayout {
        constructor() {
            super();
        }

        // OnCreate(_this, context, attrs, defStyle) {
        //     elog("====CalculatorPadLayout.js====::OnCreate====begin=======");
        //     super.OnCreate(_this, context, attrs, defStyle);
        //     // _this._constructor(context, attrs, defStyle);
        //     elog("====CalculatorPadLayout.js====::OnCreate====end");
        // }

        // GenerateLayoutParams(_this, attrs, result){
        //     elog("====CalculatorPadLayout.js====::GenerateLayoutParams====begin");
        //     result.data = _this._GenerateLayoutParams(attrs);
        //     elog("====CalculatorPadLayout.js====::GenerateLayoutParams====end");
        // }

        GetChildDrawingOrder(childCount, i, result) {
            elog("====CalculatorPadLayout.js====::GetChildDrawingOrder====TODO");
            ssss;
        }

        // GenerateDefaultLayoutParams(_this, result) {
        //     elog("====CalculatorPadLayout.js====::GenerateDefaultLayoutParams====begin");
        //     result.data = _this._GenerateDefaultLayoutParams();
        //     elog("====CalculatorPadLayout.js====::GenerateDefaultLayoutParams====end");
        // }
    }   //class CalculatorPadLayout

    return new CalculatorPadLayout();

};  //module.exports

elog("====CalculatorPadLayout.js====end====");