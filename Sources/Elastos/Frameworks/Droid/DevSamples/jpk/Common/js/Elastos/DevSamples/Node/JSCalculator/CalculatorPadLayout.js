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
    var ViewGroup;  // = Elastos.Droid.View.IViewGroup;

// /**
//  * A layout that places children in an evenly distributed grid based on the specified
//  *  {@link android.R.attr#columnCount} and {@link android.R.attr#rowCount} attributes.
//  */
// public class CalculatorPadLayout extends ViewGroup {

//     private int mRowCount;
        var mRowCount;
//     private int mColumnCount;
        var mColumnCount;

//     public CalculatorPadLayout(Context context) {
//         this(context, null);
//     }

//     public CalculatorPadLayout(Context context, AttributeSet attrs) {
//         this(context, attrs, 0);
//     }

//     public CalculatorPadLayout(Context context, AttributeSet attrs, int defStyle) {
        function CalculatorPadLayout(context, attrs, defStyle) {
            attrs = attrs || null;
            defStyle = defStyle || 0;

//         super(context, attrs, defStyle);
            _super(context, attrs, defStyle);

//         final TypedArray a = context.obtainStyledAttributes(attrs,
//                 new int[] { android.R.attr.rowCount, android.R.attr.columnCount }, defStyle, 0);
            var a = context.ObtainStyledAttributes(attrs, [Elastos.Droid.R.attr.rowCount, Elastos.Droid.R.attr.columnCount], defStyle, 0);
//         mRowCount = a.getInt(0, 1);
            mRowCount = a.GetInt32(0, 1);
//         mColumnCount = a.getInt(1, 1);
            mColumnCount = a.GetInt32(1, 1);

//         a.recycle();
            a.Recycle();
//     }
        }
        var _pt = CalculatorPadLayout.prototype;

//     @Override
//     public boolean shouldDelayChildPressedState() {
        _pt.ShouldDelayChildPressedState = function(result) {
//         return false;
            result.data = false;
//     }
        }

//     @Override
//     protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        _pt.OnLayout = function(changed, left, top, right, bottom) {
//         final int paddingLeft = getPaddingLeft();
            var paddingLeft = _this.GetPaddingLeft();
//         final int paddingRight = getPaddingRight();
            var paddingRight = _this.GetPaddingRight();
//         final int paddingTop = getPaddingTop();
            var paddingTop = _this.GetPaddingTop();
//         final int paddingBottom = getPaddingBottom();
            var paddingBottom = _this.GetPaddingBottom();

//         final boolean isRTL = getLayoutDirection() == LAYOUT_DIRECTION_RTL;
            var isRTL = _this.GetLayoutDirection() == LAYOUT_DIRECTION_RTL;
//         final int columnWidth =
//                 Math.round((float) (right - left - paddingLeft - paddingRight)) / mColumnCount;
            var columnWidth = Math.round(right - left - paddingLeft - paddingRight) / mColumnCount;
//         final int rowHeight =
//                 Math.round((float) (bottom - top - paddingTop - paddingBottom)) / mRowCount;
            var rowHeight = Math.round(bottom - top - paddingTop - paddingBottom) / mRowCount;

//         int rowIndex = 0, columnIndex = 0;
            var rowIndex = 0, columnIndex = 0;
//         for (int childIndex = 0; childIndex < getChildCount(); ++childIndex) {
            for (var childIndex = 0; childIndex < _this.GetChildCount(); ++childIndex) {
//             final View childView = getChildAt(childIndex);
                var childView = _this.GetChildAt(childIndex);
//             if (childView.getVisibility() == View.GONE) {
                if (childView.GetVisibility() == View.GONE) {
//                 continue;
                    continue;
//             }
                }

//             final MarginLayoutParams lp = (MarginLayoutParams) childView.getLayoutParams();
                var lp = childView.GetLayoutParams();

//             final int childTop = paddingTop + lp.topMargin + rowIndex * rowHeight;
                var childTop = paddingTop + lp.topMargin + rowIndex * rowHeight;
//             final int childBottom = childTop - lp.topMargin - lp.bottomMargin + rowHeight;
                var childBottom = childTop - lp.topMargin - lp.bottomMargin + rowHeight;
//             final int childLeft = paddingLeft + lp.leftMargin +
//                     (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
                childLeft = paddingLeft + lp.leftMargin +
                        (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
//             final int childRight = childLeft - lp.leftMargin - lp.rightMargin + columnWidth;
                var childRight = childLeft - lp.leftMargin - lp.rightMargin + columnWidth;

//             final int childWidth = childRight - childLeft;
                var childWidth = childRight - childLeft;
//             final int childHeight = childBottom - childTop;
                var childHeight = childBottom - childTop;
//             if (childWidth != childView.getMeasuredWidth() ||
//                     childHeight != childView.getMeasuredHeight()) {
                if (childWidth != childView.GetMeasuredWidth() ||
                        childHeight != childView.GetMeasuredHeight()) {
//                 childView.measure(
//                         MeasureSpec.makeMeasureSpec(childWidth, MeasureSpec.EXACTLY),
//                         MeasureSpec.makeMeasureSpec(childHeight, MeasureSpec.EXACTLY));
                    childView.Measure(
                            MeasureSpec.MakeMeasureSpec(childWidth, MeasureSpec.EXACTLY),
                            MeasureSpec.MakeMeasureSpec(childHeight, MeasureSpec.EXACTLY));
//             }
                }
//             childView.layout(childLeft, childTop, childRight, childBottom);
                childView.Layout(childLeft, childTop, childRight, childBottom);

//             rowIndex = (rowIndex + (columnIndex + 1) / mColumnCount) % mRowCount;
                rowIndex = (rowIndex + (columnIndex + 1) / mColumnCount) % mRowCount;
//             columnIndex = (columnIndex + 1) % mColumnCount;
                columnIndex = (columnIndex + 1) % mColumnCount;
//         }
            }
//     }
        }

//     @Override
//     public LayoutParams generateLayoutParams(AttributeSet attrs) {
        _pt.GenerateLayoutParams = function(attrs, result) {
//         return new MarginLayoutParams(getContext(), attrs);
            result.data = new MarginLayoutParams(_this.GetContext(), attrs);
//     }
        }

//     @Override
//     protected LayoutParams generateDefaultLayoutParams() {
        _pt.GenerateDefaultLayoutParams = function(result) {
//         return new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            result.data = new MarginLayoutParams(_this.LayoutParams.WRAP_CONTENT, _this.LayoutParams.WRAP_CONTENT);
//     }
        }

//     @Override
//     protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
        _pt.GenerateLayoutParams = function(p, result) {
//         return new MarginLayoutParams(p);
            result.data = new MarginLayoutParams(p);
//     }
        }

//     @Override
//     protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
        _pt.CheckLayoutParams = function(p, result) {
//         return p instanceof MarginLayoutParams;
            result.data = p instanceof MarginLayoutParams;
//     }
        }
// }

//--------.java----end----

    return CalculatorPadLayout;
};  //module.exports

elog("====CalculatorPadLayout.js====end====");
