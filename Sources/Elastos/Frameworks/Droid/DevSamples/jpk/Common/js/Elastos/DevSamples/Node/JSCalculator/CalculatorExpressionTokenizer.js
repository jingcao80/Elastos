elog("====CalculatorExpressionTokenizer.js====begin====");

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
//  * Licensed under the Apache License, Version 2.0 (the 'License');
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *   http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an 'AS IS' BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// package com.android.calculator2;

// import android.content.Context;

// import java.util.HashMap;
// import java.util.Map;
// import java.util.Map.Entry;

// public class CalculatorExpressionTokenizer {
class _CalculatorExpressionTokenizer {

//     private final Map<String, String> mReplacementMap;

//     public CalculatorExpressionTokenizer(Context context) {
//         mReplacementMap = new HashMap<>();

//         mReplacementMap.set(".", context.GetString(R.string.dec_point));

//         mReplacementMap.set("0", context.GetString(R.string.digit_0));
//         mReplacementMap.set("1", context.GetString(R.string.digit_1));
//         mReplacementMap.set("2", context.GetString(R.string.digit_2));
//         mReplacementMap.set("3", context.GetString(R.string.digit_3));
//         mReplacementMap.set("4", context.GetString(R.string.digit_4));
//         mReplacementMap.set("5", context.GetString(R.string.digit_5));
//         mReplacementMap.set("6", context.GetString(R.string.digit_6));
//         mReplacementMap.set("7", context.GetString(R.string.digit_7));
//         mReplacementMap.set("8", context.GetString(R.string.digit_8));
//         mReplacementMap.set("9", context.GetString(R.string.digit_9));

//         mReplacementMap.set("/", context.GetString(R.string.op_div));
//         mReplacementMap.set("*", context.GetString(R.string.op_mul));
//         mReplacementMap.set("-", context.GetString(R.string.op_sub));

//         mReplacementMap.set("cos", context.GetString(R.string.fun_cos));
//         mReplacementMap.set("ln", context.GetString(R.string.fun_ln));
//         mReplacementMap.set("log", context.GetString(R.string.fun_log));
//         mReplacementMap.set("sin", context.GetString(R.string.fun_sin));
//         mReplacementMap.set("tan", context.GetString(R.string.fun_tan));

//         mReplacementMap.set("Infinity", context.GetString(R.string.inf));
//     }
    constructor(context) {
        this.mReplacementMap = new Map();

        this.mReplacementMap.set(".", context.GetString(R.string.dec_point));

        this.mReplacementMap.set("0", context.GetString(R.string.digit_0));
        this.mReplacementMap.set("1", context.GetString(R.string.digit_1));
        this.mReplacementMap.set("2", context.GetString(R.string.digit_2));
        this.mReplacementMap.set("3", context.GetString(R.string.digit_3));
        this.mReplacementMap.set("4", context.GetString(R.string.digit_4));
        this.mReplacementMap.set("5", context.GetString(R.string.digit_5));
        this.mReplacementMap.set("6", context.GetString(R.string.digit_6));
        this.mReplacementMap.set("7", context.GetString(R.string.digit_7));
        this.mReplacementMap.set("8", context.GetString(R.string.digit_8));
        this.mReplacementMap.set("9", context.GetString(R.string.digit_9));

        this.mReplacementMap.set("/", context.GetString(R.string.op_div));
        this.mReplacementMap.set("*", context.GetString(R.string.op_mul));
        this.mReplacementMap.set("-", context.GetString(R.string.op_sub));

        this.mReplacementMap.set("cos", context.GetString(R.string.fun_cos));
        this.mReplacementMap.set("ln", context.GetString(R.string.fun_ln));
        this.mReplacementMap.set("log", context.GetString(R.string.fun_log));
        this.mReplacementMap.set("sin", context.GetString(R.string.fun_sin));
        this.mReplacementMap.set("tan", context.GetString(R.string.fun_tan));

        this.mReplacementMap.set("Infinity", context.GetString(R.string.inf));
    }

//     public String getNormalizedExpression(String expr) {
//         for (Entry<String, String> replacementEntry : mReplacementMap.entrySet()) {
//             expr = expr.replace(replacementEntry.getValue(), replacementEntry.getKey());
//         }
//         return expr;
//     }
    GetNormalizedExpression(expr) {
        for (let [key, value] of this.mReplacementMap.entries()) {
            expr = expr.replace(value, key);
        }
        return expr;
    }

//     public String getLocalizedExpression(String expr) {
//         for (Entry<String, String> replacementEntry : mReplacementMap.entrySet()) {
//             expr = expr.replace(replacementEntry.getKey(), replacementEntry.getValue());
//         }
//         return expr;
//     }
    GetLocalizedExpression(expr) {
        for (let [key, value] of this.mReplacementMap.entries()) {
            expr = expr.replace(key, value);
        }
        return expr;
    }

}   //class _CalculatorExpressionTokenizer

//--------.java----end----

    class CalculatorExpressionTokenizer extends _CalculatorExpressionTokenizer{
        constructor(context) {
            elog("====CalculatorExpressionTokenizer::constructor====");
            super(context);
            //Assert(0);
        }
    }

    return CalculatorExpressionTokenizer;

};  //module.exports

elog("====CalculatorExpressionTokenizer.js====end====");