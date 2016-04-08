TVUI.CFG={
	//defaultVideoPath:'file:///D:/temp/video/',
	defaultVideoPath:'/storage/sdcard0/',	/*default path of video files*/

	maxFreeTimes:3000000,		/*free time,show demo*/

	endFlag:null			/*bound check,no use*/
};

TVUI.CMD=(function(){
var $=function(sid){return document.getElementById(sid)};
var _openNewWindow=function(s){
	var oTmp=document.getElementById('dom_newwindow');
	if(!oTmp)return;

	oTmp.href=s;
	oTmp.click();
};
var P=TVUI;
var T = {
	/*------------------------------------------*/
	cmd_showSetting:function(bShow,bEdit){
		if(bShow){
			setTimeout(
				function(){
					var a=['bar_top_02','bar_bot_01','bar_top_01','bar_top_bk'];
					for(var i=0,im=a.length;i<im;i++){
						var o=$(a[i]);
						o.style.display="";
					}
				},
				50
			)

		}

		P.setCpo(bShow?P.IC_SETTING:P.HOME);
	},
	cmd_showBrowser:function(bShow,bEdit){
		P.setCpo(bShow?P.IC_BROWSER:P.HOME);
	},

	cmd_shortcutSetting:function(oTarget){
		var o=$('app_ic_setting_exec');
		o&&o.setAttribute('id','app_ic_setting_edit');

		T.cmd_showSetting(true,true);
	},
	cmd_ui_shortcutSetting_ret:function(){
		T.cmd_showSetting(false);
	},

	cmd_ui_setting_ret:function(){
		T.cmd_showSetting(false);
	},

	/*--------play video by html5----------------*/
	_cmd_playVideo:function(sPID,sURI,sRetPage){
		var bGenVideoTag=false;
		var oCvsCont=$(sPID);
		var sVID=sPID+'_video';

		(function(){
			var oVideo=$(sVID);
			if(!oVideo){
				if(!bGenVideoTag){
					oCvsCont.innerHTML=[
							'<video id="'+sVID+'" class="video-js vjs-default-skin" controls preload="auto" width="100%" height="100%" poster="my_video_poster.png" data-setup="{}">',
							'	<source src="'+sURI+'" type="video/mp4">',
							'	<source src="'+sURI+'" type="video/webm">',
							'</video>'
					].join('');

					bGenVideoTag=true;
				}

				setTimeout(arguments.callee,50);
				return;
			}

			function _onended(e){
				P.setCpo(P.HOME);
				setTimeout(
					function(){
						oVideo.removeEventListener('ended',_onended,false);
						oCvsCont.innerHTML='';
					},
					1000
				);
			}

			oVideo.addEventListener(
				'ended',
				_onended,
				false
			);

			//oVideo.play();
			oVideo.autoplay=true;

			P.setCpo(P[sRetPage]);
		})();
	},
	cmd_app_tv:function(){
		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.StartVideoViewDemo(TVUI.LETV.getCurVideoName(), "android.intent.action.MAIN");

		}catch(e){}}
	},
	cmd_app_tv_icons:function(oTarget){
		var sid=oTarget.getAttribute('id');
		var num=parseInt(sid.split('_').pop());

		TVUI.LETV.setAngle(num*90);
	},

	/*--------elast home menu----------------*/
	cmd_home_ic_androidmarket:function(){
		/*Android Market*/

		P.setCpo(P.IC_ANDROIDMARKET);
	},
	cmd_home_ic_cloud:function(){
		/*Personal Cloud Storage*/
		var bDefault=true;
		if(TVUI.isCarReady()){
			try{
				TVUI.LETV.stop();

				//var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
				//launcher.LaunchApp2("com.popocloud.app", "android.intent.action.MAIN");

				_openNewWindow('index_storage.htm');

				bDefault=false;
			}catch(e){}
		}
		bDefault&&alert('running : cmd_home_ic_cloud not ready!');
	},
	cmd_home_ic_share:function(){
		/*Personal Cloud Share*/

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			//var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			//launcher.LaunchApp2("Sandalwood", "android.intent.action.MAIN");

			_openNewWindow('http://localhost:8880/web/');

		}catch(e){}}
	},
	cmd_home_ic_elastosmall:function(){
		/*Elastos Mall*/

		P.setCpo(P.IC_ELASTOSMALL);
	},
	cmd_home_ic_multi:function(){
		/*Multi Media Center*/

		var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("XBMC", "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		/*weather default action would be run*/
		bDefault&&alert('running : cmd_home_ic_multi failure!');
	},
	cmd_home_ic_browser:function(){
		/*Browser*/

		var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("Snowfox", "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		bDefault&&alert('running : cmd_home_ic_browser failure!');
	},

	cmd_home_ic_setting:function(oEvt){
		/*Control Center*/

		var o=$('app_ic_setting_edit');
		o&&o.setAttribute('id','app_ic_setting_exec');

		T.cmd_showSetting(true,false);
	},

	/*--------system configuration----------------*/
	cmd_ic_setting_check:function(oTarget){
		/*Shoutcut for Control Center*/

		var bChk=!(oTarget.src.indexOf("/elastos_demoui_ic_check.png")>-1);

		var src="img/ui_ic/elastos_demoui_ic_&T.check.png".replace('&T.',bChk?"":"un");
		oTarget.setAttribute("src",src);

		var cmd=P.FN.findByAttr(oTarget.parentNode,'cmd');
		if(cmd){
			//var oHot=$(cmd.replace('cmd_','hot_'));
			//if(oHot)oHot.style.display=bChk?"":"none";

			var prop=cmd.split('_').pop();
			P.IC_SETTING.state[prop]=bChk;
			P.IC_SETTING.saveState();
		}
	},

	cmd_ic_setting_calculator:function(){
		/* Calculator Demo*/

		function _exec_calculator(){
			/*----add code here----*/
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("com.android.calculator2", "android.intent.action.MAIN");
		}

		var bDefault=true;
		if(TVUI.isCarReady()){
			try{
				_exec_calculator();

				bDefault=false;
			}catch(e){}
		}
		bDefault&&alert('running : cmd_ic_setting_calculator not ready!');
	},
	cmd_ic_setting_wifi:function(){
	    var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("WifiSetting", "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		bDefault&&alert('running : cmd_ic_setting_wifi failure!');
	},
	cmd_ic_setting_gps:function(){
		//TODO:for demo
		var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("LocationDemo", "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		bDefault&&alert('running : cmd_ic_setting_gps failure!');
	},
	cmd_ic_setting_privilege:function(){
		var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2("ElPermission", "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		bDefault&&alert('running : cmd_ic_setting_privilege failure!');
	},
	cmd_ic_setting_bluetooth:function(){
		alert('running : cmd_ic_setting_bluetooth');
	},
	cmd_ic_setting_storage:function(){
		alert('running : cmd_ic_setting_storage');
	},
	cmd_ic_setting_security:function(){
		alert('running : cmd_ic_setting_security');
	},
	cmd_ic_setting_apmanage:function(){
		alert('running : cmd_ic_setting_apmanage');
	},
	cmd_ic_setting_shortcut:function(){
		alert('running : cmd_ic_setting_shortcut');
	},
	cmd_ic_setting_voice:function(){
		alert('running : cmd_ic_setting_voice');
	},
	cmd_ic_setting_bright:function(){
		alert('running : cmd_ic_setting_bright');
	},
	cmd_ic_setting_resolution:function(){
		alert('running : cmd_ic_setting_resolution');
	},
	cmd_ic_setting_word:function(){
		alert('running : cmd_ic_setting_word');
	},
	cmd_ic_setting_language:function(){
		alert('running : cmd_ic_setting_language');
	},
	cmd_ic_setting_input:function(){
		alert('running : cmd_ic_setting_input');
	},
	cmd_ic_setting_time:function(){
		alert('running : cmd_ic_setting_time');
	},
	cmd_ic_setting_worldt:function(){
		alert('running : cmd_ic_setting_worldt');
	},
	cmd_ic_setting_reset:function(){
		alert('running : cmd_ic_setting_reset');
	},
	cmd_ic_setting_infor:function(){
		alert('running : cmd_ic_setting_infor');
	},
	cmd_exec_app:function(o) {
		var bDefault=true;

		if(TVUI.isCarReady()){try{
			TVUI.LETV.stop();

			var pachakgeName = P.FN.findByAttr(o, 'app');
			var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
			launcher.LaunchApp2(pachakgeName, "android.intent.action.MAIN");

			bDefault=false;
		}catch(e){}}

		bDefault&&alert('running : cmd_exec_app failure!');
	}
};	//return

return T;

})();
