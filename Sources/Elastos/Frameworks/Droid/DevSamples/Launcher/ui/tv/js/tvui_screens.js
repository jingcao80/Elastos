TVUI.IC_ANDROIDMARKET={
	obj:document.getElementById('app_ic_androidmarket'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};
TVUI.IC_CLOUD={
	obj:document.getElementById('app_ic_cloud'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};
TVUI.IC_SHARE={
	obj:document.getElementById('app_ic_share'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};
TVUI.IC_ELASTOSMALL={
	obj:document.getElementById('app_ic_elastosmall'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};
TVUI.IC_MULTI={
	obj:document.getElementById('app_ic_multi'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";

		document.getElementById('app_ic_multi_cont').innerHTML='';
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};
TVUI.IC_BROWSER={
	obj:document.getElementById('app_ic_browser'),
	cfg:[],

	visible:true,
	enable:true,
	
	hide:function(){
		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		this.obj.style.display="";

		TVUI.cpo=this;
	}
};


