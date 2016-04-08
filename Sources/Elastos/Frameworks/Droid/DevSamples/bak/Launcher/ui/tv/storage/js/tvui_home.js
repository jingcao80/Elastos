TVUI.HOME=(function(){
var $=function(sid){return document.getElementById(sid);};

return {
	obj:$('ui_home'),

	visible:true,
	enable:true,

	init:function(T){
		T=T||this;
		for(var p in T)T[p]&&T[p].init&&T[p].init();
	},

	run:function (T) {
		T=T||TVUI.HOME;

		if(!T.visible)return;
		if(!T.enable)return;

		for(var p in T)T[p].run&&T[p].run();
	},

	hide:function(){
		this.visible=false;

		var a=['app_tv','app_ms','app_kt','bar_top_02','bar_bot_01','bar_top_01','bar_top_bk'];
		for(var i=0,im=a.length;i<im;i++){
			var o=$(a[i]);
			if(o)o.style.display="none";
		}

		TVUI.LETV&&TVUI.LETV.stop();
	},
	show:function(){
		this.visible=true;

		TVUI.cpo=this;

		if(this.obj)this.obj.style.display="";

		var a=['app_tv','app_ms','app_kt','bar_top_02','bar_bot_01','bar_top_01','bar_top_bk'];
		for(var i=0,im=a.length;i<im;i++){
			var o=$(a[i]);
			if(o)o.style.display="";
		}

		TVUI.LETV&&TVUI.LETV.play();
	}
};

})();

