elog("====Calculator.js====begin====");

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
    var IView__INVISIBLE = 0x00000004;
    var IView__GONE = 0x00000008;
    var IView__NO_ID = -1;
    var IMotionEvent__ACTION_DOWN = 0;
    var IMotionEvent__ACTION_UP = 1;

//--------common definition----end----


//================SetContentView Test====begin================

var STC = require("/data/temp/node/Common/js/elastos_layout.js")(aoElastos, aoActivity);
var ST = new STC();

//================SetContentView Test====begin================


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

// import android.animation.Animator;
// import android.animation.Animator.AnimatorListener;
// import android.animation.AnimatorListenerAdapter;
// import android.animation.AnimatorSet;
// import android.animation.ArgbEvaluator;
// import android.animation.ObjectAnimator;
// import android.animation.ValueAnimator;
// import android.animation.ValueAnimator.AnimatorUpdateListener;
// import android.app.Activity;
class Activity {
    constructor() {
        //TODO return wrapper of native hacked activity compoment
        //TODO to replace this._obj
    }

    OnCreate(_this, savedInstanceState) {}
}
// import android.graphics.Rect;
// import android.os.Bundle;
// import android.support.annotation.NonNull;
// import android.support.v4.view.ViewPager;
// import android.text.Editable;
// import android.text.TextUtils;
// import android.text.TextWatcher;
class TextWatcher {
    // constructor(text) {
    //     var _obj = Droid_New("Elastos.Droid.Text.ITextWatcher");
    //     return _obj;
    // }
}

// import android.view.KeyEvent;
// import android.view.View;
// import android.view.View.OnKeyListener;
class OnKeyListener {}

// import android.view.View.OnLongClickListener;
// import android.view.ViewAnimationUtils;
// import android.view.ViewGroupOverlay;
// import android.view.animation.AccelerateDecelerateInterpolator;
// import android.widget.Button;
// import android.widget.TextView;

// import com.android.calculator2.CalculatorEditText.OnTextSizeChangeListener;
// import com.android.calculator2.CalculatorExpressionEvaluator.EvaluateCallback;

var CalculatorExpressionTokenizer = require("./CalculatorExpressionTokenizer.js")(aoElastos, aoActivity);
var CalculatorExpressionEvaluator = require("./CalculatorExpressionEvaluator.js")(aoElastos, aoActivity);
var CalculatorExpressionBuilder = require("./CalculatorExpressionBuilder.js")(aoElastos, aoActivity);

// public class Calculator extends Activity
//         implements OnTextSizeChangeListener, EvaluateCallback, OnLongClickListener {
class Calculator extends Activity {

//     private static final String NAME = Calculator.class.getName();
    static get NAME() {
        //return Calculator.class.getName();
        elog("====Calculator::name===="+Calculator.name);
        return Calculator.name;
    }

//     // instance state keys
//     private static final String KEY_CURRENT_STATE = NAME + "_currentState";
    static get KEY_CURRENT_STATE() {
        return Calculator.NAME + "_currentState";
    }
//     private static final String KEY_CURRENT_EXPRESSION = NAME + "_currentExpression";
    static get KEY_CURRENT_EXPRESSION() {
        return Calculator.NAME + "_currentExpression";
    }

//     /**
//      * Constant for an invalid resource id.
//      */
//     public static final int INVALID_RES_ID = -1;
    static get INVALID_RES_ID() {return -1;}

//     private enum CalculatorState {
//         INPUT, EVALUATE, RESULT, ERROR
//     }
    static get CalculatorState() {
        return {
            INPUT:0, EVALUATE:1, RESULT:2, ERROR:3,
            values:function(){return [0,1,2,3];}
        }
    }

//     private final TextWatcher mFormulaTextWatcher = new TextWatcher() {
//         @Override
//         public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) {
//         }

//         @Override
//         public void onTextChanged(CharSequence charSequence, int start, int count, int after) {
//         }

//         @Override
//         public void afterTextChanged(Editable editable) {
//             setState(CalculatorState.INPUT);
//             mEvaluator.evaluate(editable, Calculator.this);
//         }
//     };
    get mFormulaTextWatcher() {
        if (this._mFormulaTextWatcher) return this._mFormulaTextWatcher;
        this._mFormulaTextWatcher = new class _ extends TextWatcher {
            //@Override
            BeforeTextChanged(charSequence, start, count, after) {
            }

            //@Override
            OnTextChanged(charSequence, start, count, after) {
            }

            //@Override
            AfterTextChanged(editable) {
                this.SetState(Calculator.CalculatorState.INPUT);
                this.mEvaluator.evaluate(editable, Calculator.this);
            }
        }();
        return this._mFormulaTextWatcher;
    };

//     private final OnKeyListener mFormulaOnKeyListener = new OnKeyListener() {
//         @Override
//         public boolean onKey(View view, int keyCode, KeyEvent keyEvent) {
//             switch (keyCode) {
//                 case KeyEvent.KEYCODE_NUMPAD_ENTER:
//                 case KeyEvent.KEYCODE_ENTER:
//                     if (keyEvent.getAction() == KeyEvent.ACTION_UP) {
//                         mCurrentButton = mEqualButton;
//                         onEquals();
//                     }
//                     // ignore all other actions
//                     return true;
//             }
//             return false;
//         }
//     };
    get mFormulaOnKeyListener() {
        if (this._mFormulaOnKeyListener) return this._mFormulaOnKeyListener;
        this._mFormulaOnKeyListener = new class _ extends OnKeyListener {
            //@Override
            OnKey(view, keyCode, keyEvent) {
                switch (keyCode) {
                    case KeyEvent.KEYCODE_NUMPAD_ENTER:
                    case KeyEvent.KEYCODE_ENTER:
                        if (keyEvent.getAction() == KeyEvent.ACTION_UP) {
                            mCurrentButton = mEqualButton;
                            onEquals();
                        }
                        // ignore all other actions
                        return true;
                }
                return false;
            }
        }();
        return this._mFormulaOnKeyListener;
    };

//     private final Editable.Factory mFormulaEditableFactory = new Editable.Factory() {
//         @Override
//         public Editable newEditable(CharSequence source) {
//             final boolean isEdited = mCurrentState == CalculatorState.INPUT
//                     || mCurrentState == CalculatorState.ERROR;
//             return new CalculatorExpressionBuilder(source, mTokenizer, isEdited);
//         }
//     };
    get mFormulaEditableFactory() {
        if (this._mFormulaEditableFactory) return this._mFormulaEditableFactory;
        //this._mFormulaEditableFactory = new class _ extends Editable.Factory {
        this._mFormulaEditableFactory = new class Editable__Factory {
            // @Override
            NewEditable(source, result) {
                elog("====ViewPager::mFormulaEditableFactory::NewEditable====begin====");
                var isEdited = this.mCurrentState == Calculator.CalculatorState.INPUT
                        || this.mCurrentState == Calculator.CalculatorState.ERROR;
                //return new CalculatorExpressionBuilder(source, this.mTokenizer, isEdited);
                result.data = new CalculatorExpressionBuilder(source, this.mTokenizer, isEdited);
                elog("====ViewPager::mFormulaEditableFactory::NewEditable====end====");
            }
        }();
        return this._mFormulaEditableFactory;
    };

//     private CalculatorState mCurrentState;
//     private CalculatorExpressionTokenizer mTokenizer;
//     private CalculatorExpressionEvaluator mEvaluator;

//     private View mDisplayView;
//     private CalculatorEditText mFormulaEditText;
//     private CalculatorEditText mResultEditText;
//     private ViewPager mPadViewPager;
//     private View mDeleteButton;
//     private View mEqualButton;
//     private View mClearButton;

//     private View mCurrentButton;
//     private Animator mCurrentAnimator;

//     @Override
//     protected void onCreate(Bundle savedInstanceState) {
//         super.onCreate(savedInstanceState);
//         setContentView(R.layout.activity_calculator);

//         mDisplayView = findViewById(R.id.display);
//         mFormulaEditText = (CalculatorEditText) findViewById(R.id.formula);
//         mResultEditText = (CalculatorEditText) findViewById(R.id.result);
//         mPadViewPager = (ViewPager) findViewById(R.id.pad_pager);
//         mDeleteButton = findViewById(R.id.del);
//         mClearButton = findViewById(R.id.clr);

//         mEqualButton = findViewById(R.id.pad_numeric).findViewById(R.id.eq);
//         if (mEqualButton == null || mEqualButton.getVisibility() != View.VISIBLE) {
//             mEqualButton = findViewById(R.id.pad_operator).findViewById(R.id.eq);
//         }

//         mTokenizer = new CalculatorExpressionTokenizer(this);
//         mEvaluator = new CalculatorExpressionEvaluator(mTokenizer);

//         savedInstanceState = savedInstanceState == null ? Bundle.EMPTY : savedInstanceState;
//         setState(CalculatorState.values()[
//                 savedInstanceState.getInt(KEY_CURRENT_STATE, CalculatorState.INPUT.ordinal())]);
//         mFormulaEditText.setText(mTokenizer.getLocalizedExpression(
//                 savedInstanceState.getString(KEY_CURRENT_EXPRESSION, "")));
//         mEvaluator.evaluate(mFormulaEditText.getText(), this);

//         mFormulaEditText.setEditableFactory(mFormulaEditableFactory);
//         mFormulaEditText.addTextChangedListener(mFormulaTextWatcher);
//         mFormulaEditText.setOnKeyListener(mFormulaOnKeyListener);
//         mFormulaEditText.setOnTextSizeChangeListener(this);
//         mDeleteButton.setOnLongClickListener(this);
//     }
    OnCreate(_this, savedInstanceState) {
        this._obj = _this;
elog("====Calculator::OnCreate====0====");

        super.OnCreate(_this, savedInstanceState);
        //_this.SetContentView(R.layout.activity_calculator);

elog("====Calculator::OnCreate====0.1====");

        ST.SetContentView(_this, R.layout.activity_calculator);

elog("====Calculator::OnCreate====0.2====");

//return;

        this.mDisplayView = _this.FindViewById(R.id.display);
        this.mFormulaEditText = _this.FindViewById(R.id.formula);
        this.mResultEditText = _this.FindViewById(R.id.result);
        this.mPadViewPager = _this.FindViewById(R.id.pad_pager);
        this.mDeleteButton = _this.FindViewById(R.id.del);
        this.mClearButton = _this.FindViewById(R.id.clr);

        this.mEqualButton = _this.FindViewById(R.id.pad_numeric).FindViewById(R.id.eq);
        if (this.mEqualButton == null || this.mEqualButton.GetVisibility() != IView__VISIBLE) {
            this.mEqualButton = _this.FindViewById(R.id.pad_operator).FindViewById(R.id.eq);
        }
elog("====Calculator::OnCreate====1====");

//----test begin----
var w1 = this.mPadViewPager.GetWidth();
var h1 = this.mPadViewPager.GetHeight();
var w2 = this.mFormulaEditText.GetWidth();
var h2 = this.mFormulaEditText.GetHeight();
elog("====Calculator::OnCreate====1.1===="+w1+"==="+h1);
elog("====Calculator::OnCreate====1.2===="+w2+"==="+h2);
//ss.tt();Assert(0);

//----test end----


        this.mTokenizer = new CalculatorExpressionTokenizer(_this);
elog("====Calculator::OnCreate====2====");
        this.mEvaluator = new CalculatorExpressionEvaluator(this.mTokenizer);
elog("====Calculator::OnCreate====3====");

// static AutoPtr<IBundle> EMPTY;
// static AutoPtr<IBundle> InitEMPTY()
// {
//     AutoPtr<CBundle> empty;
//     CBundle::NewByFriend((CBundle**)&empty);
//     empty->mMap = CArrayMap::EMPTY;
//     return empty;
// }
// AutoPtr<IBundle> CBundle::EMPTY = InitEMPTY();

    // AutoPtr<IBundleHelper> bhl;
    // CBundleHelper::AcquireSingleton((IBundleHelper**)&bhl);
    // AutoPtr<IBundle> empty;
    // bhl->GetEMPTY((IBundle**)&empty);

    var Bundle__EMPTY = Droid_New("Elastos.Droid.Os.CBundleHelper").GetEMPTY();

        savedInstanceState = savedInstanceState == null ? Bundle__EMPTY : savedInstanceState;

//CObject.showMethods(savedInstanceState);
//elog("====interface/class name:"+savedInstanceState.getClass().GetName());
//var ss = CObject.getObjectMethodProtos(savedInstanceState,"GetInt32")
//elog("====PROTO:"+ss);

elog("====Calculator::OnCreate====4====state:"+Calculator.KEY_CURRENT_STATE+"===="+Calculator.CalculatorState.INPUT);

        this.SetState(Calculator.CalculatorState.values()[
                savedInstanceState.GetInt32(Calculator.KEY_CURRENT_STATE, Calculator.CalculatorState.INPUT)]);
elog("====Calculator::OnCreate====5====");

// var s0 = Calculator.KEY_CURRENT_EXPRESSION;
// elog("====Calculator::OnCreate====5.0===="+s0);
// var s1 = savedInstanceState.GetString(s0, "");
// elog("====Calculator::OnCreate====5.1====["+s1+"]");
// var s2 = this.mTokenizer.GetLocalizedExpression(s1);
// elog("====Calculator::OnCreate====5.2===="+s2);

// CObject.showMethods(this.mFormulaEditText);

        //this.mFormulaEditText.SetText(this.mTokenizer.GetLocalizedExpression(
        //        savedInstanceState.GetString(Calculator.KEY_CURRENT_EXPRESSION, "")));
        //this.mFormulaEditText.SetText(CString(this.mTokenizer.GetLocalizedExpression(
        //        savedInstanceState.GetString(Calculator.KEY_CURRENT_EXPRESSION, ""))));
        this.mFormulaEditText.SetText(CString("先定个能达到的小目标，比方说我先挣它:"));

elog("====Calculator::OnCreate====6====");
        this.mEvaluator.Evaluate(this.mFormulaEditText.GetText(), this);
elog("====Calculator::OnCreate====7====");

        this.mFormulaEditText.SetEditableFactory(this.mFormulaEditableFactory);
elog("====Calculator::OnCreate====8====");
        this.mFormulaEditText.AddTextChangedListener(this.mFormulaTextWatcher);
elog("====Calculator::OnCreate====9====");
        this.mFormulaEditText.SetOnKeyListener(this.mFormulaOnKeyListener);
elog("====Calculator::OnCreate====10====");

CObject.showMethods(this.mFormulaEditText, "Listener");

        //this.mFormulaEditText.SetOnTextSizeChangeListener(_this);
elog("====Calculator::OnCreate====11====");
        this.mDeleteButton.SetOnLongClickListener(this);

    // textView->SetEditableFactory(mFormulaEditableFactory);
    // textView->AddTextChangedListener(mFormulaTextWatcher);
    // IView::Probe(mFormulaEditText)->SetOnKeyListener(IViewOnKeyListener::Probe(mFormulaOnKeyListener));
    // mFormulaEditText->SetOnTextSizeChangeListener(IOnTextSizeChangeListener::Probe(this));
    // return mDeleteButton->SetOnLongClickListener(IViewOnLongClickListener::Probe(this));



        elog("====Calculator::OnCreate====end====");
    }

//     @Override
//     protected void onSaveInstanceState(@NonNull Bundle outState) {
//         // If there's an animation in progress, cancel it first to ensure our state is up-to-date.
//         if (mCurrentAnimator != null) {
//             mCurrentAnimator.cancel();
//         }

//         super.onSaveInstanceState(outState);

//         outState.putInt(KEY_CURRENT_STATE, mCurrentState.ordinal());
//         outState.putString(KEY_CURRENT_EXPRESSION,
//                 mTokenizer.getNormalizedExpression(mFormulaEditText.getText().toString()));
//     }
    OnSaveInstanceState(_this, outState) {
        // If there's an animation in progress, cancel it first to ensure our state is up-to-date.
        if (this.mCurrentAnimator != null) {
            this.mCurrentAnimator.cancel();
        }

        super.onSaveInstanceState(outState);

        outState.putInt(Calculator.KEY_CURRENT_STATE, this.mCurrentState.ordinal());
        outState.putString(Calculator.KEY_CURRENT_EXPRESSION,
                mTokenizer.getNormalizedExpression(mFormulaEditText.getText().toString()));
    }

//     private void setState(CalculatorState state) {
//         if (mCurrentState != state) {
//             mCurrentState = state;

//             if (state == CalculatorState.RESULT || state == CalculatorState.ERROR) {
//                 mDeleteButton.setVisibility(View.GONE);
//                 mClearButton.setVisibility(View.VISIBLE);
//             } else {
//                 mDeleteButton.setVisibility(View.VISIBLE);
//                 mClearButton.setVisibility(View.GONE);
//             }

//             if (state == CalculatorState.ERROR) {
//                 final int errorColor = getResources().getColor(R.color.calculator_error_color);
//                 mFormulaEditText.setTextColor(errorColor);
//                 mResultEditText.setTextColor(errorColor);
//                 getWindow().setStatusBarColor(errorColor);
//             } else {
//                 mFormulaEditText.setTextColor(
//                         getResources().getColor(R.color.display_formula_text_color));
//                 mResultEditText.setTextColor(
//                         getResources().getColor(R.color.display_result_text_color));
//                 getWindow().setStatusBarColor(
//                         getResources().getColor(R.color.calculator_accent_color));
//             }
//         }
//     }
    SetState(state) {
        elog("====Calculator::SetState====begin===="+this.mCurrentState+"===="+state);

        var _this = this._obj;

        if (this.mCurrentState != state) {
            this.mCurrentState = state;

            if (state == Calculator.CalculatorState.RESULT || state == Calculator.CalculatorState.ERROR) {
                this.mDeleteButton.SetVisibility(IView__GONE);
                this.mClearButton.SetVisibility(IView__VISIBLE);
            } else {
                this.mDeleteButton.SetVisibility(IView__VISIBLE);
                this.mClearButton.SetVisibility(IView__GONE);
            }

            if (state == Calculator.CalculatorState.ERROR) {
                var errorColor = _this.GetResources().GetColor(R.color.calculator_error_color);
                this.mFormulaEditText.SetTextColor(errorColor);
                this.mResultEditText.SetTextColor(errorColor);
                _this.GetWindow().setStatusBarColor(errorColor);
            } else {
                this.mFormulaEditText.SetTextColor(
                        _this.GetResources().GetColor(R.color.display_formula_text_color));
                this.mResultEditText.SetTextColor(
                        _this.GetResources().GetColor(R.color.display_result_text_color));
                _this.GetWindow().SetStatusBarColor(
                        _this.GetResources().GetColor(R.color.calculator_accent_color));
            }
        }
        else {
            elog("====Calculator::SetState====no act====");
        }
        elog("====Calculator::SetState====end====");
    }

//     @Override
//     public void onBackPressed() {
//         if (mPadViewPager == null || mPadViewPager.getCurrentItem() == 0) {
//             // If the user is currently looking at the first pad (or the pad is not paged),
//             // allow the system to handle the Back button.
//             super.onBackPressed();
//         } else {
//             // Otherwise, select the previous pad.
//             mPadViewPager.setCurrentItem(mPadViewPager.getCurrentItem() - 1);
//         }
//     }
    OnBackPressed(_this) {
        if (mPadViewPager == null || mPadViewPager.getCurrentItem() == 0) {
            // If the user is currently looking at the first pad (or the pad is not paged),
            // allow the system to handle the Back button.
            super.onBackPressed();
        } else {
            // Otherwise, select the previous pad.
            mPadViewPager.setCurrentItem(mPadViewPager.getCurrentItem() - 1);
        }
    }

//     @Override
//     public void onUserInteraction() {
//         super.onUserInteraction();

//         // If there's an animation in progress, cancel it so the user interaction can be handled
//         // immediately.
//         if (mCurrentAnimator != null) {
//             mCurrentAnimator.cancel();
//         }
//     }
    OnUserInteraction(_this) {
        _this._OnUserInteraction();

        // If there's an animation in progress, cancel it so the user interaction can be handled
        // immediately.
        if (this.mCurrentAnimator != null) {
            this.mCurrentAnimator.Cancel();
        }
    }

//     public void onButtonClick(View view) {
//         mCurrentButton = view;

//         switch (view.getId()) {
//             case R.id.eq:
//                 onEquals();
//                 break;
//             case R.id.del:
//                 onDelete();
//                 break;
//             case R.id.clr:
//                 onClear();
//                 break;
//             case R.id.fun_cos:
//             case R.id.fun_ln:
//             case R.id.fun_log:
//             case R.id.fun_sin:
//             case R.id.fun_tan:
//                 // Add left parenthesis after functions.
//                 mFormulaEditText.append(((Button) view).getText() + "(");
//                 break;
//             default:
//                 mFormulaEditText.append(((Button) view).getText());
//                 break;
//         }
//     }
    OnButtonClick(view) {
        mCurrentButton = view;

        switch (view.getId()) {
            case R.id.eq:
                onEquals();
                break;
            case R.id.del:
                onDelete();
                break;
            case R.id.clr:
                onClear();
                break;
            case R.id.fun_cos:
            case R.id.fun_ln:
            case R.id.fun_log:
            case R.id.fun_sin:
            case R.id.fun_tan:
                // Add left parenthesis after functions.
                mFormulaEditText.append(view.GetText() + "(");
                break;
            default:
                mFormulaEditText.append(view.GetText());
                break;
        }
    }

//     @Override
//     public boolean onLongClick(View view) {
//         mCurrentButton = view;

//         if (view.getId() == R.id.del) {
//             onClear();
//             return true;
//         }
//         return false;
//     }
    OnLongClick(_this, view) {
        mCurrentButton = view;

        if (view.getId() == R.id.del) {
            onClear();
            return true;
        }
        return false;
    }

//     @Override
//     public void onEvaluate(String expr, String result, int errorResourceId) {
//         if (mCurrentState == CalculatorState.INPUT) {
//             mResultEditText.setText(result);
//         } else if (errorResourceId != INVALID_RES_ID) {
//             onError(errorResourceId);
//         } else if (!TextUtils.isEmpty(result)) {
//             onResult(result);
//         } else if (mCurrentState == CalculatorState.EVALUATE) {
//             // The current expression cannot be evaluated -> return to the input state.
//             setState(CalculatorState.INPUT);
//         }

//         mFormulaEditText.requestFocus();
//     }
    OnEvaluate(_this, expr, result, errorResourceId) {
        if (this.mCurrentState == Calculator.CalculatorState.INPUT) {
            //this.mResultEditText.SetText(result);
            this.mResultEditText.SetText(CString("$:1,00,000,000.00"));
        } else if (errorResourceId != Calculator.INVALID_RES_ID) {
            onError(errorResourceId);
        } else if (!TextUtils.isEmpty(result)) {
            onResult(result);
        } else if (this.mCurrentState == Calculator.CalculatorState.EVALUATE) {
            // The current expression cannot be evaluated -> return to the input state.
            setState(Calculator.CalculatorState.INPUT);
        }

        this.mFormulaEditText.RequestFocus();
    }

//     @Override
//     public void onTextSizeChanged(final TextView textView, float oldSize) {
//         if (mCurrentState != CalculatorState.INPUT) {
//             // Only animate text changes that occur from user input.
//             return;
//         }

//         // Calculate the values needed to perform the scale and translation animations,
//         // maintaining the same apparent baseline for the displayed text.
//         final float textScale = oldSize / textView.getTextSize();
//         final float translationX = (1.0f - textScale) *
//                 (textView.getWidth() / 2.0f - textView.getPaddingEnd());
//         final float translationY = (1.0f - textScale) *
//                 (textView.getHeight() / 2.0f - textView.getPaddingBottom());

//         final AnimatorSet animatorSet = new AnimatorSet();
//         animatorSet.playTogether(
//                 ObjectAnimator.ofFloat(textView, View.SCALE_X, textScale, 1.0f),
//                 ObjectAnimator.ofFloat(textView, View.SCALE_Y, textScale, 1.0f),
//                 ObjectAnimator.ofFloat(textView, View.TRANSLATION_X, translationX, 0.0f),
//                 ObjectAnimator.ofFloat(textView, View.TRANSLATION_Y, translationY, 0.0f));
//         animatorSet.setDuration(getResources().getInteger(android.R.integer.config_mediumAnimTime));
//         animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
//         animatorSet.start();
//     }
    OnTextSizeChanged(_this, textView, oldSize) {
        if (this.mCurrentState != Calculator.CalculatorState.INPUT) {
            // Only animate text changes that occur from user input.
            return;
        }

        // Calculate the values needed to perform the scale and translation animations,
        // maintaining the same apparent baseline for the displayed text.
        var textScale = oldSize / textView.getTextSize();
        var translationX = (1.0 - textScale) *
                (textView.getWidth() / 2.0 - textView.getPaddingEnd());
        var translationY = (1.0 - textScale) *
                (textView.getHeight() / 2.0 - textView.getPaddingBottom());

        var animatorSet = new AnimatorSet();
        animatorSet.playTogether(
                ObjectAnimator.ofFloat(textView, View.SCALE_X, textScale, 1.0),
                ObjectAnimator.ofFloat(textView, View.SCALE_Y, textScale, 1.0),
                ObjectAnimator.ofFloat(textView, View.TRANSLATION_X, translationX, 0.0),
                ObjectAnimator.ofFloat(textView, View.TRANSLATION_Y, translationY, 0.0));
        animatorSet.setDuration(getResources().getInteger(android.R.integer.config_mediumAnimTime));
        animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
        animatorSet.start();
    }

//     private void onEquals() {
//         if (mCurrentState == CalculatorState.INPUT) {
//             setState(CalculatorState.EVALUATE);
//             mEvaluator.evaluate(mFormulaEditText.getText(), this);
//         }
//     }
    OnEquals() {
        var _this = this._obj;
        if (this.mCurrentState == Calculator.CalculatorState.INPUT) {
            this.SetState(Calculator.CalculatorState.EVALUATE);
            this.mEvaluator.Evaluate(this.mFormulaEditText.GetText(), _this);
        }
    }

//     private void onDelete() {
//         // Delete works like backspace; remove the last character from the expression.
//         final Editable formulaText = mFormulaEditText.getEditableText();
//         final int formulaLength = formulaText.length();
//         if (formulaLength > 0) {
//             formulaText.delete(formulaLength - 1, formulaLength);
//         }
//     }
    OnDelete() {
        // Delete works like backspace; remove the last character from the expression.
        var formulaText = mFormulaEditText.getEditableText();
        var formulaLength = formulaText.length();
        if (formulaLength > 0) {
            formulaText.delete(formulaLength - 1, formulaLength);
        }
    }

//     private void reveal(View sourceView, int colorRes, AnimatorListener listener) {
//         final ViewGroupOverlay groupOverlay =
//                 (ViewGroupOverlay) getWindow().getDecorView().getOverlay();

//         final Rect displayRect = new Rect();
//         mDisplayView.getGlobalVisibleRect(displayRect);

//         // Make reveal cover the display and status bar.
//         final View revealView = new View(this);
//         revealView.setBottom(displayRect.bottom);
//         revealView.setLeft(displayRect.left);
//         revealView.setRight(displayRect.right);
//         revealView.setBackgroundColor(getResources().getColor(colorRes));
//         groupOverlay.add(revealView);

//         final int[] clearLocation = new int[2];
//         sourceView.getLocationInWindow(clearLocation);
//         clearLocation[0] += sourceView.getWidth() / 2;
//         clearLocation[1] += sourceView.getHeight() / 2;

//         final int revealCenterX = clearLocation[0] - revealView.getLeft();
//         final int revealCenterY = clearLocation[1] - revealView.getTop();

//         final double x1_2 = Math.pow(revealView.getLeft() - revealCenterX, 2);
//         final double x2_2 = Math.pow(revealView.getRight() - revealCenterX, 2);
//         final double y_2 = Math.pow(revealView.getTop() - revealCenterY, 2);
//         final float revealRadius = (float) Math.max(Math.sqrt(x1_2 + y_2), Math.sqrt(x2_2 + y_2));

//         final Animator revealAnimator =
//                 ViewAnimationUtils.createCircularReveal(revealView,
//                         revealCenterX, revealCenterY, 0.0f, revealRadius);
//         revealAnimator.setDuration(
//                 getResources().getInteger(android.R.integer.config_longAnimTime));
//         revealAnimator.addListener(listener);

//         final Animator alphaAnimator = ObjectAnimator.ofFloat(revealView, View.ALPHA, 0.0f);
//         alphaAnimator.setDuration(
//                 getResources().getInteger(android.R.integer.config_mediumAnimTime));

//         final AnimatorSet animatorSet = new AnimatorSet();
//         animatorSet.play(revealAnimator).before(alphaAnimator);
//         animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
//         animatorSet.addListener(new AnimatorListenerAdapter() {
//             @Override
//             public void onAnimationEnd(Animator animator) {
//                 groupOverlay.remove(revealView);
//                 mCurrentAnimator = null;
//             }
//         });

//         mCurrentAnimator = animatorSet;
//         animatorSet.start();
//     }
    Reveal(sourceView, colorRes, listener) {
        var groupOverlay =
                getWindow().getDecorView().getOverlay();

        var displayRect = new Rect();
        mDisplayView.getGlobalVisibleRect(displayRect);

        // Make reveal cover the display and status bar.
        var revealView = new View(this);
        revealView.setBottom(displayRect.bottom);
        revealView.setLeft(displayRect.left);
        revealView.setRight(displayRect.right);
        revealView.setBackgroundColor(getResources().getColor(colorRes));
        groupOverlay.add(revealView);

        var clearLocation = new int[2];
        sourceView.getLocationInWindow(clearLocation);
        clearLocation[0] += sourceView.getWidth() / 2;
        clearLocation[1] += sourceView.getHeight() / 2;

        var revealCenterX = clearLocation[0] - revealView.getLeft();
        var revealCenterY = clearLocation[1] - revealView.getTop();

        var x1_2 = Math.pow(revealView.getLeft() - revealCenterX, 2);
        var x2_2 = Math.pow(revealView.getRight() - revealCenterX, 2);
        var y_2 = Math.pow(revealView.getTop() - revealCenterY, 2);
        var revealRadius = Math.max(Math.sqrt(x1_2 + y_2), Math.sqrt(x2_2 + y_2));

        var revealAnimator =
                ViewAnimationUtils.createCircularReveal(revealView,
                        revealCenterX, revealCenterY, 0.0, revealRadius);
        revealAnimator.setDuration(
                getResources().getInteger(android.R.integer.config_longAnimTime));
        revealAnimator.addListener(listener);

        var alphaAnimator = ObjectAnimator.ofFloat(revealView, View.ALPHA, 0.0);
        alphaAnimator.setDuration(
                getResources().getInteger(android.R.integer.config_mediumAnimTime));

        var animatorSet = new AnimatorSet();
        animatorSet.play(revealAnimator).before(alphaAnimator);
        animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
        animatorSet.addListener(new class _ extends AnimatorListenerAdapter {
            // @Override
            OnAnimationEnd(animator) {
                groupOverlay.remove(revealView);
                mCurrentAnimator = null;
            }
        }());

        mCurrentAnimator = animatorSet;
        animatorSet.start();
    }

//     private void onClear() {
//         if (TextUtils.isEmpty(mFormulaEditText.getText())) {
//             return;
//         }

//         reveal(mCurrentButton, R.color.calculator_accent_color, new AnimatorListenerAdapter() {
//             @Override
//             public void onAnimationEnd(Animator animation) {
//                 mFormulaEditText.getEditableText().clear();
//             }
//         });
//     }
    OnClear() {
        if (TextUtils.isEmpty(mFormulaEditText.getText())) {
            return;
        }

        reveal(mCurrentButton, R.color.calculator_accent_color, new class _ extends AnimatorListenerAdapter {
            // @Override
            OnAnimationEnd(animation) {
                mFormulaEditText.getEditableText().clear();
            }
        }());
    }

//     private void onError(final int errorResourceId) {
//         if (mCurrentState != CalculatorState.EVALUATE) {
//             // Only animate error on evaluate.
//             mResultEditText.setText(errorResourceId);
//             return;
//         }

//         reveal(mCurrentButton, R.color.calculator_error_color, new AnimatorListenerAdapter() {
//             @Override
//             public void onAnimationEnd(Animator animation) {
//                 setState(CalculatorState.ERROR);
//                 mResultEditText.setText(errorResourceId);
//             }
//         });
//     }
    OnError(errorResourceId) {
        if (this.mCurrentState != Calculator.CalculatorState.EVALUATE) {
            // Only animate error on evaluate.
            mResultEditText.setText(errorResourceId);
            return;
        }

        reveal(mCurrentButton, R.color.calculator_error_color, new class _ extends AnimatorListenerAdapter {
            // @Override
            OnAnimationEnd(animation) {
                setState(Calculator.CalculatorState.ERROR);
                mResultEditText.setText(errorResourceId);
            }
        }());
    }

//     private void onResult(final String result) {
//         // Calculate the values needed to perform the scale and translation animations,
//         // accounting for how the scale will affect the final position of the text.
//         final float resultScale =
//                 mFormulaEditText.getVariableTextSize(result) / mResultEditText.getTextSize();
//         final float resultTranslationX = (1.0f - resultScale) *
//                 (mResultEditText.getWidth() / 2.0f - mResultEditText.getPaddingEnd());
//         final float resultTranslationY = (1.0f - resultScale) *
//                 (mResultEditText.getHeight() / 2.0f - mResultEditText.getPaddingBottom()) +
//                 (mFormulaEditText.getBottom() - mResultEditText.getBottom()) +
//                 (mResultEditText.getPaddingBottom() - mFormulaEditText.getPaddingBottom());
//         final float formulaTranslationY = -mFormulaEditText.getBottom();

//         // Use a value animator to fade to the final text color over the course of the animation.
//         final int resultTextColor = mResultEditText.getCurrentTextColor();
//         final int formulaTextColor = mFormulaEditText.getCurrentTextColor();
//         final ValueAnimator textColorAnimator =
//                 ValueAnimator.ofObject(new ArgbEvaluator(), resultTextColor, formulaTextColor);
//         textColorAnimator.addUpdateListener(new AnimatorUpdateListener() {
//             @Override
//             public void onAnimationUpdate(ValueAnimator valueAnimator) {
//                 mResultEditText.setTextColor((int) valueAnimator.getAnimatedValue());
//             }
//         });

//         final AnimatorSet animatorSet = new AnimatorSet();
//         animatorSet.playTogether(
//                 textColorAnimator,
//                 ObjectAnimator.ofFloat(mResultEditText, View.SCALE_X, resultScale),
//                 ObjectAnimator.ofFloat(mResultEditText, View.SCALE_Y, resultScale),
//                 ObjectAnimator.ofFloat(mResultEditText, View.TRANSLATION_X, resultTranslationX),
//                 ObjectAnimator.ofFloat(mResultEditText, View.TRANSLATION_Y, resultTranslationY),
//                 ObjectAnimator.ofFloat(mFormulaEditText, View.TRANSLATION_Y, formulaTranslationY));
//         animatorSet.setDuration(getResources().getInteger(android.R.integer.config_longAnimTime));
//         animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
//         animatorSet.addListener(new AnimatorListenerAdapter() {
//             @Override
//             public void onAnimationStart(Animator animation) {
//                 mResultEditText.setText(result);
//             }

//             @Override
//             public void onAnimationEnd(Animator animation) {
//                 // Reset all of the values modified during the animation.
//                 mResultEditText.setTextColor(resultTextColor);
//                 mResultEditText.setScaleX(1.0f);
//                 mResultEditText.setScaleY(1.0f);
//                 mResultEditText.setTranslationX(0.0f);
//                 mResultEditText.setTranslationY(0.0f);
//                 mFormulaEditText.setTranslationY(0.0f);

//                 // Finally update the formula to use the current result.
//                 mFormulaEditText.setText(result);
//                 setState(CalculatorState.RESULT);

//                 mCurrentAnimator = null;
//             }
//         });

//         mCurrentAnimator = animatorSet;
//         animatorSet.start();
//     }
    OnResult(result) {
        // Calculate the values needed to perform the scale and translation animations,
        // accounting for how the scale will affect the final position of the text.
        var resultScale =
                mFormulaEditText.getVariableTextSize(result) / mResultEditText.getTextSize();
        var resultTranslationX = (1.0 - resultScale) *
                (mResultEditText.getWidth() / 2.0 - mResultEditText.getPaddingEnd());
        var resultTranslationY = (1.0 - resultScale) *
                (mResultEditText.getHeight() / 2.0 - mResultEditText.getPaddingBottom()) +
                (mFormulaEditText.getBottom() - mResultEditText.getBottom()) +
                (mResultEditText.getPaddingBottom() - mFormulaEditText.getPaddingBottom());
        var formulaTranslationY = -mFormulaEditText.getBottom();

        // Use a value animator to fade to the final text color over the course of the animation.
        var resultTextColor = mResultEditText.getCurrentTextColor();
        var formulaTextColor = mFormulaEditText.getCurrentTextColor();
        var textColorAnimator =
                ValueAnimator.ofObject(new ArgbEvaluator(), resultTextColor, formulaTextColor);
        textColorAnimator.addUpdateListener(new class _ extends AnimatorUpdateListener() {
            // @Override
            OnAnimationUpdate(valueAnimator) {
                mResultEditText.setTextColor(valueAnimator.getAnimatedValue());
            }
        }());

        var animatorSet = new AnimatorSet();
        animatorSet.playTogether(
                textColorAnimator,
                ObjectAnimator.ofFloat(mResultEditText, View.SCALE_X, resultScale),
                ObjectAnimator.ofFloat(mResultEditText, View.SCALE_Y, resultScale),
                ObjectAnimator.ofFloat(mResultEditText, View.TRANSLATION_X, resultTranslationX),
                ObjectAnimator.ofFloat(mResultEditText, View.TRANSLATION_Y, resultTranslationY),
                ObjectAnimator.ofFloat(mFormulaEditText, View.TRANSLATION_Y, formulaTranslationY));
        animatorSet.setDuration(getResources().getInteger(android.R.integer.config_longAnimTime));
        animatorSet.setInterpolator(new AccelerateDecelerateInterpolator());
        animatorSet.addListener(new class _ extends AnimatorListenerAdapter() {
            // @Override
            OnAnimationStart(animation) {
                mResultEditText.setText(result);
            }

            // @Override
            OnAnimationEnd(animation) {
                // Reset all of the values modified during the animation.
                mResultEditText.setTextColor(resultTextColor);
                mResultEditText.setScaleX(1.0);
                mResultEditText.setScaleY(1.0);
                mResultEditText.setTranslationX(0.0);
                mResultEditText.setTranslationY(0.0);
                mFormulaEditText.setTranslationY(0.0);

                // Finally update the formula to use the current result.
                mFormulaEditText.setText(result);
                setState(Calculator.CalculatorState.RESULT);

                mCurrentAnimator = null;
            }
        }());

        mCurrentAnimator = animatorSet;
        animatorSet.start();
    }

}   //class Calculator

//--------.java----end----

    class Calculator_ extends Calculator {
        constructor(){
            elog('====Calculator::constructor====begin====');
            super();
            elog('====Calculator::constructor====end====');
        }

        OnBackPressed(_this){
            elog('====Calculator::OnBackPressed====begin====');
            super.OnBackPressed(_this);
            elog('====Calculator::OnBackPressed====end====');
        }
        OnUserInteraction(_this){
            elog('====Calculator::OnUserInteraction====begin====');
            super.OnUserInteraction(_this);
            elog('====Calculator::OnBackPreOnUserInteraction====end====');
        }
        // OnLongClick(_this, iview, result){}
        // OnEvaluate(expr, result, errorResourceId){}
        // OnTextSizeChanged(_this, textView, oldSize) {}
        OnCreate(_this, savedInstanceState) {
            elog('====Calculator::OnCreate====begin====');
            super.OnCreate(_this, savedInstanceState);
            elog('====Calculator::OnCreate====end====');
        }
        OnSaveInstanceState(_this, outState) {
            elog('====Calculator::OnSaveInstanceState====begin====');
            super.OnSaveInstanceState(_this, outState);
            elog('====Calculator::OnSaveInstanceState====end====');
        }

        OnHandleMessage(_this, aoMessage){
            elog('====Calculator::OnHandleMessage====begin====');
        }
    }   //class Calculator

    return new Calculator_();

};  //module.exports

elog("====Calculator.js====end====");