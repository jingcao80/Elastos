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

function showMethods(aMethodNames){
    var j=-1;
    var b=[];
    var name;
    for(var i=0,im=aMethodNames.length;i<im;i++){
      name = aMethodNames[i];
      //if(name.indexOf('Get')<0)continue;
      b.push(name);
      j++;
      if(j>18){
        oLog.Log('aMethodNames.'+(i-j)+'====: '+b.join('=='));
        j=-1;
        b=[];
      }
    }
    oLog.Log('aMethodNames.'+(i-j)+'====: '+b.join('=='));
    whmlog('==============ssss.3=================');
}

try {
    var sEcoName,sClassName;
    var oLog,oReflection;

    sEcoName = "/data/elastos/JSTestLog.eco";
    sClassName = "CTestLog";
    oLog = elastos[bNode?'require':'Require'](sEcoName, sClassName);

    sEcoName = "/data/elastos/JSTestReflection.eco";
    sClassName = "CTestReflection";
    oReflection = elastos.require(sEcoName, sClassName);

    //sEcoName = "/data/elastos/JSTestCarDataType.eco";
    //sClassName = "CTestCarDataType";

    //sEcoName = "/data/elastos/TextViewDemo.eco";
    //sClassName = "CNodeListener";

    sEcoName = "/data/elastos/JSCarRoot.eco";
    sClassName = "CCarRoot";
/*
    oRoot = elastos.require(sEcoName);
    whmlog('==============ssss.0.0================='+typeof(oModuleInfo_1));
    var oTestNew = new oReflection;
    whmlog('==============ssss.0.1================='+typeof(oModuleInfo_1));
    oModuleInfo_2 = elastos.require("/system/lib/Elastos.Core.eco","CStringWrapper","abcde");
    whmlog('==============ssss.0.2================='+typeof(oModuleInfo_1));
*/

    oCarRoot = elastos.require(sEcoName, sClassName);

    whmlog('==============ssss.1=================');
    var aMethodNames = oReflection.GetModuleClassMethodNames(sEcoName, sClassName);
    aMethodNames = aMethodNames.sort();
    whmlog('==============ssss.2=================');




    whmlog('==============ssss.9.0=================');

    sEcoName = "system/lib/Elastos.Droid.Core.eco";
    var oModuleInfo = oCarRoot.Test_Require_ModuleInfo(sEcoName);

    //var oTestCar = oCarRoot;
    var oTestCar = oModuleInfo;
    //var oTestCar = oModuleInfo_1;

    var c = [];
    for(var p in oTestCar) {
        c.push(p);
    }

    showMethods(c);

    var oClassInfos = oModuleInfo.GetAllClassInfos();
    whmlog('==============ssss.9.1.1=====classinfos============'+oClassInfos.length);
    whmlog('==============ssss.9.1.2=====classinfos[0].name============'+oClassInfos[0].GetName());

/*
    //sEcoName = "/system/lib/Elastos.Runtime.eco";
    //sEcoName = "/system/lib/Elastos.Core.eco";
    sEcoName = "/system/lib/Elastos.Droid.Core.eco";
    var oModuleInfo_1 = oCarRoot.Test_Require_ModuleInfo(sEcoName);
    var oClassInfos_1 = oModuleInfo_1.GetAllClassInfos();

    var bExist = false;
    var aClassNames = [];
    for(var i=0,im=oClassInfos_1.length;i<im;i++){
        var sName = oClassInfos_1[i].GetName();
        aClassNames.push(sName);
        if(sName=="CStringWrapper"){
            bExist = true;
            whmlog('==============ssss.9.1.2.0=====names============'+sName+'=='+aClassNames[i]);
            //break;
        }
    }
    aClassNames = aClassNames.sort();
    whmlog('==============ssss.9.1.2.1=====names============'+aClassNames);
    showMethods(aClassNames);
    whmlog('==============ssss.9.1.2.2=====exist============'+bExist);
*/

    //var oClassInfo = oModuleInfo.GetClassInfo("CTestClassInfo");
    var oClassInfo = oModuleInfo.GetClassInfo("CTextView");
    var aConstructorInfos = oClassInfo.GetAllConstructorInfos();
    for (var i=0,im=aConstructorInfos.length;i<im;i++){
        var constructorInfo = aConstructorInfos[i];
        var aParamInfos = constructorInfo.GetAllParamInfos();
        var aParamNames = [];
        for(var j=0,jm=aParamInfos.length;j<jm;j++){
            var paramInfo = aParamInfos[j];
            aParamNames.push(paramInfo.GetName());
        }
        whmlog('======'+i+'========ssss.9.1.3=====constructor------'+constructorInfo.GetName()+'['+aParamNames.join('/')+']');
    }

    whmlog('==============ssss.10.1=================');

    sEcoName = "/data/elastos/TextViewDemo.eco";
    sClassName = "CNodeListener";

    var oNodeListener = elastos.require(sEcoName, sClassName);

    var pEnqueueUIMessage = oNodeListener.GetEnqueueUIMessagePtr();
    elastos.SetEnqueueUIMessagePtr(pEnqueueUIMessage);
    var oActivity = oNodeListener.GetMainActivity();

    var oConstructorInfo = aConstructorInfos[0];
    var oArgumentList = oConstructorInfo.CreateArgumentList();
    oArgumentList.SetInputArgumentOfObjectPtr(0,oActivity);
    var oNewTextView = oConstructorInfo.CreateObject(oArgumentList);

    var oModuleInfo_elastos_core = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Core.eco");
    var oClassInfo_CStringWrapper = oModuleInfo_elastos_core.GetClassInfo("CStringWrapper");

    var aConstructorInfos_CStringWrapper = oClassInfo_CStringWrapper.GetAllConstructorInfos();
    var oConstructorInfo = aConstructorInfos_CStringWrapper[0];
    var oArgumentList = oConstructorInfo.CreateArgumentList();
    oArgumentList.SetInputArgumentOfString(0,'Hello NewTextView!!');
    var oCStringWrapper = oConstructorInfo.CreateObject(oArgumentList);

    whmlog('==============ssss.22================='+oCStringWrapper.ToString());

    oNewTextView.SetText(oCStringWrapper);
    var sText = oNewTextView.GetText().ToString();

    whmlog('==============ssss.23.1========TextView:'+sText+'=========');


return;


    //--------get car instance and methods--------begin--------

    var sText;

    var myTextViewId = 0x7f050007;
    var oTextView = oActivity.FindViewById(myTextViewId);
    var sText = oTextView.GetText().ToString();
    whmlog('==============ssss.4.1========TextView:'+sText+'=========');
    //oTestCar.SetText('xyz');

    var myEditTextId = 0x7f050008;
    var oEditText = oActivity.FindViewById(myEditTextId);
    sText = oEditText.GetText().ToString();
    whmlog('==============ssss.4.2========EditText:'+sText+'=========');

    var chkIosId = 0x7f050009;
    var oChkIos = oActivity.FindViewById(chkIosId);
    sText = oChkIos.GetText().ToString();
    whmlog('==============ssss.4.2========ChkIos:'+sText+'=========');

    var chkAndroidId = 0x7f05000a;
    var oChkAndroid = oActivity.FindViewById(chkAndroidId);
    var oCharSequence = oChkAndroid.GetText();
    sText = oCharSequence.ToString();
    whmlog('==============ssss.4.2========ChkAndroid:'+sText+'=========');

    var oWindowManager = oActivity.GetWindowManager();
    var oWindow = oActivity.GetWindow();
    var oIntent = oActivity.GetIntent();
    var oComponent = oIntent.GetComponent();
    whmlog('==============ssss.4.3========oComponent:'+oComponent.GetPackageName()+'=='+oComponent.GetClassName()+'=======');
    var oAction = oIntent.GetAction();
    whmlog('==============ssss.4.4========oAction:=='+oAction+'=======');

    //error:Only the original thread that created a view hierarchy can touch its views.
    oChkIos.SetText('ios_123567890');
    oChkIos.SetChecked(true);
    oChkAndroid.SetChecked(false);
    oChkIos.SetWidth(200);

//return;

    //--------crash--------
    //var oCategories = oIntent.GetCategories();
    //whmlog('==============ssss.4.5========oCategories:=='+oCategories+'=======');

    var oMainLooper = oActivity.GetMainLooper();
    var oThread = oMainLooper.GetThread();
    var oQueue = oMainLooper.GetQueue();

    var oLayoutInflater = oActivity.GetLayoutInflater();

    var oDecorView = oActivity.GetDecorView();

    var oLayout = oChkIos.GetLayout();
    var oRootView = oChkIos.GetRootView();

    //oTestCar = oActivity;  //CActivityOne
    //oTestCar = oWindowManager;  //no classname
    //oTestCar = oWindow;  //CPhoneWindow
    //oTestCar = oMainLooper;  //no classname
    //oTestCar = oThread;  //CThread
    //oTestCar = oQueue;  //CMessageQueue
    //oTestCar = oIntent;  //CIntent
    //oTestCar = oComponent;  //CComponentName
    //oTestCar = oChkIos;  //CCheckBox
    oTestCar = oCharSequence;  //CStringWrapper
    //oTestCar = oLayout;  //CStaticLayout
    //oTestCar = oRootView;  //undefined
    //oTestCar = oLayoutInflater;  //CPhoneLayoutInflater
    //oTestCar = oDecorView;  //no classname

    var sClassName = oReflection.GetClassName(oTestCar);
    whmlog('==============ssss.5.0========oTestCar ClassName:=='+sClassName+'=======');

    p++;
    a.push("step " + p + ": oReflection type: " + typeof(oReflection));
    p++;
    a.push("step " + p + ": oReflection.GetModuleInfo type: " + typeof(oReflection.GetModuleInfo));

    p++
    var b = ["============================================================"];
    var c = [];
    for (var prop in oTestCar){
        //if(prop.indexOf('Get')<0)continue;
        //if(prop.indexOf('At')<0)continue;
        c.push(prop);
    }
    c = c.sort();

    showMethods(c);

    for (var i=0,im=c.length;i<im;i++) {
	//if(c[i]!='SetChecked')continue;
	//if(c[i]!='GetHeight')continue;
	//if(c[i]!='GetText')continue;
	//if(c[i]!='GetWindowManager')continue;
	//if(c[i]!='GetWindow')continue;
	//if(c[i].indexOf('GetLayout')<0)continue;
	//if(c[i].indexOf('Post')<0)continue;
	//if(c[i].indexOf('GetRootView')<0)continue;
	//if(c[i].indexOf('SetText')<0)continue;
	//continue;

        b.push(c[i]);
        var oMethodProto = oReflection.GetMethodProto(oTestCar,c[i]);
        var s1 = JSON.stringify(oMethodProto);
        b.push(s1);

        oLog.Log(s1);
    }

    whmlog('==============ssss.6.0========GetMethodProto end=======');

    //--------get car instance and methods--------end--------

    b.push("============================================================");
    var aClassNames = oReflection.GetAllModuleClassNames('/system/lib/Elastos.Droid.Core.eco');
    var aClassNames = oReflection.GetAllModuleClassNames('/data/elastos/WebViewDemo.eco');
    //aClassNames = aClassNames.sort();
    b.push('classname sum: ' + aClassNames.length);
    //b.push(aClassNames.join('\n'));

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
    //for(var i=0,im=a.length;i<im;i++){
    //  oLog.Log('elastos reflection test result: '+s);
    //}
    oLog.Log('====================oLog.Log: test end================');
    whmlog('====================whmlog: test end================');
}
else {
    s = a.join("<br>");
    var oMain = document.getElementById("main");
    oMain && (oMain.innerHTML = s);
}

}    //fnTestReflection

ElastosTest.fnTestReflection(1);

//---------------------------end--------------------------------


