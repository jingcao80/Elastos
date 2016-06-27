module.exports = function (api) {

var sEcoName = "/data/temp/node/bin/Elastos.DevSamples.Node.CarRuntime.eco";
var sClassName = "Elastos.DevSamples.Node.CarRuntime.CCarRuntime";
var _Runtime_Native = api.require(sEcoName, sClassName);

function _getObjClassInfo(ao_car) {
    return _Runtime_Native.Test_CObject_ReflectClassInfo(ao_car);
}

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
function getSubDataTypeCarString(ao_TypeInfo){
    //elog("====getSubDataTypeCarString====begin");
    var a = [],s;

    var iDataType = ao_TypeInfo.GetDataType();
    switch (iDataType) {
        case CarDataType.IInterface:
            s = "<"+ao_TypeInfo.GetName()+">";
            a.push(s);

            break;
        case CarDataType.LocalPtr:
            var oElementTypeInfo = ao_TypeInfo.GetTargetTypeInfo();
            var iElementDataType = oElementTypeInfo.GetDataType();
            s = getDataTypeCarString(iElementDataType);
            a.push(s);
            //s = "<"+oElementTypeInfo.GetName()+">";
            //a.push(s);
            s = getSubDataTypeCarString(oElementTypeInfo);
            a.push(s);

            break;
        case CarDataType.ArrayOf:
            var oElementTypeInfo = ao_TypeInfo.GetElementTypeInfo();
            var iElementDataType = oElementTypeInfo.GetDataType();
            s = getDataTypeCarString(iElementDataType);
            a.push(s);
            //s = "<"+oElementTypeInfo.GetName()+">";
            //a.push(s);
            s = getSubDataTypeCarString(oElementTypeInfo);
            a.push(s);

            break;
        default:
            break;
    }

    s = a.join("");
    if(s.length)s = "<" + s + ">";

    //elog("====getSubDataTypeCarString====end");
    return s;
}
function __getCarDataTypeString(ao_TypeInfo){
    //elog("====__getCarDataTypeString====begin");
    var a = [];

    var iDataType = ao_TypeInfo.GetDataType();
    var s;
    s = getDataTypeCarString(iDataType);
    a.push(s);
    //s = "<"+ao_TypeInfo.GetName()+">";
    //a.push(s);
    s = getSubDataTypeCarString(ao_TypeInfo);
    a.push(s);

    //elog("====__getCarDataTypeString====end");
    return a.join("");
}

//return: ParamProto=ParamName[]
function getParamProto(ao_ParamInfo){
    //elog("====getParamProto====begin");
    var aProto = [];

    aProto.push(ao_ParamInfo.GetName());
    aProto.push("[");

    var oTypeInfo = ao_ParamInfo.GetTypeInfo();
    var s = __getCarDataTypeString(oTypeInfo);
    aProto.push(s);

    aProto.push("]");

    //elog("====getParamProto====end");
    return aProto.join("");
}

//return: ConstructorProto=Constructor(ParamProtos);
function getConstructorProto(ao_ConstructorInfo){
    //elog("====getConstructorProto====begin");
    var aProto = [];

    aProto.push(ao_ConstructorInfo.GetName());
    aProto.push("(");

    var len = ao_ConstructorInfo.GetParamCount();
    elog("====getConstructorProto====GetParamCount:"+len);

if (len > 0) {

    var bProto = [];
    var aParamInfos = ao_ConstructorInfo.GetAllParamInfos();
    elog("====getConstructorProto====GetAllParamInfos:"+aParamInfos.length);
    for (var i=0,im=aParamInfos.length;i<im;i++) {
        var sParamProto = getParamProto(aParamInfos[i]);
        bProto.push(sParamProto);
    }
    aProto.push(bProto.join(","));
}

    aProto.push(")");

    //elog("====getConstructorProto====end");
    return aProto.join("");
}

//return: ClassName{ConstructorProtos};
function getConstructorProtos(ao_ClassInfo){
    elog("====getConstructorProtos====begin");

    var aProto = [];

    aProto.push(ao_ClassInfo.GetName());
    aProto.push("{");

    var len = ao_ClassInfo.GetConstructorCount();
    elog("====getConstructorProtos====GetConstructorCount:"+len);

    var aConstructorInfos = ao_ClassInfo.GetAllConstructorInfos();
    elog("====getConstructorProtos====GetAllConstructorInfos:"+aConstructorInfos.length);
    for (var i=0,im=aConstructorInfos.length;i<im;i++) {
        aProto.push("=="+i+"==");
        elog("====getConstructorProto=="+i+"==begin");
        var sConstructorProto = getConstructorProto(aConstructorInfos[i]);
        elog("====getConstructorProto=="+i+"==end");
        aProto.push(sConstructorProto);
    }

    aProto.push("}");

    elog("====getConstructorProtos====end");
    return aProto.join("");
}

function showMethods(ao) {
    var a = [];
    for (var p in ao) a.push(p);
    var s = "====methods====[" + a.join("][") + "]";
    elog(s);
}

//--------proto info end--------

function classinfo__createObject(oModuleInfo,oClassInfo){
    var newObject;

    if(typeof(oModuleInfo)=='string') {
        oModuleInfo = _getModuleInfo(oModuleInfo);
    }

    var sClassName = "null";
    if(typeof(oClassInfo)=='string') {
        sClassName = oClassInfo;
        elog('====classinfo__createObject====begin==0.0==ClassName:'+sClassName);
        oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
        elog('====classinfo__createObject====begin==0.1==ClassName:'+sClassName);
        sClassName = oClassInfo.GetName();
        elog('====classinfo__createObject====begin==0.2==ClassName:'+sClassName);
    }
    else {
        sClassName = oClassInfo.GetName();
        elog('====classinfo__createObject====begin==0.3==ClassName:'+sClassName);
    }

    elog("====sConstructorProtos====begin====");
    var sConstructorProtos = getConstructorProtos(oClassInfo);
    elog("====sConstructorProtos====:" + sConstructorProtos);
    elog("====sConstructorProtos====end====");

    var bCreateOnUIThread = getCreateOnUIThread(sClassName);

    var length = arguments.length;
    if(length==2){
        if (bCreateOnUIThread) {
            elog('====classinfo__createObject========oClassInfo.RemoteCreateObject');
            newObject = oClassInfo.RemoteCreateObject();
        }
        else {
            elog('====classinfo__createObject========oClassInfo.LocalCreateObject');
            newObject = oClassInfo.LocalCreateObject();
        }
    }
    else {
        var aConstructorInfos = oClassInfo.GetAllConstructorInfos();

        elog('====classinfo__createObject====ConstructorInfos total num:' + aConstructorInfos.length);

        var oConstructorInfo;
        var paramCount = length - 2;  //RDK4.2.2
        //var paramCount = length - 1;
        var bSameArgs = false;
        for(var i=0, im=aConstructorInfos.length; i<im; i++){
            oConstructorInfo = aConstructorInfos[i];

            var _paramCount = oConstructorInfo.GetParamCount();

            elog('====classinfo__createObject===='+i+' _paramCount:'+_paramCount+'===paramCount:'+paramCount+'===========');

            bSameArgs = false;
            if (_paramCount == paramCount) {
                bSameArgs = true;
                var aParamInfos = oConstructorInfo.GetAllParamInfos();
                for(var j = 0, jm = paramCount; j<jm; j++) {  //RDK4.2.2
                //for(var j = 0, jm = paramCount -1; j<jm; j++) {
                    var paramInfo = aParamInfos[j];
                    var oTypeInfo = paramInfo.GetTypeInfo();

                    var iDataType = oTypeInfo.GetDataType();
                    var sJsDataType = getDataTypeJavascriptString(iDataType);

                    var arg_in = arguments[j+2];
                    var type_in = typeof(arg_in);

                    elog("====classinfo__createObject====param:"+j+" js type:"+sJsDataType+" carType:"+type_in+" carDataType:"+iDataType);

                    if (sJsDataType == type_in) {
                        if (iDataType == CarDataType.IInterface) {
                            //TODO:compare the interface name
                            //continue;
                            var sFromClassName = arg_in.ToString();
                            var sToClassName = oTypeInfo.GetName();
                            elog('==============classinfo__createObject ======find method====Interface====paraClass:'+sFromClassName+"=="+sToClassName);

                            var oArgClassInfo = _getObjClassInfo(arg_in);
                            var oArgClassName = oArgClassInfo.GetName();
                            elog('==============classinfo__createObject ======find method====Interface====paraClassName:'+oArgClassName);
                            oArgInterfaces = oArgClassInfo.GetAllInterfaceInfos();
                            //var aa = [];
                            //elog('==============classinfo__createObject ======find method====Interface====paraInterfacesNum:'+oArgInterfaces.length);
                            var hasInterface = false;
                            for (var k=0,km=oArgInterfaces.length;k<km;k++){
                                var sArgInterfaceName = oArgInterfaces[k].GetName();
                                if (sArgInterfaceName == sToClassName) {
                                    hasInterface = true;
                                    break;
                                }
                                //elog('==============classinfo__createObject ======find method====Interface====paraInterfaceName:'+k+":"+s);
                                //aa.push(s);
                            }
                            bSameArgs = hasInterface;
                            //elog('==============classinfo__createObject ======find method====Interface====paraInterfaces:['+aa.join("][")+"]");
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
                                    //showMethods(arg_in);
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
        }

        if (!bSameArgs) {
            elog("====classinfo__createObject====can not find constructor : " + sClassName);
            return null;
        }
        else {
            elog("====classinfo__createObject====find constructor : " + sClassName);
        }

        var aParamInfos = oConstructorInfo.GetAllParamInfos();
        var oArgumentList = oConstructorInfo.CreateArgumentList();

        for(var i = 0, im = paramCount; i<im; i++) {  //RDK4.2.2
        //for(var i = 0, im = paramCount - 1; i<im; i++) {
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
        }

        //var sAnnotation = oConstructorInfo.GetAnnotation();

        //newObject = oConstructorInfo.CreateObject(oArgumentList);
        if (bCreateOnUIThread) {
            newObject = oConstructorInfo.RemoteCreateObject(oArgumentList);
        }
        else {
            newObject = oConstructorInfo.LocalCreateObject(oArgumentList);
        }
    }

    return newObject;
}   //classinfo__createObject

return {
    getModuleInfo : _getModuleInfo,
    createObject : classinfo__createObject,
};

};  //exports