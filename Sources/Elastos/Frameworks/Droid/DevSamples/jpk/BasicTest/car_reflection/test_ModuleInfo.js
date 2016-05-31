elog("========test reflection moduleInfo ====begin====");

var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

if (bNode) {
    root.elastos = require("../../bin/elastos.node");
}

elog("========test reflection moduleInfo ========type elastos : " + typeof elastos);

var sEcoName = "../../bin/Elastos.DevSamples.Node.CarRuntime.eco";
var sClassName = "Elastos.DevSamples.Node.CarRuntime.CCarRuntime";
var oCarRuntime = elastos.require(sEcoName, sClassName);

elog("========test reflection moduleInfo ========typeof oCarRuntime : " + typeof oCarRuntime);

var api = {};

api.getModuleInfo = function (asEcoName) {
    return oCarRuntime.Test_Require_ModuleInfo(asEcoName);
};

var oModuleInfo = api.getModuleInfo("../../bin/Elastos.DevSamples.Node.CarRuntime.eco");

elog("========test reflection moduleInfo ========typeof oModuleInfo : " + typeof oModuleInfo);

if(false){  //TEST OK
    // CARAPI GetPath(
    //      [out]  String * pPath);
//TODO: oModuleInfo.path
var sPath = oModuleInfo.GetPath();
elog("========test reflection moduleInfo ========GetPath : " + sPath);
} //false

if(false){  //TEST BUG
    // CARAPI GetVersion(
    //     /* [out] */ Int32 * pMajor,
    //     /* [out] */ Int32 * pMinor,
    //     /* [out] */ Int32 * pBuild,
    //     /* [out] */ Int32 * pRevision);
//TODO: oModuleInfo.version
//TOFIX: the names of the return values are not correct
var oVersion = oModuleInfo.GetVersion();
elog("========test reflection moduleInfo ========GetVersion : " + JSON.stringify(oVersion));
} //false

if(false){  //TEST OK
    // CARAPI GetClassCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.classCount
var iClassCount = oModuleInfo.GetClassCount();
elog("========test reflection moduleInfo ========GetClassCount : " + iClassCount);
} //false

if(false){  //TEST OK
    // CARAPI GetAllClassInfos(
    //     /* [out, callee] */ ArrayOf<ITestClassInfo *> ** ppClassInfos);
//TOFIX: ClassInfo::GetName only return class shortname currently, it should return class fullname.
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
var aClassInfos = oModuleInfo.GetAllClassInfos();
elog("========test reflection moduleInfo ========GetAllClassInfos====length : " + aClassInfos.length);
var oClassInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aClassInfos.length;i<im;i++) {
    oClassInfo = aClassInfos[i];
    var sClassName = oClassInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllClassInfos===class " + i + " name : " + sClassName);
}
} //false

if(false){  //TEST OK
    // CARAPI GetClassInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestClassInfo ** ppClassInfo);
//TOFIX: short classname should be supported
//TODO: oModuleInfo.sClassName / oModuleInfo[sClassName]
var oClassInfo = oModuleInfo.GetClassInfo("Elastos.DevSamples.Node.CarRuntime.CCarRuntime");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetClassInfo===class name : " + sClassName);
}   //false

if(true){  //TEST OK
    // CARAPI GetInterfaceCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.interfaceCount
var iInterfaceCount = oModuleInfo.GetInterfaceCount();
elog("========test reflection moduleInfo ========GetInterfaceCount : " + iInterfaceCount);
}   //false

if(true){  //TEST ERROR
    // CARAPI GetAllInterfaceInfos(
    //     /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos);
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
//never be used in framework source code currently
var aInterfaceInfos = oModuleInfo.GetAllInterfaceInfos();
elog("========test reflection moduleInfo ========GetAllInterfaceInfos====length : " + aInterfaceInfos.length);
var oInterfaceInfo;
//return "CCarRuntime" when i==0
for (var i=0,im=aInterfaceInfos.length;i<im;i++) {
    oInterfaceInfo = aInterfaceInfos[i];
    var sInterfaceName = oInterfaceInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllInterfaceInfos====" + i + " name : " + sInterfaceName);
    var sInterfaceNamespace = oInterfaceInfo.GetNamespace();
    elog("========test reflection moduleInfo ========GetAllInterfaceInfos====" + i + " namespace : " + sInterfaceNamespace);
}
} //false

if(true){  //TEST ERROR
    // CARAPI GetInterfaceInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo);
//TODO: oModuleInfo.sInterfaceName / oModuleInfo[sInterfaceName]
var oInterfaceInfo = oModuleInfo.GetInterfaceInfo("Elastos.DevSamples.Node.CarRuntime.ICarRuntime");
var sInterfaceName = oInterfaceInfo.GetName();
elog("========test reflection moduleInfo ========GetInterfaceInfo===name : " + sInterfaceName);
var sInterfaceNamespace = oInterfaceInfo.GetNamespace();
elog("========test reflection moduleInfo ========GetInterfaceInfo===namespace : " + sInterfaceNamespace);
}   //false

if(false){  //TEST OK
    // CARAPI GetStructCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.structCount
var iStructCount = oModuleInfo.GetStructCount();
elog("========test reflection moduleInfo ========GetStructCount : " + iStructCount);
}   //false

if(false){  //TEST TEST
    // CARAPI GetAllStructInfos(
    //     /* [out] */ ArrayOf<ITestStructInfo *> ** ppStructInfos);
//TODO: oModuleInfo.structInfos[i] / oModuleInfo.structInfos[sClassName]
var aStructInfos = oModuleInfo.GetAllStructInfos();
elog("========test reflection moduleInfo ========GetAllStructInfos====length : " + aStructInfos.length);
var oStructInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aStructInfos.length;i<im;i++) {
    oStructInfo = aStructInfos[i];
    var sStructName = oStructInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllStructInfos===name " + i + " name : " + sStructName);
}
} //false

if(false){  //TEST TEST
    // CARAPI GetStructInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestStructInfo ** ppStructInfo);
//TODO: oModuleInfo.sStructName / oModuleInfo[sStructName]
var oStructInfo = oModuleInfo.GetClassInfo("Elastos.DevSamples.Node.CarRuntime.CCarRuntime");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetClassInfo===class name : " + sClassName);
}   //false

if(false){  //TEST OK
    // CARAPI GetEnumCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.enumCount
var iEnumCount = oModuleInfo.GetEnumCount();
elog("========test reflection moduleInfo ========GetEnumCount : " + iEnumCount);
}   //false

if(false){  //TEST TEST
    // CARAPI GetAllEnumInfos(
    //     /* [out] */ ArrayOf<ITestEnumInfo *> ** ppEnumInfos);
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
var aClassInfos = oModuleInfo.GetAllClassInfos();
elog("========test reflection moduleInfo ========GetAllClassInfos====length : " + aClassInfos.length);
var oClassInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aClassInfos.length;i<im;i++) {
    oClassInfo = aClassInfos[i];
    var sClassName = oClassInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllClassInfos===class " + i + " name : " + sClassName);
}
} //false

if(false){  //TEST TEST
    // CARAPI GetEnumInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestEnumInfo ** ppEnumInfo);
//TODO: oModuleInfo.sClassName / oModuleInfo[sClassName]
var oClassInfo = oModuleInfo.GetClassInfo("Elastos.DevSamples.Node.CarRuntime.CCarRuntime");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetClassInfo===class name : " + sClassName);
}   //false

if(false){  //TEST OK
    // CARAPI GetTypeAliasCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.typeAliasCount
var iTypeAliasCount = oModuleInfo.GetTypeAliasCount();
elog("========test reflection moduleInfo ========GetTypeAliasCount : " + iTypeAliasCount);
}   //false

if(false){  //TEST TEST
    // CARAPI GetAllTypeAliasInfos(
    //     /* [out] */ ArrayOf<ITestTypeAliasInfo *> ** ppTypeAliasInfos);
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
var aClassInfos = oModuleInfo.GetAllClassInfos();
elog("========test reflection moduleInfo ========GetAllClassInfos====length : " + aClassInfos.length);
var oClassInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aClassInfos.length;i<im;i++) {
    oClassInfo = aClassInfos[i];
    var sClassName = oClassInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllClassInfos===class " + i + " name : " + sClassName);
}
} //false

if(false){  //TEST TEST
    // CARAPI GetTypeAliasInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestTypeAliasInfo ** ppTypeAliasInfo);
//TODO: oModuleInfo.sClassName / oModuleInfo[sClassName]
var oClassInfo = oModuleInfo.GetClassInfo("Elastos.DevSamples.Node.CarRuntime.CCarRuntime");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetClassInfo===class name : " + sClassName);
}   //false

if(false){  //TEST OK
    // CARAPI GetConstantCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.constantCount
var iConstantCount = oModuleInfo.GetConstantCount();
elog("========test reflection moduleInfo ========GetConstantCount : " + iConstantCount);
}   //false

if(false){  //TEST TEST
    // CARAPI GetAllConstantInfos(
    //     /* [out] */ ArrayOf<ITestConstantInfo *> ** ppConstantInfos);
//TODO: oModuleInfo.constantInfos[i] / oModuleInfo.constantInfos[sClassName]
var aConstantInfos = oModuleInfo.GetAllConstantInfos();
elog("========test reflection moduleInfo ========GetAllConstantInfos====length : " + aConstantInfos.length);
var oConstantInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aConstantInfos.length;i<im;i++) {
    oConstantInfo = aConstantInfos[i];
    var sConstantName = oConstantInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllConstantInfos===" + i + " name : " + sConstantName);
}
} //false

if(false){  //TEST TEST
    // CARAPI GetConstantInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestConstantInfo ** ppConstantInfo);
//TODO: oModuleInfo.sClassName / oModuleInfo[sClassName]
var oConstantInfo = oModuleInfo.GetConstantInfo("Elastos.DevSamples.Node.CarRuntime.CConstantInfo");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetConstantInfo===class name : " + sConstantName);
}   //false

if(false){  //TEST OK
    // CARAPI GetImportModuleInfoCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.importModuleInfoCount
var iImportModuleInfoCount = oModuleInfo.GetImportModuleInfoCount();
elog("========test reflection moduleInfo ========GetImportModuleInfoCount : " + iImportModuleInfoCount);
}   //false

if(false){  //TEST TEST
    // CARAPI GetAllImportModuleInfos(
    //     /* [out] */ ArrayOf<ITestModuleInfo *> ** ppModuleInfos);
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
var aImportModuleInfos = oModuleInfo.GetAllImportModuleInfos();
elog("========test reflection moduleInfo ========GetAllClassInfos====length : " + aClassInfos.length);
var oImportModuleInfo;
//return "CCarRuntime" when i==0, just the short class name
for (var i=0,im=aImportModuleInfos.length;i<im;i++) {
    oImportModuleInfo = aImportModuleInfos[i];
    var sImportModuleName = oImportModuleInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllImportModuleInfos===" + i + " name : " + sClassName);
}
} //false

    // CARAPI constructor();

    // CARAPI constructor(
    //     /* [in] */ IModuleInfo * pModuleInfo);

elog("========test reflection moduleInfo ====end====");
