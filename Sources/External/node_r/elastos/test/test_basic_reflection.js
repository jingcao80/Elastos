var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

if (bNode) {
    root.elastos = require('./elastos.node');
}

ElastosTest.fnTestReflection = function(looptimes) {

var a = [];
var s = "";
var p = 0;

try {
    var sEcoName,sClassName;
    var oLog,oReflection;

    sEcoName = "/data/elastos/JSTestLog.eco";
    sClassName = "CTestLog";
    oLog = elastos[bNode?'require':'Require'](sEcoName, sClassName);;

    sEcoName = "/data/elastos/JSTestReflection.eco";
    sClassName = "CTestReflection";
    //var elastos = require('./elastos.node');
    oReflection = elastos.require(sEcoName, sClassName);

    sEcoName = "/data/elastos/JSElastosRoot.eco";
    sClassName = "CElastosRoot";

    var oTestCar = elastos.require(sEcoName, sClassName);

    p++;
    a.push("step " + p + ": oReflection type: " + typeof(oReflection));
    p++;
    a.push("step " + p + ": oReflection.GetModuleInfo type: " + typeof(oReflection.GetModuleInfo));

    p++
    var b = ["============================================================"];
    for (var prop in oTestCar){
        //b.push(prop);

        var oMethodProto = oReflection.GetMethodProto(oTestCar,prop);
        b.push(JSON.stringify(oMethodProto));

    }

    b.push("============================================================");
    //var aClassNames = oReflection.GetAllModuleClassNames('/system/lib/Elastos.Droid.Core.eco');
    var aClassNames = oReflection.GetAllModuleClassNames('/data/elastos/WebViewDemo.eco');
    //var b.push("==============="+aClassNames+"===============");
    aClassNames = aClassNames.sort();
    b.push('classname sum: ' + aClassNames.length);
    b.push(aClassNames.join('\n'));

    a.push(b.join("\n===="));

/*
    for (var i = 0; i < looptimes; i++) {
        p++;
        s = "step " + p + ": ================line: " + i + "================";
        a.push(s);
        oLog.Log(s);
    }
*/
    a.push("all the tests are OK!");
} catch (e) {
    a.push("test error at p = " + p);
}

if (bNode) {
    s = a.join("\n");
    console.log('elastos.require = ',s);
    oLog.Log('elastos reflection test result: '+s);
}
else {
    s = a.join("<br>");
    var oMain = document.getElementById("main");
    oMain && (oMain.innerHTML = s);
}

}    //fnTestReflection

ElastosTest.fnTestReflection(5);

//---------------------------end--------------------------------

