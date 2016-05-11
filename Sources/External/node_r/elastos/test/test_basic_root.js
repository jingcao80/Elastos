var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

if (bNode) {
    root.elastos = require('./elastos.node');
}

ElastosTest.fnTestElastosRoot = function(looptimes) {

var a = [];
var s = "";
var p = 0;

try {
    p++;
    a.push("step " + p + ": elastos type: " + typeof(elastos));
    p++;
    a.push("step " + p + ": elastos.Require type: " + typeof(elastos.Require));

    var sEcoName = "/data/elastos/JSElastosRoot.eco";
    var sClassName = "CElastosRoot";

    p++;
    var oCar;
    if (bNode) {
        oCar = elastos.require(sEcoName, sClassName);
    }
    else {
        oCar = elastos.Require(sEcoName, sClassName);
    }

    for (var i = 0; i < looptimes; i++) {
        a.push("step " + p + ": oCar type: " + typeof(oCar));
        p++;

        a.push("step " + p + ": oCar.Require type:" + typeof(oCar.Require));
        p++;

        //oCar = oCar.Require(sEcoName, sClassName);
        var oCar = oCar.Require(sEcoName, sClassName);
        a.push("step " + p + ": oCar_1.Require type:" + typeof(oCar.Require));
        p++;
    }

    a.push("all the tests are OK!");

} catch (e) {
    a.push("test error at p = " + p + ' : ' + e);
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

}    //fnTestElastosRoot

ElastosTest.fnTestElastosRoot(5);

//---------------------------end--------------------------------
