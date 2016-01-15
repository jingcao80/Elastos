var o = document.getElementsByTagName('BODY')[0];

var r = '';

function testCarDataType() {
	try {
        var ls_econame = '/data/elastos/JSTestObject1.eco';
        var ls_clsname = 'CTestObject';
        var carObject = window.CarObjectFactory.CreateInstance(ls_econame, ls_clsname);

		r += '--' + carObject.Test_NULL();
        r += '--' + carObject.Test_Int16(16);
        r += '--' + carObject.Test_Int32(32);
        r += '--' + carObject.Test_Int64(64);
        r += '--' + carObject.Test_Byte(8);
        r += '--' + carObject.Test_Float(12.34);
        r += '--' + carObject.Test_Double(1234.5678);
        r += '--' + carObject.Test_Char('A');
        r += '--' + carObject.Test_String('HelloWorld');
        r += '--' + carObject.Test_Boolean(false);
        var lo_ret = carObject.Test_ECode(0xA0010000);
        r += '--' + lo_ret.toString(16);
        r += '--' + carObject.Test_Enum(0x1);
        lo_ret = '--' + carObject.Test_ArrayOf_Int16([1, 2, 3]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Int32([4, 5, 6]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Int64([7, 8, 9]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Byte([10, 11, 12]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Float([12.34, 56.78, 12.78]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Double([1234.5678, 2345.6789, 3456.7890]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Char(['A', 'B', 'C']);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_String(['ABC', 'BCD', 'CDE']);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Boolean([true, false, true]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_ECode([0xA0010000, 0xA0010001, 0xA0010002]);
        r += '--' + JSON.stringify(lo_ret) + '--';
        lo_ret = '--' + carObject.Test_ArrayOf_Enum([0x1, 0x10, 0x100]);
        r += '--' + JSON.stringify(lo_ret) + '--';

        window.CarLogger.Log('Begin test Callback');

        var jsObj = (function(){
            return {
                OnEvent1:function(i) {
                    var s = 'call jsObj.OnEvent1, i: ' + JSON.stringify(i);
                    window.CarLogger.Log(i);
                },
                OnEvent2:function(s) {
                    var s = 'call jsObj.OnEvent2, s: ' + s;
                    window.CarLogger.Log(s);
                }
            }
        })();

        window.CarLogger.Log('call carObject.Test_AddEventListener');
        carObject.Test_AddEventListener(jsObj);
	}
    catch(e) {
		r +=' error';
	}
}

testCarDataType();

/*
//--------OK--------
try{
r += '--'+window.CarObject.Test_NULL();
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Int16(16);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Int32(32);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Int64(64);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Byte(8);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Float(12.34);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Double(1234.5678);
}catch(e){}
*/

/*
try{
var r = window.CarObject.test_07_Char8();
}catch(e){}

try{
var r = window.CarObject.test_08_Char16();
}catch(e){}
*/
/*
try{
var r = window.CarObject.test_09_CString();
}catch(e){}
*/

/*
//--------OK--------
try{
r += '--'+window.CarObject.Test_Int32_String(123);
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_String_Int32_Int32_Int32_Int32_String_String_String_String('ab',12,34,56,78);
}catch(e){}
*/

/*
try{
r += '--'+window.CarObject.test_10_String_PString('abcd');
}catch(e){}
*/

/*
//--------OK--------
try{
r += '--'+window.CarObject.Test_String('abcd');
}catch(e){}

//--------OK--------
try{
r += '--'+window.CarObject.Test_Boolean(true);
}catch(e){}
*/

/*
try{
var r = window.CarObject.test_12_EMuid();
}catch(e){}

try{
var r = window.CarObject.test_13_EGuid();
}catch(e){}

try{
var r = window.CarObject.test_14_ECode();
}catch(e){}

try{
var r = window.CarObject.test_15_LocalPtr();
}catch(e){}

try{
var r = window.CarObject.test_16_LocalType();
}catch(e){}

try{
var r = window.CarObject.test_17_Enum();
}catch(e){}

try{
var r = window.CarObject.test_18_StringBuf();
}catch(e){}

try{
var r = window.CarObject.test_19_ArrayOf();
}catch(e){}
*/
/*
try{
var lo_ret = window.CarObject.Test_ArrayOfInt32_Int32_Int32([1,2,3],12,34,[4,5,6]);
r += '--'+JSON.stringify(lo_ret);
r += '--';
}catch(e){}
*/
/*
try{
r += '--'+window.CarObject.Test_ArrayOfInt32([1,2,3],[4,5,6]);
}catch(e){}
*/
/*
//--------OK--------
try{
var lo_car = window.CarObject.Test_Require('/data/elastos/JSTestObject.eco','CTestObject');
r += '--'+typeof(lo_car);
if(lo_car)r += '--'+typeof(lo_car.Test_String);
if(lo_car)r += '--'+lo_car.Test_String('Hello Car World!');
}catch(e){}
*/

/*
try{
var r = window.CarObject.test_20_BufferOf();
}catch(e){}

try{
var r = window.CarObject.test_21_MemoryOf();
}catch(e){}

try{
var r = window.CarObject.test_22_CppVector();
}catch(e){}

try{
var r = window.CarObject.test_23_Struct();
}catch(e){}

try{
var r = window.CarObject.test_24_Interface();
}catch(e){}
*/

/*
try{
var r = window.CarObject.test_25_Object_native_function();
}catch(e){}

try{
var r = window.CarObject.test_26_Object_js_function();
}catch(e){}

try{
var r = window.CarObject.test_27_Object_js_array();
}catch(e){}

try{
var r = window.CarObject.test_28_Object_js_object();
}catch(e){}

try{
var r = window.CarObject.test_29_Object_js_car();
}catch(e){}
*/

/*
try{
var r = window.CarObject.test_30_reserved();
}catch(e){}

try{
var r = window.CarObject.test_31_reserved();
}catch(e){}

try{
var r = window.CarObject.test_32_reserved();
}catch(e){}
*/

o.innerHTML = '<div style="color:red;">'+'CAR--value:'+r+'</div>';


