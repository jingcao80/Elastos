var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

/*
if (bNode) {
    root.elastos = require('./elastos.node');
}
*/

ElastosTest.fnTestReflection = function(looptimes) {

var a = [];
var s = "";
var p = 0;

try {
    var sEcoName = "/data/elastos/JSTestLog.eco";
    var sClassName = "CTestLog";

    //var oLog = elastos.Require(sEcoName, sClassName);
    var oLog = elastos.require(sEcoName, sClassName);

    p++;
    a.push("step " + p + ": oLog type: " + typeof(oLog));
    p++;
    a.push("step " + p + ": oLog.Log type: " + typeof(oLog.Log));

    for (var i = 0; i < looptimes; i++) {
        p++;
        s = "step " + p + ": ================line: " + i + "================";
        a.push(s);
        oLog.Log(s);
    }
    a.push("all the tests are OK!");
} catch (e) {
    a.push("test error at p = " + p);
}

if (bNode) {
    s = a.join("\n");
    console.log('elastos.require = ',s);
}
else {
    s = a.join("<br>");
    var oMain = document.getElementById("main");
    oMain && (oMain.innerHTML = s);
}

}    //fnTestReflection

ElastosTest.fnTestReflection(5);

//---------------------------end--------------------------------

