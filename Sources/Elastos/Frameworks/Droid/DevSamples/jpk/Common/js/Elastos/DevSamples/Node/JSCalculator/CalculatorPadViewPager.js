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
// import android.support.v4.view.PagerAdapter;
    var PagerAdapter = require("./support/v4/view/PagerAdapter.js");
// import android.support.v4.view.ViewPager;
    var ViewPager = require("./support/v4/view/ViewPager.js");
// import android.util.AttributeSet;
// import android.view.View;
// import android.view.ViewGroup;

// public class CalculatorPadViewPager extends ViewPager {
    var _this;

//     private final PagerAdapter mStaticPagerAdapter = new PagerAdapter() {
        var mStaticPagerAdapter = new PagerAdapter().extend({
//         @Override
//         public int getCount() {
            getCount : function(result) {
//             return getChildCount();
                result.data = _this.getChildCount();
//         }
            },

//         @Override
//         public Object instantiateItem(ViewGroup container, int position) {
            instantiateItem : function(container, position, result) {
//             return getChildAt(position);
                result.data = getChildAt(position);
//         }
            },

//         @Override
//         public void destroyItem(ViewGroup container, int position, Object object) {
            destroyItem : function(container, position, object) {
//             removeViewAt(position);
                _this.removeViewAt(position);
//         }
            },

//         @Override
//         public boolean isViewFromObject(View view, Object object) {
            isViewFromObject : function(view, object, result) {
//             return view == object;
                result.data = (view == object);
//         }
            },

//         @Override
//         public float getPageWidth(int position) {
            getPageWidth : function(position, result) {
//             return position == 1 ? 7.0f / 9.0f : 1.0f;
                result.data = position == 1 ? 7.0 / 9.0 : 1.0;
//         }
            },
//     };
        });

//     private final OnPageChangeListener mOnPageChangeListener = new SimpleOnPageChangeListener() {
        var mOnPageChangeListener = {
//         private void recursivelySetEnabled(View view, boolean enabled) {
            recursivelySetEnabled : function(view, enabled) {
//             if (view instanceof ViewGroup) {
//                 final ViewGroup viewGroup = (ViewGroup) view;
//                 for (int childIndex = 0; childIndex < viewGroup.getChildCount(); ++childIndex) {
//                     recursivelySetEnabled(viewGroup.getChildAt(childIndex), enabled);
//                 }
//             } else {
//                 view.setEnabled(enabled);
//             }
//         }
            },

//         @Override
//         public void onPageSelected(int position) {
            onPageSelected : function(position) {
//             if (getAdapter() == mStaticPagerAdapter) {
//                 for (int childIndex = 0; childIndex < getChildCount(); ++childIndex) {
//                     // Only enable subviews of the current page.
//                     recursivelySetEnabled(getChildAt(childIndex), childIndex == position);
//                 }
//             }
//         }
            },
//     };
        };

//     private final PageTransformer mPageTransformer = new PageTransformer() {
        var mPageTransformer = {
//         @Override
//         public void transformPage(View view, float position) {
            transformPage : function(view, position) {
//             if (position < 0.0f) {
//                 // Pin the left page to the left side.
//                 view.setTranslationX(getWidth() * -position);
//                 view.setAlpha(Math.max(1.0f + position, 0.0f));
//             } else {
//                 // Use the default slide transition when moving to the next page.
//                 view.setTranslationX(0.0f);
//                 view.setAlpha(1.0f);
//             }
//         }
            },
//     };
        };

//     public CalculatorPadViewPager(Context context) {
//         this(context, null);
//     }

//     public CalculatorPadViewPager(Context context, AttributeSet attrs) {
        function CalculatorPadViewPager(_this, context, attrs) {
            attrs = attrs || null;
//         super(context, attrs);

//         setAdapter(mStaticPagerAdapter);
//         setBackgroundColor(getResources().getColor(android.R.color.black));
//         setOnPageChangeListener(mOnPageChangeListener);
//         setPageMargin(getResources().getDimensionPixelSize(R.dimen.pad_page_margin));
//         setPageTransformer(false, mPageTransformer);
//     }
        }
        var _pt = CalculatorPadViewPager.prototype;

//     @Override
//     protected void onFinishInflate() {
        _pt.OnFinishInflate = function() {
//         super.onFinishInflate();
            _super.OnFinishInflate();

//         // Invalidate the adapter's data set since children may have been added during inflation.
//         if (getAdapter() == mStaticPagerAdapter) {
            if (_this.GetAdapter() == mStaticPagerAdapter) {
//             mStaticPagerAdapter.notifyDataSetChanged();
                mStaticPagerAdapter.notifyDataSetChanged();
//         }
            }
//     }
        }
// }

//--------.java----end----

    return CalculatorPadViewPager;
};  //module.exports