function fnclick_reflection(e){
	var a=[];
	var s='';
	try{
		//var lo_car = window.CarObject.Test_Require('/data/elastos/JSTestObject.eco','CTestObject');
		//var lo_car = window.CarObject.Test_Require('/data/elastos/JSTestObject.eco','CTestModuleInfo');
		//var lo_car = window.CarObject.Test_Require('/system/lib/Elastos.Core.eco','CSystem');
		//var lo_car = window.CarObject.Test_Require_ClassInfo('/system/lib/Elastos.Runtime.eco','CArgumentList');
		//var lo_car = window.CarObject.Test_Require_ModuleInfo('/system/lib/Elastos.Core.eco','CSystem');
		var la_eco = [
			'/system/lib/Elastos.Core.eco',						//class:806
			'/system/lib/Elastos.Droid.Commands.Am.eco',		//class:8
			'/system/lib/Elastos.Droid.Commands.Monkey.eco',	//class:56
			'/system/lib/Elastos.Droid.Core.eco',				//class:2878
			'/system/lib/Elastos.Droid.JavaProxy.eco',			//class:146

			'/system/lib/Elastos.Droid.Server.eco',				//class:190
			'/system/lib/Elastos.Droid.System.eco',				//class:2(CZygote/CZygoteClassObject)
			'/system/lib/Elastos.Runtime.eco',					//class:3
			'/system/lib/Org.Kxml2.IO.eco',						//class:4
			'/system/lib/Org.Xml.Sax.eco',						//class:34

			'/data/elastos/Elastos.eco',						//class:2
			'/data/elastos/Gallery.eco',						//class:8
			'/data/elastos/ImageWallpaper.eco',					//class:12
			'/data/elastos/MusicPlayer.eco',					//class:16
			'/data/elastos/PinyinIME.eco',						//class:12

			'/data/elastos/SettingsProvider.eco',				//class:4
			'/data/elastos/SystemUI.eco',						//class:8
			'/data/elastos/WebViewDemo.eco',					//class:2
			'/data/elastos/JSTestObject.eco',					//class:6
		];

		var li_rc = 18;
		var lo_econame = document.getElementById('eco_name');
		lo_econame.value = la_eco[li_rc];

		var lo_car = window.CarObject.Test_Require_ModuleInfo(
			la_eco[li_rc],
			'CTestModuleInfo'
			);
		a=[
			la_eco[li_rc],
			'--'+typeof(lo_car),
			
			//'<br>',
			//'--GetPath--'+typeof(lo_car.GetPath),
			//'--GetPath--'+typeof(lo_car.GetPath()),
			//'--GetPath--'+JSON.stringify(lo_car.GetPath()),
			
			
			//'<br>',
			//'--GetVersion--'+typeof(lo_car.GetVersion),
			//'--GetVersion--'+typeof(lo_car.GetVersion()),
			//'--GetVersion--'+JSON.stringify(lo_car.GetVersion()),
			
			
			//'<br>',
			//'--GetClassCount--'+typeof(lo_car.GetClassCount),
			//'--GetClassCount--'+lo_car.GetClassCount(),
			
			/*
			'<br>',
			'--GetInterfaceCount--'+typeof(lo_car.GetInterfaceCount),
			'--GetInterfaceCount--'+lo_car.GetInterfaceCount(),
			'<br>',
			'--GetStructCount--'+typeof(lo_car.GetStructCount),
			'--GetStructCount--'+lo_car.GetStructCount(),
			'<br>',
			'--GetEnumCount--'+lo_car.GetEnumCount(),
			'--GetTypeAliasCount--'+lo_car.GetTypeAliasCount(),
			'<br>',
			'--GetConstantCount--'+lo_car.GetConstantCount(),
			'--GetImportModuleInfoCount--'+lo_car.GetImportModuleInfoCount()
			*/
		//if(lo_car)s += '--'+lo_car.GetName();
		//if(lo_car)s += '--'+typeof(lo_car.Test_String);
		//if(lo_car)s += '--GetClassInfo--'+typeof(lo_car.GetClassInfo);
		//if(lo_car)s += '--'+typeof(lo_car.SetInputArgumentOfInt16);
		//if(lo_car)s += '--'+typeof(lo_car.GetProperties);
		//if(lo_car)s += '--'+lo_car.Test_String('Hello Car World!');
		//if(lo_car)s += '--'+lo_car.GetPath();
		];

		/*
		a.push('<br>');
		a.push('--GetPath--'+typeof(lo_car.GetPath));
		//var ls_path = lo_car.GetPath();	//BUG
		var ls_path;
		ls_Path = lo_car.GetPath();
		a.push('--GetPath--'+typeof(ls_Path));
		a.push('--GetPath--'+ls_Path);
		*/
		
		a.push('<br>');
		a.push('--GetVersion--'+typeof(lo_car.GetVersion));
		a.push('--GetVersion--'+typeof(lo_car.GetVersion()));
		//a.push('--GetVersion--'+JSON.stringify(lo_car.GetVersion()));
		var lo_version = lo_car.GetVersion();
		a.push('--GetVersion--'+typeof(lo_version));
		a.push('--GetVersion--'+JSON.stringify(lo_version));

//var ttt = lo_car.ttt;
//lo_car.ttt='aaa';

/*		

		a.push('<br>');
		a.push('--GetClassInfo--'+typeof(lo_car.GetClassInfo));
		//a.push('--GetClassInfo--'+typeof(lo_car.GetClassInfo()));
		var lo_classinfo;
		//lo_classinfo = lo_car.GetClassInfo('CBoolean');
		lo_classinfo = lo_car.GetClassInfo('CTestModuleInfo');
		a.push('--GetClassInfo--'+typeof(lo_classinfo));
		a.push('--classinfo.GetName--'+typeof(lo_classinfo.GetName));
		if(typeof(lo_classinfo.GetName)=='function')a.push('--GetClassInfo--'+lo_classinfo.GetName());
		


		a.push('<br>');
		a.push('--GetAllClassInfos--'+typeof(lo_car.GetAllClassInfos));
		//a.push('--GetClassInfo--'+typeof(lo_car.GetClassInfo()));
		var lo_allclassinfos;
		//lo_classinfo = lo_car.GetClassInfo('CBoolean');
		lo_allclassinfos = lo_car.GetAllClassInfos();
		a.push('--GetAllClassInfos--'+typeof(lo_allclassinfos));
		//a.push('--classinfo.GetName--'+typeof(lo_classinfo.GetName));

		var lo_classinfo;

		if(typeof(lo_allclassinfos)=='object'){
			a.push('--GetAllClassInfos--'+lo_allclassinfos.length);
			for (var i=0;i<lo_allclassinfos.length;i++){
				lo_classinfo = lo_allclassinfos[i];
				a.push('--GetAllClassInfos--object.'+i+'----'+lo_classinfo.GetName())
					//+lo_classinfo.GetName());
			}
		}

		var lo_car_test = window.CarObject.Test_Require('/data/elastos/JSTestObject.eco','CTestCarDataType');
		a.push('--------CTestCarDataType----------');
		a.push('Test_NULL:'+lo_car_test.Test_NULL());

		a.push('Test_Int16: '+lo_car_test.Test_Int16(12));
		a.push('Test_Int32: '+lo_car_test.Test_Int32(1234));
		a.push('Test_Int64: '+lo_car_test.Test_Int64(12345678));
		a.push('Test_Byte: '+lo_car_test.Test_Byte(45));
		a.push('Test_Float: '+lo_car_test.Test_Float(12.34));
		a.push('Test_Double: '+lo_car_test.Test_Double(34.5678));

		a.push('Test_Char8: '+lo_car_test.Test_Char8('a'));
		a.push('Test_Char16(呵呵): '+lo_car_test.Test_Char16('x'));
		a.push('Test_CString: '+lo_car_test.Test_CString('ab'));
		a.push('Test_:String: '+lo_car_test.Test_String('cd'));
		a.push('Test_Boolean: '+lo_car_test.Test_Boolean(true));
		//a.push('Test_EMuid: '+lo_car_test.Test_EMuid());
		//a.push('Test_EGuid: '+lo_car_test.Test_EGuid());
		a.push('Test_ECode: '+lo_car_test.Test_ECode(24));
		a.push('Test_Enum: '+lo_car_test.Test_Enum(3));
		a.push('Test_StringBuf: '+lo_car_test.Test_StringBuf('CString_StringBuf_Value'));
		a.push('Test_ArrayOf_Int16: '+lo_car_test.Test_ArrayOf_Int16([1,2,3,4]));
		a.push('Test_ArrayOf_Int32: '+lo_car_test.Test_ArrayOf_Int32([2,3,4,5]));
		a.push('Test_ArrayOf_Int64: '+lo_car_test.Test_ArrayOf_Int64([3,4,5,6]));
		a.push('Test_ArrayOf_Byte: '+lo_car_test.Test_ArrayOf_Byte([11,12,13,14]));
		a.push('Test_ArrayOf_Float: '+lo_car_test.Test_ArrayOf_Float([12,13,14,15]));
		a.push('Test_ArrayOf_Double: '+lo_car_test.Test_ArrayOf_Double([13,14,15,16]));
		a.push('Test_ArrayOf_Char8: '+lo_car_test.Test_ArrayOf_Char8(['a','b','c','d']));
		//a.push('Test_ArrayOf_Char16: '+lo_car_test.Test_ArrayOf_Char16(['ab','cd','ef','gh']));
		a.push('Test_ArrayOf_CString: '+lo_car_test.Test_ArrayOf_CString(['how','do','you','do!']));
		a.push('Test_ArrayOf_String: '+lo_car_test.Test_ArrayOf_String(['are','you','ok?']));
		//a.push('Test_ArrayOf_Boolean: '+lo_car_test.Test_ArrayOf_Boolean([true,fase,true,false]));
		//a.push('Test_ArrayOf_EMuid: '+lo_car_test.Test_ArrayOf_EMuid());
		//a.push('Test_ArrayOf_EGuid: '+lo_car_test.Test_ArrayOf_EGuid());
		a.push('Test_ArrayOf_ECode: '+lo_car_test.Test_ArrayOf_ECode([1,2,3,4]));
		a.push('Test_ArrayOf_Enum: '+lo_car_test.Test_ArrayOf_Enum([0,1,2,3]));
		//a.push('Test_ArrayOf_IInterface: '+lo_car_test.Test_ArrayOf_IInterface());
		//a.push('Test_: '+lo_car_test.Test_());
		//a.push('Test_: '+lo_car_test.Test_());


		////a.push('Test_Int16: '+lo_car_test.Test_Int16_Ref(12));
*/
		
		for(var j=0;j<100;j++){
			a.push('----'+j+'----');
		}
		

		s = a.join('<br>');
	}catch(e){}

	var o = document.getElementById("view");
	o.innerHTML = s;
	o.scrollTo(0,0);
}




