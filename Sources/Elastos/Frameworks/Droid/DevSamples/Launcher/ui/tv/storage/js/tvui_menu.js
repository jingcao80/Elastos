TVUI.MENU={
	name:null,
	obj:null,

	preTag:null,

	visible:true,
	enable:true,

	hide:function(){
		this.visible=false;
		if(this.obj)this.obj.style.display="none";
	},
	show:function(){
		this.visible=true;
		if(this.obj)this.obj.style.display="";

		TVUI.cpo=this;
	},

	setName:function(name){
		this.name=name;
		this.obj=document.getElementById(this.name);
	},

	move:function(x,y){
		if(!this.obj)return;

		this.obj.style.left=x;
		this.obj.style.top=y;
	},

	init:function(){
		//this.setName('menu_main');
		this.setName('');
	}
};
