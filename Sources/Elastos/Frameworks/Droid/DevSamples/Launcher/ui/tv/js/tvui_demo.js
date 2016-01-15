TVUI.DEMO={
	obj:document.getElementById('ui_demo'),

	visible:true,
	enable:true,

	childs:[],

	_init:function(){

		this.obj=document.getElementById('ui_demo');

		var aAlpha=[
			{cvs_id:'ui_demo_00',alpha:0.1},
			{cvs_id:'ui_demo_01',alpha:0.25},
			{cvs_id:'ui_demo_02',alpha:0.4},
			{cvs_id:'ui_demo_03',alpha:0.55},
			{cvs_id:'ui_demo_04',alpha:0.7},
			{cvs_id:'ui_demo_05',alpha:0.85}
		];

		for(var i=0,im=aAlpha.length;i<im;i++){
			this.childs.push(new DivCanvas(aAlpha[i].cvs_id,null,true));
			this.childs.push(new DivCanvas(aAlpha[i].cvs_id+'_txt',null,false));
		}

	},

	init:function(T){
		T=T||this;

		T._init();

		for(var p in T)T[p].init&&T[p].init();

		for(var i=0,im=T.childs.length;i<im;i++){
			T.childs[i].init();
		}
	},

	run:function (T) {
		T=T||TVUI.DEMO;

		if(!T.visible)return;
		if(!T.enable)return;

		for(var p in T)T[p].run&&T[p].run();

		for(var i=0,im=T.childs.length;i<im;i++){
			T.childs[i].run();
		}

		T.visible&&setTimeout(T.run,20);
	},

	hide:function(){
		if(!this.visible)return;

		this.visible=false;
		this.obj.style.display="none";
	},
	show:function(){
		if(this.visible)return;

		this.visible=true;
		this.obj.style.display="";

		this.run();
	}
};
