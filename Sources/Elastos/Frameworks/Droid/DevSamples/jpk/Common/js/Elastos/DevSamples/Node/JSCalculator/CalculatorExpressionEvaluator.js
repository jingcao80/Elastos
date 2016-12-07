elog("====CalculatorExpressionEvaluator.js====begin====");

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

// import org.javia.arity.Symbols;
var Symbols = require("./arity/Symbols.js")(aoElastos, aoActivity);
// import org.javia.arity.SyntaxException;
var SyntaxException = require("./arity/SyntaxException.js")(aoElastos, aoActivity);
// import org.javia.arity.Util;
var Util = require("./arity/Util.js")(aoElastos, aoActivity);

// public class CalculatorExpressionEvaluator {
class CalculatorExpressionEvaluator {

//     private static final int MAX_DIGITS = 12;
    static get MAX_DIGITS() {return 12;}
//     private static final int ROUNDING_DIGITS = 2;
    static get ROUNDING_DIGITS() {return 2;}

//     private final Symbols mSymbols;
//     private final CalculatorExpressionTokenizer mTokenizer;

//     public CalculatorExpressionEvaluator(CalculatorExpressionTokenizer tokenizer) {
//         mSymbols = new Symbols();
//         mTokenizer = tokenizer;
//     }
    constructor(tokenizer) {
        this.mSymbols = new Symbols();
        this.mTokenizer = tokenizer;
    }

//     public void evaluate(CharSequence expr, EvaluateCallback callback) {
//         evaluate(expr.toString(), callback);
//     }
    Evaluate_0(expr, callback) {
        evaluate(expr.ToString(), callback);
    }

//     public void evaluate(String expr, EvaluateCallback callback) {
//         expr = mTokenizer.getNormalizedExpression(expr);

//         // remove any trailing operators
//         while (expr.length() > 0 && "+-/*".indexOf(expr.charAt(expr.length() - 1)) != -1) {
//             expr = expr.substring(0, expr.length() - 1);
//         }

//         try {
//             if (expr.length() == 0 || Double.valueOf(expr) != null) {
//                 callback.onEvaluate(expr, null, Calculator.INVALID_RES_ID);
//                 return;
//             }
//         } catch (NumberFormatException e) {
//             // expr is not a simple number
//         }

//         try {
//             double result = mSymbols.eval(expr);
//             if (Double.isNaN(result)) {
//                 callback.onEvaluate(expr, null, R.string.error_nan);
//             } else {
//                 // The arity library uses floating point arithmetic when evaluating the expression
//                 // leading to precision errors in the result. The method doubleToString hides these
//                 // errors; rounding the result by dropping N digits of precision.
//                 final String resultString = mTokenizer.getLocalizedExpression(
//                         Util.doubleToString(result, MAX_DIGITS, ROUNDING_DIGITS));
//                 callback.onEvaluate(expr, resultString, Calculator.INVALID_RES_ID);
//             }
//         } catch (SyntaxException e) {
//             callback.onEvaluate(expr, null, R.string.error_syntax);
//         }
//     }
    Evaluate(expr, callback) {
        if (typeof expr != "string") expr = expr.ToString();

        expr = this.mTokenizer.GetNormalizedExpression(expr);

        // remove any trailing operators
        while (expr.length > 0 && "+-/*".indexOf(expr.charAt(expr.length - 1)) != -1) {
            expr = expr.substring(0, expr.length() - 1);
        }

        try {
            if (expr.length == 0 || Double.valueOf(expr) != null) {
                callback.onEvaluate(expr, null, Calculator.INVALID_RES_ID);
                return;
            }
        } catch (e) {
            // expr is not a simple number
        }

        try {
            var result = mSymbols.eval(expr);
            if (Double.isNaN(result)) {
                callback.onEvaluate(expr, null, R.string.error_nan);
            } else {
                // The arity library uses floating point arithmetic when evaluating the expression
                // leading to precision errors in the result. The method doubleToString hides these
                // errors; rounding the result by dropping N digits of precision.
                var resultString = this.mTokenizer.getLocalizedExpression(
                        Util.doubleToString(result, MAX_DIGITS, ROUNDING_DIGITS));
                callback.onEvaluate(expr, resultString, Calculator.INVALID_RES_ID);
            }
        } catch (e) {
            //s.t();Assert(0);
            callback.OnEvaluate(expr, null, R.string.error_syntax);
        }
    }

//     public interface EvaluateCallback {
//         public void onEvaluate(String expr, String result, int errorResourceId);
//     }
    static get EvaluateCallback() {
        return class _ {
            OnEvaluate(expr, result, errorResourceId) {
                elog("====EvaluateCallback::OnEvaluate====virtual====");
                Assert(0);
            }
        }
    }

}   //class CalculatorExpressionEvaluator

//--------.java----end----

return CalculatorExpressionEvaluator;

};  //module.exports

elog("====CalculatorExpressionEvaluator.js====end====");