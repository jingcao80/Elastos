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

/*
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
// import android.graphics.Rect;
// import android.os.Bundle;
// import android.support.annotation.NonNull;
// import android.support.v4.view.ViewPager;
// import android.text.Editable;
// import android.text.TextUtils;
// import android.text.TextWatcher;
// import android.view.KeyEvent;
// import android.view.View;
// import android.view.View.OnKeyListener;
// import android.view.View.OnLongClickListener;
// import android.view.ViewAnimationUtils;
// import android.view.ViewGroupOverlay;
// import android.view.animation.AccelerateDecelerateInterpolator;
// import android.widget.Button;
// import android.widget.TextView;

// import com.android.calculator2.CalculatorEditText.OnTextSizeChangeListener;
// import com.android.calculator2.CalculatorExpressionEvaluator.EvaluateCallback;

    var CalculatorExpressionTokenizer = require("CalculatorExpressionTokenizer.js")(aoElastos, aoActivity);
    var CalculatorExpressionEvaluator = require("CalculatorExpressionEvaluator.js")(aoElastos, aoActivity);
    var CalculatorExpressionBuilder = require("CalculatorExpressionBuilder.js")(aoElastos, aoActivity);

// public class Calculator extends Activity
//         implements OnTextSizeChangeListener, EvaluateCallback, OnLongClickListener {
    function CActivityListener(host) {
        this.mHost = host;
    }
    var _apt = CActivityListener.prototype;

//     private static final String NAME = Calculator.class.getName();
        var NAME = oActivity.ToString();

//     // instance state keys
//     private static final String KEY_CURRENT_STATE = NAME + "_currentState";
        var KEY_CURRENT_STATE = NAME + "_currentState";
//     private static final String KEY_CURRENT_EXPRESSION = NAME + "_currentExpression";
        var KEY_CURRENT_EXPRESSION = NAME + "_currentExpression";

//     /**
//      * Constant for an invalid resource id.
//      */
//     public static final int INVALID_RES_ID = -1;
        var INVALID_RES_ID = -1;

//     private enum CalculatorState {
//         INPUT, EVALUATE, RESULT, ERROR
//     }
        var CalculatorState = {
            INPUT:0, EVALUATE:1, RESULT:2, ERROR:3
        };

//     private final TextWatcher mFormulaTextWatcher = new TextWatcher() {
        var mFormulaTextWatcher = {
//         @Override
//         public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) {
            BeforeTextChanged : function(charSequence, start, count, after) {
//         }
            },

//         @Override
//         public void onTextChanged(CharSequence charSequence, int start, int count, int after) {
            OnTextChanged : function(charSequence, start, count, after) {
//         }
            },

//         @Override
//         public void afterTextChanged(Editable editable) {
            AfterTextChanged : function(editable) {
//             setState(CalculatorState.INPUT);
                setState(CalculatorState.INPUT);
//             mEvaluator.evaluate(editable, Calculator.this);
                mEvaluator.evaluate(editable, oActivity);
//         }
            }
//     };
        };

//     private final OnKeyListener mFormulaOnKeyListener = new OnKeyListener() {
        var mFormulaOnKeyListener = {
//         @Override
//         public boolean onKey(View view, int keyCode, KeyEvent keyEvent) {
            OnKey : function(view, keyCode, keyEvent) {
//             switch (keyCode) {
                switch (keyCode) {
//                 case KeyEvent.KEYCODE_NUMPAD_ENTER:
                    case KeyEvent.KEYCODE_NUMPAD_ENTER:
//                 case KeyEvent.KEYCODE_ENTER:
                    case KeyEvent.KEYCODE_ENTER:
                    {
//                     if (keyEvent.getAction() == KeyEvent.ACTION_UP) {
                        if (keyEvent.GetAction() == KeyEvent.ACTION_UP) {
//                         mCurrentButton = mEqualButton;
                            mCurrentButton = mEqualButton;
//                         onEquals();
                            onEquals();
//                     }
                        }
//                     // ignore all other actions
//                     return true;
                        return true;
                    }
//             }
                }
//             return false;
                return false;
//         }
            }
//     };
        };

//     private final Editable.Factory mFormulaEditableFactory = new Editable.Factory() {
        var mFormulaEditableFactory = {
//         @Override
//         public Editable newEditable(CharSequence source) {
            newEditable : function (source, result) {
//             final boolean isEdited = mCurrentState == CalculatorState.INPUT
//                     || mCurrentState == CalculatorState.ERROR;
                var isEdited = mCurrentState == CalculatorState.INPUT
                        || mCurrentState == CalculatorState.ERROR;
//             return new CalculatorExpressionBuilder(source, mTokenizer, isEdited);
                result.data = new CalculatorExpressionBuilder(source, mTokenizer, isEdited);
//         }
            }
//     };
        };

//     private CalculatorState mCurrentState;
        var mCurrentState;
//     private CalculatorExpressionTokenizer mTokenizer;
        var mTokenizer;
//     private CalculatorExpressionEvaluator mEvaluator;
        var mEvaluator;

//     private View mDisplayView;
        var mDisplayView;
//     private CalculatorEditText mFormulaEditText;
        var mFormulaEditText;
//     private CalculatorEditText mResultEditText;
        var mResultEditText;
//     private ViewPager mPadViewPager;
        var mPadViewPager;
//     private View mDeleteButton;
        var mDeleteButton;
//     private View mEqualButton;
        var mEqualButton;
//     private View mClearButton;
        var mClearButton;

//     private View mCurrentButton;
        var mCurrentButton;
//     private Animator mCurrentAnimator;
        var mCurrentAnimator;

//     @Override
//     protected void onCreate(Bundle savedInstanceState) {
        _apt.OnCreate = function(context, savedInstanceState){
            elog('====jso_activity_cb====OnCreate.begin====');

//         super.onCreate(savedInstanceState);
//         setContentView(R.layout.activity_calculator);
            oActivity.SetContentView(R.layout.activity_calculator);

//         mDisplayView = findViewById(R.id.display);
            mDisplayView = oActivity.FindViewById(R.id.display);
//         mFormulaEditText = (CalculatorEditText) findViewById(R.id.formula);
            mFormulaEditText = oActivity.FindViewById(R.id.formula);
//         mResultEditText = (CalculatorEditText) findViewById(R.id.result);
            mResultEditText = oActivity.FindViewById(R.id.result);
//         mPadViewPager = (ViewPager) findViewById(R.id.pad_pager);
            mPadViewPager = oActivity.FindViewById(R.id.pad_pager);
//         mDeleteButton = findViewById(R.id.del);
            mDeleteButton = oActivity.FindViewById(R.id.del);
//         mClearButton = findViewById(R.id.clr);
            mClearButton = oActivity.FindViewById(R.id.clr);

//         mEqualButton = findViewById(R.id.pad_numeric).findViewById(R.id.eq);
            mEqualButton = oActivity.FindViewById(R.id.pad_numeric).FindViewById(R.id.eq);
//         if (mEqualButton == null || mEqualButton.getVisibility() != View.VISIBLE) {
            if (mEqualButton == null || mEqualButton.GetVisibility() != IView__VISIBLE) {
//             mEqualButton = findViewById(R.id.pad_operator).findViewById(R.id.eq);
                mEqualButton = oActivity.FindViewById(R.id.pad_operator).FindViewById(R.id.eq);
//         }
            }

            elog('====jso_activity_cb====OnCreate==111==');

//         mTokenizer = new CalculatorExpressionTokenizer(this);
            mTokenizer = new CalculatorExpressionTokenizer(this);

            elog('====jso_activity_cb====OnCreate==2==');

//         mEvaluator = new CalculatorExpressionEvaluator(mTokenizer);
            mEvaluator = new CalculatorExpressionEvaluator(mTokenizer);

            elog('====jso_activity_cb====OnCreate==3==');

//         savedInstanceState = savedInstanceState == null ? Bundle.EMPTY : savedInstanceState;
            var bhl = Droid_New("Elastos.Droid.Os.CBundleHelper");
            var empty = bhl.GetEMPTY();
            savedInstanceState = savedInstanceState == null ? empty : savedInstanceState;

            elog('====jso_activity_cb====OnCreate==4==');

//         setState(CalculatorState.values()[
//                 savedInstanceState.getInt(KEY_CURRENT_STATE, CalculatorState.INPUT.ordinal())]);
            oActivity.SetState(
                    savedInstanceState.GetInt32(KEY_CURRENT_STATE, CalculatorState.INPUT));

            elog('====jso_activity_cb====OnCreate==5==');

//         mFormulaEditText.setText(mTokenizer.getLocalizedExpression(
//                 savedInstanceState.getString(KEY_CURRENT_EXPRESSION, "")));
            mFormulaEditText.SetText(mTokenizer.GetLocalizedExpression(
                    savedInstanceState.GetString(KEY_CURRENT_EXPRESSION, "")));

            elog('====jso_activity_cb====OnCreate==6==');

//         mEvaluator.evaluate(mFormulaEditText.getText(), this);
            //TODO:this shoud be convert to IEvaluateCallback automatically
            //TODO:just as: IEvaluateCallback::Probe(this)
            mEvaluator.Evaluate(mFormulaEditText.GetText(), this);

            elog('====jso_activity_cb====OnCreate==7==');


//         mFormulaEditText.setEditableFactory(mFormulaEditableFactory);
            mFormulaEditText.SetEditableFactory(mFormulaEditableFactory);
//         mFormulaEditText.addTextChangedListener(mFormulaTextWatcher);
            mFormulaEditText.AddTextChangedListener(mFormulaTextWatcher);
//         mFormulaEditText.setOnKeyListener(mFormulaOnKeyListener);
            mFormulaEditText.SetOnKeyListener(mFormulaOnKeyListener);
//         mFormulaEditText.setOnTextSizeChangeListener(this);
            mFormulaEditText.SetOnTextSizeChangeListener(this);
//         mDeleteButton.setOnLongClickListener(this);
            mDeleteButton.DetOnLongClickListener(this);

            elog('====jso_activity_cb====OnCreate.end====');
//     }
        }

//     @Override
//     protected void onSaveInstanceState(@NonNull Bundle outState) {
        _apt.OnSaveInstanceState = function(context, outState){
            elog('====jso_activity_cb====OnSaveInstanceState.begin====');

//         // If there's an animation in progress, cancel it first to ensure our state is up-to-date.
//         if (mCurrentAnimator != null) {
//             mCurrentAnimator.cancel();
//         }

//         super.onSaveInstanceState(outState);

//         outState.putInt(KEY_CURRENT_STATE, mCurrentState.ordinal());
//         outState.putString(KEY_CURRENT_EXPRESSION,
//                 mTokenizer.getNormalizedExpression(mFormulaEditText.getText().toString()));
//     }
        }

//     private void setState(CalculatorState state) {
        function setState(state) {
            elog('====Calculator.js====setState.begin====');

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
        }

//     @Override
//     public void onBackPressed() {
        _apt.OnBackPressed = function(context){
            elog('====jso_activity_cb====OnBackPressed.begin====');

//         if (mPadViewPager == null || mPadViewPager.getCurrentItem() == 0) {
//             // If the user is currently looking at the first pad (or the pad is not paged),
//             // allow the system to handle the Back button.
//             super.onBackPressed();
//         } else {
//             // Otherwise, select the previous pad.
//             mPadViewPager.setCurrentItem(mPadViewPager.getCurrentItem() - 1);
//         }
//     }
        }

//     @Override
//     public void onUserInteraction() {
        _apt.OnUserInteraction = function(context){
            elog('====jso_activity_cb====OnUserInteraction.begin====');

//         super.onUserInteraction();

//         // If there's an animation in progress, cancel it so the user interaction can be handled
//         // immediately.
//         if (mCurrentAnimator != null) {
//             mCurrentAnimator.cancel();
//         }
//     }
        }

//     public void onButtonClick(View view) {
        function onButtonClick(view) {
            elog('====Calculator.js====onButtonClick.begin====');

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
        }

//     @Override
//     public boolean onLongClick(View view) {
        _apt.OnLongClick = function(context, view){
            elog('====jso_activity_cb====OnLongClick.begin====');

//         mCurrentButton = view;

//         if (view.getId() == R.id.del) {
//             onClear();
//             return true;
//         }
//         return false;
//     }
        }

//     @Override
//     public void onEvaluate(String expr, String result, int errorResourceId) {
        _apt.OnEvaluate = function(context, expr, result, errorResourceId){
            elog('====jso_activity_cb====OnEvaluate.begin====');

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
        }

//     @Override
//     public void onTextSizeChanged(final TextView textView, float oldSize) {
        _apt.OnTextSizeChanged = function(context, textView, oldSize){
            elog('====jso_activity_cb====OnTextSizeChanged.begin====');

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
        }

//     private void onEquals() {
        function onEquals() {
            elog('====Calculator.js====onEquals.begin====');

//         if (mCurrentState == CalculatorState.INPUT) {
//             setState(CalculatorState.EVALUATE);
//             mEvaluator.evaluate(mFormulaEditText.getText(), this);
//         }
//     }
        }

//     private void onDelete() {
        function onDelete() {
            elog('====Calculator.js====onDelete.begin====');

//         // Delete works like backspace; remove the last character from the expression.
//         final Editable formulaText = mFormulaEditText.getEditableText();
//         final int formulaLength = formulaText.length();
//         if (formulaLength > 0) {
//             formulaText.delete(formulaLength - 1, formulaLength);
//         }
//     }
        }

//     private void reveal(View sourceView, int colorRes, AnimatorListener listener) {
        function reveal(sourceView, colorRes, listener) {
            elog('====Calculator.js====reveal.begin====');

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
        }

//     private void onClear() {
        function onClear() {
            elog('====Calculator.js====onClear.begin====');

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
        }

//     private void onError(final int errorResourceId) {
        function onError(errorResourceId) {
            elog('====Calculator.js====onError.begin====');

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
        }

//     private void onResult(final String result) {
        function onResult(result) {
            elog('====Calculator.js====onResult.begin====');

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
        }
// }

//--------.java----end----

//--------Listener----default begin----

    // _apt.OnCreate = function(context, savedInstanceState){
    //     elog('====jso_activity_cb====OnCreate.begin====');
    // }  //OnCreate
    _apt.OnStart = function(context){
        elog('====jso_activity_cb====OnStart.begin==000==');
    }
    _apt.OnResume = function(context){
        elog('====jso_activity_cb====OnResume.begin====');
    }
    _apt.OnPause = function(context){
        elog('====jso_activity_cb====OnPause.begin====');
    }
    _apt.OnStop = function(context){
        elog('====jso_activity_cb====OnStop.begin====');
    }
    _apt.OnDestroy = function(context){
        elog('====jso_activity_cb====OnDestroy.begin====');
    }
    _apt.OnActivityResult = function(context, aiRequestCode, aiResultCode, aoIntentData){
        elog('====jso_activity_cb====OnActivityResult.begin====');
    }
    _apt.OnCreateDialog = function(context, aiId, out_aoDialog){
        elog('====jso_activity_cb====OnCreateDialog.begin====');
    }   //OnCreateDialog
    _apt.OnCreateContextMenu = function(context, aoMenu, aoV, aoMenuInfo){
        elog('====jso_activity_cb====OnCreateContextMenu.begin====');
    }
    _apt.CreateNavigationBar = function(context){
        elog('====jso_activity_cb====CreateNavigationBar.begin====');
    }
    _apt.OnExchangeData = function(aiTimes ,aoInObject, aoOutObject) {
        elog('====jso_activity_cb====OnExchangeData.begin====');
    }
    _apt.OnHandleMessage = function(context, aoMessage){
        elog('====jso_activity_cb====OnHandleMessage.begin====');
    }

//--------Listener----default end----

    var oListener = new CActivityListener(oActivity);
    return oListener;
};  //module.exports