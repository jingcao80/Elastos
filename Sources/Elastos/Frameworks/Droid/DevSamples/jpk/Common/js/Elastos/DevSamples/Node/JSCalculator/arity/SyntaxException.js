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
 * Copyright (C) 2007-2008 Mihai Preda.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// package org.javia.arity;

// /**
//    Thrown when the expression can't be compiled, because it's either not
//    well-formed (e.g. "1+"), or because some simbols aren't defined (e.g. "foo+2").
// */
// public class SyntaxException extends Exception {
    function SyntaxException() {}   //no constructor
    var _pt = SyntaxException.prototype;

//     /**
//        The expression which caused the error.
//      */
//     public String expression;

//     /**
//        Explicative message (cause of error).
//      */
//     public String message;

//     /**
//        The position inside expression where the error occured.
//      */
//     public int position;

//     public String toString() {
//         return "SyntaxException: " + message
//             + " in '" + expression
//             + "' at position " + position;
//     }

//     SyntaxException set(String str, int pos) {
//         message  = str;
//         position = pos;
//         fillInStackTrace();
//         return this;
//     }
// }

//--------.java----end----

    return SyntaxException;
};  //module.exports