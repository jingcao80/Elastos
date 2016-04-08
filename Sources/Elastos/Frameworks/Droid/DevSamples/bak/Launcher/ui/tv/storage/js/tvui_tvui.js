window.TVUI={
	bCarReady:true,
	bAllReady:true,

	bWeatherWidgetReady:false,
	iWeatherTryTimes:0,

	COMPONENTS:{},

	childs:[],

	screen:{
	},

	BK:new DivCanvas("bg_main",null,false),

	apo:[],
	cpo:null,

	isCarReady:function(){
		return this.bCarReady;
	},

	FN:{
		findByAttr:function(p,attr){
			var v;

			while(p&&p.getAttribute){
				v=p.getAttribute(attr);
				if(v)return v;
				p=p.parentNode;
			}
			return null;
		}
	},

	init:function(T){
		T=T||this;

		T.cpo=T.HOME;

		for(var p in T)T[p]&&T[p].init&&T[p].init();
		for(var p in TVUI.EVT)document.body.addEventListener(p,TVUI.EVT[p],false);

		setTimeout(
			function(){
				if(!T.bAllReady){
					setTimeout(arguments.callee,1000);
					return;
				}

				if(T.isCarReady()){
					try{
						var launcher = elastos.createInstance("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.eco:CLauncherHelper;1");
						launcher.CloseBootanimation();
					}catch(e){};
				}

				if(!T.bWeatherWidgetReady){
					T.showWeatherWidget(false);
				}

				//(function(){
					var o;
					o=document.getElementById('UI_KT');
					if(o)o.style.top='0px';

					//if(!T.isCarReady()){
					//	TVUI.fps.show(true);

						o=document.getElementById('app_ic_setting_btn_confirm');
						if(o)o.style.display='';
					//}

					//setInterval(window.TVUI.run,4);
					setInterval(window.TVUI.fps.psRun, 1000);

				//})();
			},
			1000
		);

	},

	run:function (T) {
		T=T||TVUI;
		for(var p in T)T[p]&&T[p].run&&T[p].run();
	},

	setCpo:function(o){
		this.cpo&&this.cpo.hide();
		if(o){
			this.cpo=o;
			this.cpo.show();
		}
	},

	showDemo:function(bDemo){
		if(bDemo){
			TVUI.DEMO.show();
			TVUI.cpo.hide();
		}else{
			TVUI.DEMO.hide();
			TVUI.cpo.show();
		}
	},
	showWeatherWidget:function(bLoad){
		var T=TVUI;
		var sid="app_wgt_weather_frame";
		var o=document.getElementById(sid);
		if(!o)return;

		T.bWeatherWidgetReady=T.bWeatherWidgetReady||bLoad;

		if(T.bWeatherWidgetReady){
			o.style.display="";
		}else{
			T.iWeatherTryTimes++;
			o.src=o.src.split('?')[0]+'?i='+T.iWeatherTryTimes;

			setTimeout(arguments.callee,30000);
		}
	}
};
