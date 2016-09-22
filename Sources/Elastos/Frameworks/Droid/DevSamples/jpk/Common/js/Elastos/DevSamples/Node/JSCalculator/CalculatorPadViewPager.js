elog("====CalculatorPadViewPager.js====begin====");

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
    var PagerAdapter = require("./support/v4/view/PagerAdapter.js")(aoElastos, aoActivity);
// import android.support.v4.view.ViewPager;
    var ViewPager = require("./support/v4/view/ViewPager.js")(aoElastos, aoActivity);
// import android.util.AttributeSet;
// import android.view.View;
// import android.view.ViewGroup;

// public class CalculatorPadViewPager extends ViewPager {
    function JSClass(host) {
        this.mHost = host;
    }
    var _pt = JSClass.prototype;

//     private final PagerAdapter mStaticPagerAdapter = new PagerAdapter() {
        var mStaticPagerAdapter = {
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
        };

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
        _pt.OnCreate = function(_this, context, attrs) {
            elog("====CalculatorPadViewPager====OnCreate====begin====");

            attrs = attrs || null;
//         super(context, attrs);

//         setAdapter(mStaticPagerAdapter);
//         setBackgroundColor(getResources().getColor(android.R.color.black));
//         setOnPageChangeListener(mOnPageChangeListener);
//         setPageMargin(getResources().getDimensionPixelSize(R.dimen.pad_page_margin));
//         setPageTransformer(false, mPageTransformer);
//     }

            elog("====CalculatorPadViewPager====OnCreate====end====");
        }

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


    //_super = aoCustomControl;
    //var _super = aoActivity;
    var Super = function(aoSuper) {
        this._super = aoSuper;
    }
    var _spt = Super.prototype;
    _spt.OnCreate = function(_this, context, attrs){
        this._super._constructor(_this, context, attrs);
    }
    // _spt.OnTouchEvent = function(event, result) {
    //         this._super._OnTouchEvent(event, result);
    // }
    // _spt.OnSaveInstanceState = function(result) {
    //         this._super._OnTouchEvent(result);
    // }
    // _spt.SetTextSize = function(unit, size) {
    //         this._super._SetTextSize(unit, size);
    // }
    // _spt.GetCompoundPaddingTop = function(result) {
    //         this._super._GetCompoundPaddingTop(result);
    // }
    // _spt.GetCompoundPaddingBottom = function(result) {
    //         this._super._GetCompoundPaddingBottom(result);
    // }
    // _spt.OnMeasure = function(widthMeasureSpec, heightMeasureSpec) {
    //         this._super._OnMeasure(widthMeasureSpec, heightMeasureSpec);
    // }
    // _spt.OnTextChanged = function(text, start, lengthBefore, lengthAfter) {
    //         this._super._OnTextChanged(text, start, lengthBefore, lengthAfter);
    // }

    var _super;

    var oThis = new JSClass(aoActivity);

    var oCallback = {
        OnCreate : function(_this, context, attrs) {
            _super = new Super(_this);
            oThis.OnCreate(_this, context, attrs);
        },
        // OnTouchEvent : function(_this, event, result) {
        //     oThis.OnTouchEvent(_this, event, result);
        // },
        // OnSaveInstanceState : function(_this, result) {
        //     oThis.OnSaveInstanceState(_this, result);
        // },
        // SetTextSize : function(_this, unit, size) {
        //     oThis.SetTextSize(_this, unit, size);
        // },
        // GetCompoundPaddingTop : function(_this, result) {
        //     oThis.GetCompoundPaddingTop(_this, result);
        // },
        // GetCompoundPaddingBottom : function(_this, result) {
        //     oThis.GetCompoundPaddingBottom(_this, result);
        // },
        // OnMeasure : function(_this, widthMeasureSpec, heightMeasureSpec) {
        //     oThis.OnMeasure(_this, widthMeasureSpec, heightMeasureSpec);
        // },
        // OnTextChanged : function(_this, text, start, lengthBefore, lengthAfter) {
        //     oThis.OnTextChanged(_this, text, start, lengthBefore, lengthAfter);
        // },
    }

    return oCallback;
};  //module.exports

elog("====CalculatorPadViewPager.js====end====");