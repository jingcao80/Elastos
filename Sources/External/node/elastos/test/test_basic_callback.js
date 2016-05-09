var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

if (bNode) {
    root.elastos = require('./elastos.node');
}

ElastosTest.fnTestCallback = function(looptimes) {
var a = [];
var s = "";
var p = 0;

try {
    var sEcoName,sClassName;
    var oLog,oCallback;

    sEcoName = "/data/elastos/JSTestLog.eco";
    sClassName = "CTestLog";
    oLog = elastos[bNode?'require':'Require'](sEcoName, sClassName);;

    sEcoName = "/data/elastos/JSTestCallback.eco";
    sClassName = "CTestCallback";
    oCallback = elastos[bNode?'require':'Require'](sEcoName, sClassName);;

    s = "Begin test Callback";
    a.push(s);
    oLog.Log(s);

    var jsObj = (function(){
        function addLine(s){
            setTimeout(
                (function(){
                    return function(){
                        if (bNode) {
                            console.log(s);
                        }
                        else {
                            var oMain = document.getElementById("main");
                            oMain.innerHTML += "<br>" + s;
                        }
                    }
                })(),
                500
            )
        }

        return {
            OnEvent1:function(i) {
                var s = 'call jsObj.OnEvent1, i: ' + JSON.stringify(i);
                addLine(s);
                oLog.Log(s);
            },
            OnEvent2:function(s) {
                var s = 'call jsObj.OnEvent2, s: ' + s;
                addLine(s);
                oLog.Log(s);
            }
        }
    })();

    oLog.Log('call carObject.Test_AddEventListener');
    oCallback.Test_AddEventListener(jsObj);

    s = "End test Callback";
    a.push(s);
    oLog.Log(s);
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

}    //fnTestElastos

//-----------------Receive------------------
ElastosTest.fnTestCallback(1);

var assert = require('assert');
var called = false;

process.on('exit', function () {
  assert(called);
});

elastos.receive(5, function (err, val) {
  assert.equal(null, err);
  //assert.equal(10, val);
  process.nextTick(function () {
    called = true;
  });
});

//---------------------------end--------------------------------
