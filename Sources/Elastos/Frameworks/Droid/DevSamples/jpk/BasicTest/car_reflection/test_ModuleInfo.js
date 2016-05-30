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

    // CARAPI GetPath(
    //      [out]  String * pPath);
//TODO: oModuleInfo.path
var sPath = oModuleInfo.GetPath();
elog("========test reflection moduleInfo ========GetPath : " + sPath);

    // CARAPI GetVersion(
    //     /* [out] */ Int32 * pMajor,
    //     /* [out] */ Int32 * pMinor,
    //     /* [out] */ Int32 * pBuild,
    //     /* [out] */ Int32 * pRevision);
//TODO: oModuleInfo.version
//TOFIX: the names of the return values are not correct
var oVersion = oModuleInfo.GetVersion();
elog("========test reflection moduleInfo ========GetVersion : " + JSON.stringify(oVersion));

    // CARAPI GetClassCount(
    //     /* [out] */ Int32 * pCount);
//TODO: oModuleInfo.classCount
var iClassCount = oModuleInfo.GetClassCount();
elog("========test reflection moduleInfo ========GetClassCount : " + iClassCount);

if(false){
    // CARAPI GetAllClassInfos(
    //     /* [out, callee] */ ArrayOf<ITestClassInfo *> ** ppClassInfos);
//TODO: oModuleInfo.classInfos[i] / oModuleInfo.classInfos[sClassName]
var aClassInfos = oModuleInfo.GetAllClassInfos();
elog("========test reflection moduleInfo ========GetAllClassInfos : " + aClassInfos.length);
var oClassInfo;
//return "CCarRuntime" when i==0
for (var i=0,im=aClassInfos.length;i<im;i++) {
    oClassInfo = aClassInfos[i];
    var sClassName = oClassInfo.GetName();
    elog("========test reflection moduleInfo ========GetAllClassInfos===class " + i + " name : " + sClassName);
}
} //false

    // CARAPI GetClassInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestClassInfo ** ppClassInfo);
//TODO: oModuleInfo.sClassName / oModuleInfo[sClassName]
//var oClassInfo = oModuleInfo.GetClassInfo("CCarRuntime");
var oClassInfo = oModuleInfo.GetClassInfo("Elastos.DevSamples.Node.CCarRuntime");
var sClassName = oClassInfo.GetName();
elog("========test reflection moduleInfo ========GetClassInfo===class name : " + sClassName);

    // CARAPI GetInterfaceCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllInterfaceInfos(
    //     /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos);

    // CARAPI GetInterfaceInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo);

    // CARAPI GetStructCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllStructInfos(
    //     /* [out] */ ArrayOf<ITestStructInfo *> ** ppStructInfos);

    // CARAPI GetStructInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestStructInfo ** ppStructInfo);

    // CARAPI GetEnumCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllEnumInfos(
    //     /* [out] */ ArrayOf<ITestEnumInfo *> ** ppEnumInfos);

    // CARAPI GetEnumInfo(
    //     /* [in] */ const String& fullName,
    //     /* [out] */ ITestEnumInfo ** ppEnumInfo);

    // CARAPI GetTypeAliasCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllTypeAliasInfos(
    //     /* [out] */ ArrayOf<ITestTypeAliasInfo *> ** ppTypeAliasInfos);

    // CARAPI GetTypeAliasInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestTypeAliasInfo ** ppTypeAliasInfo);

    // CARAPI GetConstantCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllConstantInfos(
    //     /* [out] */ ArrayOf<ITestConstantInfo *> ** ppConstantInfos);

    // CARAPI GetConstantInfo(
    //     /* [in] */ const String& name,
    //     /* [out] */ ITestConstantInfo ** ppConstantInfo);

    // CARAPI GetImportModuleInfoCount(
    //     /* [out] */ Int32 * pCount);

    // CARAPI GetAllImportModuleInfos(
    //     /* [out] */ ArrayOf<ITestModuleInfo *> ** ppModuleInfos);

    // CARAPI constructor();

    // CARAPI constructor(
    //     /* [in] */ IModuleInfo * pModuleInfo);

elog("========test reflection moduleInfo ====end====");
