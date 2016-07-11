//module.exports = function (api) {
module.exports = function (aoElastos) {

var _Runtime_Native = aoElastos.Runtime_Native;

var CObject = aoElastos.CObject;

var _getModuleInfo = function (asEcoName) {
    return _Runtime_Native.Test_Require_ModuleInfo(asEcoName);
};

var CarDataType = {
    Int16       : 1,
    Int32       : 2,
    Int64       : 3,
    Byte        : 4,
    Float       : 5,
    Double      : 6,
    Char32      : 7,
    String      : 8,
    Boolean     : 9,
    EMuid       : 10,
    EGuid       : 11,
    ECode       : 12,
    LocalPtr    : 13,
    LocalType   : 14,
    Enum        : 15,
    ArrayOf    : 16,
    CppVector   : 17,
    Struct      : 18,
    IInterface   : 19
};

var CarToJsDataTypeMap = {
    boolean:[
        CarDataType.Boolean,
    ],
    number:[
        CarDataType.Int16,
        CarDataType.Int32,
        CarDataType.Int64,
        CarDataType.Byte,
        CarDataType.Float,
        CarDataType.Double,
        CarDataType.ECode,
        CarDataType.Enum,
    ],
    string:[
        //CarDataType.Char8,
        //CarDataType.Char16,
        CarDataType.Char32,
        //CarDataType.CString,
        CarDataType.String,
        //CarDataType.StringBuf,
    ],
    array:[
        CarDataType.ArrayOf,
        //CarDataType.BufferOf,
        //CarDataType.MemoryBuf,
        CarDataType.CppVector,
    ],
    object:[
        CarDataType.EMuid,
        CarDataType.EGuid,
        CarDataType.LocalPtr,
        CarDataType.LocalType,
        CarDataType.Struct,
        CarDataType.IInterface,
    ],
    //other:[
    //    CarDataType.LocalPtr,
    //    CarDataType.LocalType,
    //],
};

var ParamIOAttribute = {
        In : 0,
        CalleeAllocOut : 1,
        CallerAllocOut : 2,
}

function getDataTypeCarString(ai_datatype){
    var sRet = 'Not Defined DataType=='+ai_datatype+'==';

    for(var p in CarDataType) {
        if (CarDataType[p] == ai_datatype) {
            sRet = p;
            break;
        }
    }

    return sRet;
}

function getDataTypeJavascriptString(ai_datatype){
    var sRet = 'Not Defined DataType';

    var bFound = false;
    for(var p in CarToJsDataTypeMap) {
        var a = CarToJsDataTypeMap[p];
        for(var i = 0,im = a.length; i<im; i++) {
            if (a[i] == ai_datatype) {
                sRet = p;
                bFound = true;
                break;
            }
        }
        if (bFound) break;
    }

    return sRet;
}

function getCreateOnUIThread(as_ClassName) {
    var bRet = false;

    if (as_ClassName == "Elastos.Droid.Widget.CPopupWindow") {
        bRet = true;
    }

    if (as_ClassName == "Elastos.Droid.Webkit.CWebView") {
        bRet = true;
    }

    return bRet;
}

function getRunOnUIThread(as_MethodName) {
    var bRet = false;

    //TODO:
    if (as_MethodName == "???") {
        bRet = true;
    }

    return bRet;
}

//--------proto info begin--------
function __getSubDataTypeCarString(ao_TypeInfo){
    var a = [],s;

    var iDataType = ao_TypeInfo.GetDataType();
    switch (iDataType) {
        case CarDataType.IInterface:
            s = ao_TypeInfo.GetName();
            a.push(s);

            break;
        case CarDataType.LocalPtr:
            var oElementTypeInfo = ao_TypeInfo.GetTargetTypeInfo();
            var iElementDataType = oElementTypeInfo.GetDataType();
            s = getDataTypeCarString(iElementDataType);
            a.push(s);
            s = __getSubDataTypeCarString(oElementTypeInfo);
            a.push(s);

            break;
        case CarDataType.ArrayOf:
            var oElementTypeInfo = ao_TypeInfo.GetElementTypeInfo();
            var iElementDataType = oElementTypeInfo.GetDataType();
            s = getDataTypeCarString(iElementDataType);
            a.push(s);
            s = __getSubDataTypeCarString(oElementTypeInfo);
            a.push(s);

            break;
        default:
            break;
    }

    s = a.join("");
    if(s.length)s = "<" + s + ">";

    return s;
}
function __getCarDataTypeString(ao_TypeInfo){
    var a = [];

    var iDataType = ao_TypeInfo.GetDataType();
    var s;
    s = getDataTypeCarString(iDataType);
    a.push(s);
    s = __getSubDataTypeCarString(ao_TypeInfo);
    a.push(s);

    return a.join("");
}

//return: ParamProto=ParamName[]
function __getParamProto(ao_ParamInfo){
    var aProto = [];

    aProto.push(ao_ParamInfo.GetName());
    aProto.push("[");

    var oTypeInfo = ao_ParamInfo.GetTypeInfo();
    var s = __getCarDataTypeString(oTypeInfo);
    aProto.push(s);

    aProto.push("]");

    return aProto.join("");
}

//return: ConstructorProto=Constructor(ParamProtos);
function __getConstructorProto(ao_ConstructorInfo){
    var aProto = [];

    aProto.push(ao_ConstructorInfo.GetName());
    aProto.push("(");

    var len = ao_ConstructorInfo.GetParamCount();
    if (len > 0) {

        var bProto = [];
        var aParamInfos = ao_ConstructorInfo.GetAllParamInfos();
        for (var i=0,im=aParamInfos.length;i<im;i++) {
            var sParamProto = __getParamProto(aParamInfos[i]);
            bProto.push(sParamProto);
        }
        aProto.push(bProto.join(","));
    }

    aProto.push(")");

    return aProto.join("");
}

//return: ClassName{ConstructorProtos};
function _getConstructorProtos(ao_ClassInfo){
    elog("====__getConstructorProtos====begin");

    var aProto = [];

    aProto.push(ao_ClassInfo.GetName());
    aProto.push("{");

    var len = ao_ClassInfo.GetConstructorCount();

    var aConstructorInfos = ao_ClassInfo.GetAllConstructorInfos();
    for (var i=0,im=aConstructorInfos.length;i<im;i++) {
        aProto.push("=="+i+"==");
        var sConstructorProto = __getConstructorProto(aConstructorInfos[i]);
        aProto.push(sConstructorProto);
    }

    aProto.push("}");

    return aProto.join("");
}

function getConstructorProtos(as_EcoName, as_ClassName){
    var oModuleInfo = _getModuleInfo(as_EcoName);
    var oClassInfo = oModuleInfo.GetClassInfo(as_ClassName);
    _getConstructorProtos(oClassInfo);
}

function _getMethodInfos(as_EcoName, as_ClassName, as_MethodName){
    var oModuleInfo = _getModuleInfo(as_EcoName);
    var oClassInfo = oModuleInfo.GetClassInfo(as_ClassName);
    var aAllMethodInfos = oClassInfo.GetAllMethodInfos();
    var aMethodInfos;
    if (arguments.length == 2) {
        aMethodInfos = aAllMethodInfos;
    }
    else {
        aMethodInfos = [];
        for(var i=0,im=aAllMethodInfos.length;i<im;i++) {
            var oMethodInfo = aAllMethodInfos[i];
            var sMethodName = oMethodInfo.GetName();
            if (sMethodName == as_MethodName) {
                aMethodInfos.push(oMethodInfo);
            }
        }
    }
    return aMethodInfos;
}
function _getMethodProtos(as_EcoName, as_ClassName, as_MethodName){
    var aProto = [];

    aProto.push(as_ClassName);
    aProto.push("{");

    var aMethodInfos = _getMethodInfos(as_EcoName, as_ClassName, as_MethodName);
    for (var i=0,im=aMethodInfos.length;i<im;i++) {
        aProto.push("=="+i+"==");
        var sMethodProto = __getConstructorProto(aMethodInfos[i]);
        aProto.push(sMethodProto);
    }

    aProto.push("}");

    return aProto.join("");
}

CObject.getConstructorProtos = getConstructorProtos;
CObject.getMethodProtos = _getMethodProtos;

//--------proto info end--------

function classinfo__createObject(oModuleInfo,oClassInfo){
    var newObject;

elog("classinfo__createObject======00======");
    if(typeof(oModuleInfo)=='string') {
        oModuleInfo = _getModuleInfo(oModuleInfo);
    }

elog("classinfo__createObject======01======");
    var sClassName = "null";
    if(typeof(oClassInfo)=='string') {
        sClassName = oClassInfo;
        oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
        if (!oClassInfo) {
            elog("classinfo__createObject======can't find class: " + sClassName);
            return;
        }
        sClassName = oClassInfo.GetName();
    }
    else {
        sClassName = oClassInfo.GetName();
    }

elog("classinfo__createObject======02======");
    var bCreateOnUIThread = getCreateOnUIThread(sClassName);

elog("classinfo__createObject======03======");
    var length = arguments.length;
    if(length==2){
elog("classinfo__createObject======04.1======");
        if (bCreateOnUIThread) {
            newObject = oClassInfo.RemoteCreateObject();
        }
        else {
            newObject = oClassInfo.LocalCreateObject();
        }
    }
    else {
elog("classinfo__createObject======04.2======");
        var aConstructorInfos = oClassInfo.GetAllConstructorInfos();

        var oConstructorInfo;
        var paramCount = length - 2;
        var bSameArgs = false;
        for(var i=0, im=aConstructorInfos.length; i<im; i++){
elog("classinfo__createObject======05======");
            oConstructorInfo = aConstructorInfos[i];

            var _paramCount = oConstructorInfo.GetParamCount();

            bSameArgs = false;
            if (_paramCount == paramCount) {
                bSameArgs = true;
                var aParamInfos = oConstructorInfo.GetAllParamInfos();
                for(var j = 0, jm = paramCount; j<jm; j++) {
                    var paramInfo = aParamInfos[j];
                    var oTypeInfo = paramInfo.GetTypeInfo();

                    var iDataType = oTypeInfo.GetDataType();
                    var sJsDataType = getDataTypeJavascriptString(iDataType);

                    var arg_in = arguments[j+2];
                    var type_in = typeof(arg_in);

                    if (sJsDataType == type_in) {
                        if (iDataType == CarDataType.IInterface) {
                            var sInterfaceName = oTypeInfo.GetName();
                            bSameArgs = CObject.hasInterface(arg_in, sInterfaceName);
                        }
                        else if (iDataType == CarDataType.LocalPtr) {
                            //TODO:compare the localptr element type
                            //continue;

                            var oElementTypeInfo = oTypeInfo.GetTargetTypeInfo();
                            var iElementDataType = oElementTypeInfo.GetDataType();

                            elog('==============classinfo__createObject ======find method====LocalPtr====element type:'+iElementDataType);

                            switch (iElementDataType) {
                                case CarDataType.IInterface:
                                    //TODO:
                                    break;
                                case CarDataType.ArrayOf:
                                    //TODO:
                                    if (typeof arg_in.GetClassID == 'function') bSameArgs = false;
                                    break;
                                default:
                                    //TODO
                                    break;
                            }
                        }
                        else {
                            //continue;
                        }
                    }
                    else {
                        bSameArgs = false
                    }
                    if (!bSameArgs) break;
                }
                if (bSameArgs) break;
            }
elog("classinfo__createObject======06======");
        }

        if (!bSameArgs) {
            elog("====classinfo__createObject====can not find constructor : " + sClassName);
            return null;
        }
elog("classinfo__createObject======07======");

        var aParamInfos = oConstructorInfo.GetAllParamInfos();
        var oArgumentList = oConstructorInfo.CreateArgumentList();
elog("classinfo__createObject======08======");

        for(var i = 0, im = paramCount; i<im; i++) {
elog("classinfo__createObject======09======");
            var paramInfo = aParamInfos[i];
            var oTypeInfo = paramInfo.GetTypeInfo();

            var iDataType = oTypeInfo.GetDataType();
            var sJsDataType = getDataTypeJavascriptString(iDataType);

            var arg = arguments[i+2];
            switch (iDataType) {
                case CarDataType.Int16:
                    oArgumentList.SetInputArgumentOfInt16(i,arg);
                    break;
                case CarDataType.Int32:
                    oArgumentList.SetInputArgumentOfInt32(i,arg);
                    break;
                case CarDataType.Int64:
                    oArgumentList.SetInputArgumentOfInt64(i,arg);
                    break;
                case CarDataType.Byte:
                    oArgumentList.SetInputArgumentOfByte(i,arg);
                    break;
                case CarDataType.Float:
                    oArgumentList.SetInputArgumentOfFloat(i,arg);
                    break;
                case CarDataType.Double:
                    oArgumentList.SetInputArgumentOfDouble(i,arg);
                    break;
                //case CarDataType.Char8:
                //case CarDataType.Char16:
                case CarDataType.Char32:
                    //oArgumentList.SetInputArgumentOfChar(i,arg);
                    oArgumentList.SetInputArgumentOfString(0,String(arg));
                    break;
                case CarDataType.String:
                    oArgumentList.SetInputArgumentOfString(i,arg);
                    break;
                case CarDataType.Boolean:
                    oArgumentList.SetInputArgumentOfBoolean(i,arg);
                    break;
                //case CarDataType.EMuid:
                //case CarDataType.EGuid:
                case CarDataType.ECode:
                    oArgumentList.SetInputArgumentOfECode(i,arg);
                    break;
                //case CarDataType.LocalPtr:
                //case CarDataType.LocalType:
                case CarDataType.Enum:
                    oArgumentList.SetInputArgumentOfEnum(i,arg);
                    break;
                //case CarDataType.ArrayOf:
                //case CarDataType.StructPtr:
                case CarDataType.IInterface:
                    //oArgumentList.SetInputArgumentOfObjectPtr(i,arguments[i+2]);
                    oArgumentList.SetInputArgumentOfObjectPtr(i,arg);
                    break;
                case CarDataType.LocalPtr:
                    var oElementTypeInfo = oTypeInfo.GetTargetTypeInfo();
                    var iElementDataType = oElementTypeInfo.GetDataType();

                    //TODO:
                    switch (iElementDataType) {
                        case CarDataType.ArrayOf:
                            ////oArgumentList.SetInputArgumentOfLocalPtr(i,arguments[i+2]);
                            var tmp = [1,2,3];
                            oArgumentList.SetInputArgumentOfLocalPtr(tmp,i,arguments[i+2]);
                            break;
                        default:
                            ////oArgumentList.SetInputArgumentOfLocalPtr(i,arguments[i+2]);
                            var tmp = [1,2,3];
                            oArgumentList.SetInputArgumentOfLocalPtr(tmp,i,arguments[i+2]);
                            break;
                    }

                    break;
                default:
                    var sCarDataType = getDataTypeCarString(iDataType);
                    elog('==============classinfo__createObject ===========argument type: to be finished===========rc:'+ i +'======datatype:'+iDataType);
                    break;
            }
elog("classinfo__createObject======10======");
        }
elog("classinfo__createObject======11======");

        //var sAnnotation = oConstructorInfo.GetAnnotation();

        //newObject = oConstructorInfo.CreateObject(oArgumentList);
        if (bCreateOnUIThread) {
            newObject = oConstructorInfo.RemoteCreateObject(oArgumentList);
        }
        else {
            newObject = oConstructorInfo.LocalCreateObject(oArgumentList);
        }
elog("classinfo__createObject======12======");
    }
elog("classinfo__createObject======13======");

    return newObject;
}   //classinfo__createObject

return {
    getModuleInfo : _getModuleInfo,
    createObject : classinfo__createObject,
};

};  //exports