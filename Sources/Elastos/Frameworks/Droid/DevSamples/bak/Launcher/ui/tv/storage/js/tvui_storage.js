TVUI.STORAGE=(function(){

var $=function(sid){return document.getElementById(sid)};

var T={		/*--------storage begin--------*/

oCloudDisk:null,

oLogInfo:{
	status:'',	/*SUCESS/FAILURE*/
	message:''
},
login:function(){
	var ls_username=$('username').value;	/*default value:huanger*/
	var ls_password=$('password').value;	/*default value:123456*/

	try{
		var oLogRet=this.oCloudDisk.Login(ls_username, ls_password);
		this.oLogInfo={
			status:oLogRet.GetStatus(),
			message:oLogRet.GetMessage()
		}
	}catch(e){		/*run in board without car*/
		this.oLogInfo={
			status:'ok',
			message:'login success'
		}
	}

	if(this.oLogInfo.status=='fail'){
		alert(this.oLogInfo.message);

		$('password').value='';

		return false;
	}

	//this.fileItemTypeRender();

	var li_SharedFileCount;

	try{
		li_SharedFileCount=this.oCloudDisk.GetSharedFileCount();
	}catch(e){		/*run in board without car*/
		li_SharedFileCount=50;
	}
	var oShareNums=$('share_nums');
	oShareNums.innerHTML=li_SharedFileCount;

	var oUserPortrait=$('user_portrait');
	oUserPortrait.src='storage/img/usr/person_normal.png';

	var oUserName = $('current_username');
	oUserName.parentNode.setAttribute('cmd','cmd_storage_usermenu');

	oUserName.innerHTML=$('username').value;
	$('main_fileitems').style.display='';

	TVUI.COMPONENTS.clsGrid.createInstances([
		'tabgrid_fileitems',
		//'tabgrid_folders',
		'tabgrid_useritems',
		'tabgrid_set_useritems',
		'tabgrid_set_groupitems',
		'tabgrid_set_msgitems'
	]);

	TVUI.EVT.initKeyboardState();

	this.fileItemTypeRender();

	return true;
},
logout:function(){
	this.oCloudDisk.Logout();

	this.oLogInfo={
		status:'',
		message:''
	}

	var oUserName = $('current_username');

	oUserName.parentNode.setAttribute('cmd','cmd_storage_login');

	oUserName.innerHTML='登录';
	$('main_fileitems').style.display='none';
},

sInitCarParm:'/data/data/com.elastos.runtime/elastos/Com.MiniSocials.Cloud.Helper.eco:CCloudStorage;1',

sRootPath:'',
sCurPath:'',
sCurName:'',

aCurFileItemInfos:[],
iCurRc:0,
iMaxRc:0,
sSortFile:'',
bDescend:false,
bAscend:false,

oCopyInfo:{
	path:'',
	name:''
},

bDebug:false,

enumSortType_bak:{
	Alphabet:0,
	CreatedTime:1
},

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

enumSyncStatus_bak:[
	'Waiting',
	'Ongoing',
	'Compeleted'
],

enumChangingReason:{
	Added:0,
	Deleted:1
},

getFileItemDom:function(o){
	var oP=o;
	var sCmd=oP.getAttribute('cmd');
	while(oP&&!sCmd){
		oP=oP.parentNode;
		sCmd=oP.getAttribute('cmd');
	}

	return oP;
},
getFileNameInDom:function(o){
	var sName=o.innerHTML.split('<nobr>').pop().split('</nobr>')[0];
	return sName;
},

getCurrentFileItemInfos:function(aenum_FileItems){
	if(!aenum_FileItems)return;

	this.aCurFileItemInfos=[];

	var oFileItem;

	//log('getCurrentFileItemInfos.Reset.begin');
	aenum_FileItems.Reset();
	//log('getCurrentFileItemInfos.Reset.end');

	//log('getCurrentFileItemInfos.MoveNext.begin');
	var bValid = aenum_FileItems.MoveNext();;
	//log('getCurrentFileItemInfos.MoveNext.end');

	while(bValid){
		//log('getCurrentFileItemInfos.Current.begin');
		var oFileItem = aenum_FileItems.Current();
		//log('getCurrentFileItemInfos.Current.end');

		try{
			//log('getCurrentFileItemInfos.begin.1');
			var oFileInfo={
				name:oFileItem.GetName(),
				fullname:oFileItem.GetFullname(),
				type:oFileItem.GetType(),
				isFolder:oFileItem.IsFolder(),
				status:oFileItem.GetStatus
			} 
			//log('getCurrentFileItemInfos.end.1');
		}catch(e){
			alert('getCurrentFileItemInfos.err.01');
		}

		if(oFileInfo.name!='.'&&oFileInfo.name!='..'){
			this.aCurFileItemInfos.push(oFileInfo);
		}

		//log('getCurrentFileItemInfos.MoveNext.begin.1');
		bValid = aenum_FileItems.MoveNext();;
		//log('getCurrentFileItemInfos.MoveNext.end.1');
	}

	this.iMaxRc=this.aCurFileItemInfos.length;
},
sortByNameCurFileItemInfo:function(){
	if(this.bDescend||this.bAscend){
		this.bDescend=!this.bDescend;
		this.bAscend=!this.bAscend;

		this.aCurFileItemInfos.reverse();

		return;
	}

	this.bAscend=true;

	var a=[];
	for(var i=0,im=this.iMaxRc;i<im;i++){
		a.push(this.aCurFileItemInfos[i].name+' '+i);
	}
	a.sort();

	var b=[];
	for(var i=0,im=a.length;i<im;i++){
		b.push(this.aCurFileItemInfos[parseInt(a[i].split(' ').pop())]);
	}
	this.aCurFileItemInfos=b;
},

leftScroll:function(){
	this.iCurRc-=12;

	if(this.iCurRc<0){
		this.iCurRc=0;
	}else{
		this.fileItemRender();
	}
},
rightScroll:function(){
	this.iCurRc+=12;

	if(this.iCurRc>this.iMaxRc){
		this.iCurRc-=12;
	}else{
		this.fileItemRender();
	}
},

fnShareCircleRender:function(){
	//alert('TVUI.STORAGE.fnShareCircleRender');
},

fileItemRender:function(ao,i){
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

fileItemRender:function() {
	var ao,ls_HTML;
	var i,im,j;

	var bValid;
	var i,im;

	ao=document.getElementsByClassName('fileitem');

	i=0;
	im=Math.min(ao.length,this.iMaxRc-this.iCurRc);
	while(i<im){
		ao[i].style.display='';

		var oFileInfo=this.aCurFileItemInfos[i+this.iCurRc];

		ao[i].setAttribute('fileinfo',JSON.stringify(oFileInfo));

		ao[i].getElementsByTagName('img')[0].src="storage/img/file/blue.png"

		ao[i].getElementsByClassName("file_name")[0].innerHTML='<nobr>'+oFileInfo.name+'</nobr>';

		var sType = (oFileInfo.isFolder)?'folder':this.getFileTypeString(oFileInfo.type);

		var sImgName;
		var oFileIconDom=ao[i].getElementsByClassName("file_icon")[0];
		if(sType=='picture'){
			sImgName=oFileInfo.fullname;
			if(sImgName.indexOf('/')==0)sImgName=sImgName.replace('/','');

			oFileIconDom.parentNode.style.left='1%';
			oFileIconDom.parentNode.style.width='98%';
		}else{
			sImgName='storage/img/file/'+sType+'.png';

			oFileIconDom.parentNode.style.left='25%';
			oFileIconDom.parentNode.style.width='50%';
		}
		oFileIconDom.src=sImgName;

		var oStateDom=ao[i].getElementsByClassName("pin_state")[0];
		if(oFileInfo.type==this.enumFileType.app){
			oStateDom.parentNode.style.display='';
			oStateDom.src='storage/img/file/ping.png';
		}else{
			oStateDom.parentNode.style.display='none';
			oStateDom.src='';
		}
		oStateDom.style.display='';

		i++;
	}

	//this.fileItemRenderBlank(ao,i);

	im=ao.length;
	var aTmp;
	for(;i<im;i++){
		ao[i].style.display='none';
	}

	$('fileitems_left').style.display=(this.iCurRc>0)?'':'none';
	$('fileitems_right').style.display=(this.iCurRc<this.iMaxRc-12)?'':'none';	
},

fileItemTypeRender:function(iType) {
	var aenum_FileItems;

	if(typeof iType!='number'){
		aenum_FileItems = this.oCloudDisk.GetAllFiles(this.sCurPath,0,true);
	}else{
		aenum_FileItems = this.oCloudDisk.GetFileByType(iType,0,0);
	}

	this.getCurrentFileItemInfos(aenum_FileItems);

	this.fileItemRender();
},

fileItemSearchRender:function(sKeyword) {
	var aenum_FileItems=this.oCloudDisk.Search(sKeyword,this.sCurPath);

	this.getCurrentFileItemInfos(aenum_FileItems);

	this.fileItemRender();
},

showPathName:function(sColor){
	var o=document.getElementById('curpath');
	if(!o)return;

	var a=this.sCurPath.split('/');
	if(a[0].length==0)a.shift();
	var h=[];
	for(var i=0,im=a.length;i<im;i++){
		h.push('<div class="btn_blue_out" style="position:static;display:inline;float:left;margin-top:10px;border:1px solid rgba(255,255,255,0.05);">'+a[i]+'/'+'</div>');
	}
	if(this.sCurName.length){
		h.push('<div class="btn_blue_out" style="position:static;display:inline;float:left;margin-top:10px;border:1px solid rgba(255,255,255,0.05);">'+this.sCurName+'</div>');
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

	if(a[0].length==0)a.shift();
	if(a.length>0)a.pop();
	this.sCurPath=a.join('/');

	this.setCurName('');
	this.showPathName();
},

fileCopy:function(){
	this.oCopyInfo = {
		path:this.sCurPath,
		name:this.sCurName
	}

	$('copytofilename').value=this.sCurName;

	this.showPathName('red');
},
filePaste:function(as_DestFolder){
	var sCopyToFileName=$('copytofilename').value;

	var sFullPath = this.oCopyInfo.path+'/'+this.oCopyInfo.name;

	var sDestFolder = as_DestFolder||this.sCurPath;

	try{
		//log('oCloudDisk.Copy:begin');
		this.oCloudDisk.Copy(sFullPath,sDestFolder);
		//log('oCloudDisk.Copy:end');

		//this.fileItemTypeRender();
	}catch(e){
		log('oCloudDisk.filePaste.err');
	}

	return true;
},
fileDelete:function(){
	var sFullPath = this.sCurPath+'/'+this.sCurName;

	this.oCloudDisk.Delete(sFullPath);
},
createFolder:function(){
	var sNewFolderName=$('newfoldername').value;

	this.oCloudDisk.CreateFolder(this.sCurPath,sNewFolderName);
},

onFileItemChanged:function(ao_FileItem, ai_ChangingReason){
	this.fileItemTypeRender();
},

init_storage:function(){
	if(!window.elastos){
		this.setRootPath('root/sync');
		window.elastos=new TmpElastos();
	}else{
		//this.setCurPath('sync1');
		this.setRootPath('sync1');
		//this.setRootPath('root/sync');
	}

	this.oCloudDisk = elastos.createInstance(this.sInitCarParm);
	//this.oCloudDisk.Init("huanger", "password");	/*nouse ,to be deleted*/

	this.oCloudDisk.addCallback(" FileItemChanged", this.onFileItemChanged);
},
init_storage_share:function(){
	//alert('init_storage_share');
},

init:function(){
	var sFileName=location.href.split('/').pop();
	if(sFileName=='index_storage.htm'){
		this.init_storage();
	}else{
		//this.init_storage_share();
		this.init_storage();
	}
}

};		/*--------T end--------*/

return T;

})();		/*--------storage end--------*/

/*-------------------------------end-------------------------------------*/
