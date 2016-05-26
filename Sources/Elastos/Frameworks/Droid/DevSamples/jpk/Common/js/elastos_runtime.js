module.exports = function (api) {

elog('==========elasto_runtime.js begin.0===========');
elog('==========elasto_runtime.js begin.1===========' + typeof api.require);

//var _Runtime_Native = api.require("/data/elastos/" + "JSCarRoot.eco", "CCarRoot");
//var _Runtime_Native = api.require("/data/temp/node/bin/Elastos.DevSamples.Node.CarRuntime.eco", "CCarRuntime");

//var _Runtime_Native = api.require("/data/temp/node/bin/Elastos.DevSamples.Node.CarRuntime.eco", "CTestCarDataType");

//var sEcoName = "../../bin/Elastos.DevSamples.Node.CarRuntime.eco";
var sEcoName = "/data/temp/node/bin/Elastos.DevSamples.Node.CarRuntime.eco";
//var sClassName = "Elastos.DevSamples.Node.CarRuntime.CTestCarDataType";
var sClassName = "Elastos.DevSamples.Node.CarRuntime.CCarRuntime";

var _Runtime_Native = api.require(sEcoName, sClassName);

elog('==========elasto_runtime.js begin.2===========');

var _getModuleInfo = function (asEcoName) {
    return _Runtime_Native.Test_Require_ModuleInfo(asEcoName);
};

var CarDataType = {
        Int16 : 1,
        Int32 : 2,
        Int64 : 3,
        Byte : 4,
        Float : 5,
        Double : 6,
        Char8 : 7,
        Char16 : 8,
        Char32 : 9,
        CString : 10,
        String : 11,
        Boolean : 12,
        EMuid : 13,
        EGuid : 14,
        ECode : 15,
        LocalPtr : 16,
        LocalType : 17,
        Enum : 18,
        StringBuf : 19,
        ArrayOf : 20,
        BufferOf : 21,
        MemoryBuf : 22,
        CppVector : 23,
        Struct : 24,
        Interface : 25,
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
        CarDataType.Char8,
        CarDataType.Char16,
        CarDataType.Char32,
        CarDataType.CString,
        CarDataType.String,
        CarDataType.StringBuf,
    ],
    array:[
        CarDataType.ArrayOf,
        CarDataType.BufferOf,
        CarDataType.MemoryBuf,
        CarDataType.CppVector,
    ],
    object:[
        CarDataType.EMuid,
        CarDataType.EGuid,
        CarDataType.LocalPtr,
        CarDataType.LocalType,
        CarDataType.Struct,
        CarDataType.Interface,
    ]
};

var ParamIOAttribute = {
        In : 0,
        CalleeAllocOut : 1,
        CallerAllocOut : 2,
}

function getDataTypeCarString(ai_datatype){
    var sRet = 'Not Defined DataType';

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
        if (bFound) {
            break;
        }
    }

    return sRet;
}

function classinfo__createObject(oModuleInfo,oClassInfo){
    var newObject;

    if(typeof(oModuleInfo)=='string') {
        //oModuleInfo = Elastos.Runtime.getModuleInfo(oModuleInfo);
        oModuleInfo = _getModuleInfo(oModuleInfo);
    }
    if(typeof(oClassInfo)=='string') {
        oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
    }

    var length = arguments.length;
    if(length==2){
        newObject = oClassInfo.CreateObject();
    }
    else {
        var aConstructorInfos = oClassInfo.GetAllConstructorInfos();
        var oConstructorInfo;
        var paramCount = length - 2;
        for(var i=0, im=aConstructorInfos.length; i<im; i++){
            oConstructorInfo = aConstructorInfos[i];
            var _paramCount = oConstructorInfo.GetParamCount();
            if (_paramCount == paramCount) {
                var bSameArgs = true;
                var aParamInfos = oConstructorInfo.GetAllParamInfos();
                for(var j = 0, jm = paramCount; j<jm; j++) {
                    var paramInfo = aParamInfos[j];
                    var oTypeInfo = paramInfo.GetTypeInfo();

                    var iDataType = oTypeInfo.GetDataType();
                    var sJsDataType = getDataTypeJavascriptString(iDataType);

                    var arg_in = arguments[j+2];
                    var type_in = typeof(arg_in);
                    if (sJsDataType == type_in) continue;

                    bSameArgs = false
                    break;
                }
                if (bSameArgs) break;
            }
        }

        var bFoundConstructor = (i < im);
        if (!bFoundConstructor) {
            elog('====classinfo__createObject====can not find constructor ==============');
            return null;
        }

        var aParamInfos = oConstructorInfo.GetAllParamInfos();
        var oArgumentList = oConstructorInfo.CreateArgumentList();

        for(var i = 0, im = paramCount; i<im; i++) {
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
                case CarDataType.Char:
                    oArgumentList.SetInputArgumentOfChar(i,arg);
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
                //case CarDataType.CarArray:
                //case CarDataType.StructPtr:
                case CarDataType.Interface:
                    oArgumentList.SetInputArgumentOfObjectPtr(i,arguments[i+2]);
                    break;
                case CarDataType.LocalPtr:
                    //to be finished
                    elog('==============classinfo__createObject ===========LocalPtr====begin====');
                    //oArgumentList.SetInputArgumentOfLocalPtr(i,arguments[i+2]);
                    var tmp = [1,2,3];
                    oArgumentList.SetInputArgumentOfLocalPtr(tmp,i,arguments[i+2]);
                    elog('==============classinfo__createObject ===========LocalPtr====end====');
                    break;
                default:
                    var sCarDataType = getDataTypeCarString(iDataType);
                    elog('==============argument type: to be finished===========rc:'+ i +'======datatype:'+iDataType);
                    break;
            }
        }

        elog('==============oConstructorInfo.GetAnnotation===========type: ' + typeof(oConstructorInfo.GetAnnotation));
        var aa=[];
        for (var p in oConstructorInfo)aa.push(p);
        elog('==============oConstructorInfo.GetAnnotation===========methods list: ' + aa.join("--"));
        var sAnnotation = oConstructorInfo.GetAnnotation();
        elog('==============oConstructorInfo.GetAnnotation===========value: ' + sAnnotation);

        newObject = oConstructorInfo.CreateObject(oArgumentList);
    }
    return newObject;
}   //classinfo__createObject

return {
    getModuleInfo : _getModuleInfo,
    createObject : classinfo__createObject,
};

};  //exports
