TVUI.IC_SETTING={
	obj:document.getElementById('app_ic_setting'),

	state:{
		wifi:true,
		bluetooth:true,
		storage:true,
		security:true,
		apmanage:true,
		shortcut:true,
		voice:true,
		bright:true,
		resolution:true,
		word:true,
		language:true,
		input:true,
		time:true,
		worldt:true,
		reset:true,
		infor:true
	},

	tabs:[],

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
	},

	saveState:function(){
		this.setTab();

		var s=JSON.stringify(this.state);

		localStorage&&localStorage.setItem("ic_setting_state", s);
	},

	getState:function(){
		var s=(localStorage&&localStorage.getItem("ic_setting_state"))||JSON.stringify(this.state);

		this.state=JSON.parse(s);

		var o;
		for(var p in this.state){
			//o=document.getElementById('hot_ic_setting_'+p);
			//o.style.display=this.state[p]?'':'none';
			o=document.getElementById('app_ic_setting_'+p);
			o=o.getElementsByTagName('IMG')
			o=o[1];
			o.setAttribute("src","img/ui_ic/elastos_demoui_ic_"+(this.state[p]?'':'un')+"check.png");
		}
	},

	getTab:function(){
		var o=document.getElementById('grp_app_hot_cont_00');
		if(!o)return;

		var a=o.getElementsByTagName('DIV');
		for(var i=0,im=a.length;i<im;i++){
			this.tabs.push({
				id:a[i].id,
				cmd:a[i].getAttribute('cmd'),
				tab:a[i].getAttribute('tab'),
				src:a[i].getElementsByTagName('IMG')[0].src
			});
		}
	},

	setTab:function(){
		var o=document.getElementById('grp_app_hot_cont_00');
		if(!o)return;

		var a=o.getElementsByTagName('DIV');
		for(var i=0,j=0,im=a.length;i<im;i++){
			var sid=this.tabs[i].id.replace('hot_ic_setting_','');
			if(typeof this.state[sid]=='undefined'||this.state[sid]){
				a[j].setAttribute('id',this.tabs[i].id);
				a[j].setAttribute('cmd',this.tabs[i].cmd);
				a[j].setAttribute('tab',this.tabs[i].tab);
				a[j].getElementsByTagName('IMG')[0].setAttribute('src',this.tabs[i].src);

				j++;
			}
		}
	},

	init:function(){
		this.getState();

		this.getTab();
		this.setTab();
	}
};
