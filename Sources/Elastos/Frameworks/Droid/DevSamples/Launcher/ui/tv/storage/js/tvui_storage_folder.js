TVUI.STORAGE.FOLDER=(function(){

var $=function(sid){return document.getElementById(sid)};

var T={		/*--------storage begin--------*/

_init:false,

sRootPath:'',
sCurPath:'',
sCurName:'',

aCurInfos:[],
iCurRc:0,
iMaxRc:0,

enumFileType:{
	document:0,
	picture:1,
	audio:2,
	video:3,
	app:4,
	others:5
},
getFileTypeString:function(i){
	for(var p in this.enumFileType)if(this.enumFileType[p]==i)return p;
	return 'others';
},

showPathName:function(sColor){
	var o=$('curfolderpath');
	if(!o)return;

	var a=this.sCurPath.split('/');
	a.shift();
	var h=[];
	for(var i=0,im=a.length;i<im;i++){
		h.push('<div class="btn_blue_out" style="position:static;display:inline;float:left;margin-top:10px;border:1px solid rgba(255,255,255,0.05);">'+a[i]+'/'+'</div>');
	}

	o.innerHTML=h.join('');
	sColor=sColor||'white';
	o.style.color=sColor;
},
setFullName:function(sFullName){
	var aFileInfo=sFullName.split('/');

	this.sCurName=aFileInfo.pop();
	this.sCurPath=aFileInfo.join('/');

	this.showPathName();
},
setRootPath:function(sPath){
	this.sRootPath=sPath;
	this.setCurPath(sPath);
},
setCurPath:function(sPath){
	this.iCurRc=0;

	if(this.sCurPath.indexOf(this.sRootPath)>1){
		var a=this.sCurPath.split(this.sRootPath);
		a[0]='';
		this.sCurPath=a.join(this.sRootPath);
	}

	if(this.sCurPath.length)this.sCurPath+='/';
	this.sCurPath+=sPath;

	this.setCurName('');
	this.showPathName();
},
setCurName:function(s){
	this.sCurName = s;

	this.showPathName();
},
delCurPath:function(){
	this.iCurRc=0;

	var a=this.sCurPath.split('/');
	if(a.length>1)a.pop();
	this.sCurPath=a.join('/');

	this.setCurName('');
	this.showPathName();
},

filePaste:function(){
	return TVUI.STORAGE.filePaste(this.sCurPath);
},

getCurrentInfos:function(aenum_FileItems){
	if(!aenum_FileItems)return;

	this.aCurInfos=[];

	var oFileItem;

	aenum_FileItems.Reset();

	var bValid = aenum_FileItems.MoveNext();;

	while(bValid){
		var oFileItem = aenum_FileItems.Current();

		var oFileInfo={
			name:oFileItem.GetName(),
			fullname:oFileItem.GetFullname(),
			type:oFileItem.GetType(),
			isFolder:oFileItem.IsFolder(),
			status:oFileItem.GetStatus
		} 

		if(oFileInfo.name!='.'&&oFileInfo.name!='..'&&oFileInfo.isFolder){
			this.aCurInfos.push(oFileInfo);
		}

		bValid = aenum_FileItems.MoveNext();;
	}

	this.iMaxRc = this.aCurInfos.length;
},

renderBlank:function(ao,i){
	im=ao.length;
	var aTmp;
	for(;i<im;i++){
		ao[i].style.display='none';

		aTmp=ao[i].getElementsByTagName('img');
		if(aTmp.length>1){
			aTmp[0].src="storage/img/file/yellow.png"
			aTmp[1].src="storage/img/file/yellow.png"
		}

		ao[i].setAttribute('fileinfo',null);
		aTmp=ao[i].getElementsByClassName("file_name");
		if(aTmp.length)aTmp[0].innerHTML='<nobr></nobr>';;

		aTmp=ao[i].getElementsByClassName("pin_state");
		if(aTmp.length>0){
			aTmp[0].src='';
			aTmp[0].style.display='none';
		}
	}
},

render:function(){
	var ao=document.getElementsByClassName('folderitem');
	var i=0;
	var im=Math.min(ao.length,this.iMaxRc-this.iCurRc);

	while(i<im){
		ao[i].style.display='';

		var oFileInfo=this.aCurInfos[i+this.iCurRc];

		ao[i].setAttribute('fileinfo',JSON.stringify(oFileInfo));
		ao[i].getElementsByTagName('img')[0].src="storage/img/file/blue.png"
		ao[i].getElementsByClassName("file_name")[0].innerHTML='<nobr>'+oFileInfo.name+'</nobr>';

		var sImgName;
		sImgName='storage/img/file/folder.png';
		var oFileIconDom=ao[i].getElementsByClassName("file_icon")[0];
		var oStyle=oFileIconDom.parentNode.style;
		oStyle.left='25%';
		oStyle.width='50%';
		oFileIconDom.src=sImgName;

		var oStateDom=ao[i].getElementsByClassName("pin_state")[0];
		oStateDom.parentNode.style.display='none';
		//oStateDom.src='';
		//oStateDom.style.display='';

		i++;
	}

	//this.renderBlank(ao,i);

	im=ao.length;
	for(;i<im;i++){
		ao[i].style.display='none';
	}

	$('folderitems_left').style.display=(this.iCurRc>0)?'':'none';
	$('folderitems_right').style.display=(this.iCurRc<this.iMaxRc-12)?'':'none';	
},
typeRender:function(iType){
	this._init||this.init();

	var aenum_FileItems;
	aenum_FileItems = TVUI.STORAGE.oCloudDisk.GetAllFiles(this.sCurPath,0,true);

	this.getCurrentInfos(aenum_FileItems);

	this.render();
},

leftScroll:function(){
	this.iCurRc-=12;

	if(this.iCurRc<0){
		this.iCurRc=0;
	}else{
		this.render();
	}
},
rightScroll:function(){
	this.iCurRc+=12;

	if(this.iCurRc>this.iMaxRc){
		this.iCurRc-=12;
	}else{
		this.render();
	}
},

init:function(){
	this._init=true;

	this.setRootPath(TVUI.STORAGE.sRootPath);

	TVUI.COMPONENTS.clsGrid.createInstances(['tabgrid_folders']);
},

};		/*--------T end--------*/

return T;

})();		/*--------storage end--------*/

/*-------------------------------end-------------------------------------*/
