/*-------------------------------window.elastos--------------------------------------*/

var TmpElastos=(function(){
	function fnRet(){
	}

	fnRet.prototype.createInstance=function(as_InitCarParm){
		this.is_InitCarParm=as_InitCarParm;

		return new TmpCloudStorage();
	}
	
	return fnRet;
})();

/*-------------------------------user data--------------------------------------*/

var ga_user=[
	{name:'huanger',sex:'Male',alias:'huangerA',desc:'huangerDesc',password:'123456'},
	{name:'user001',sex:'Male',alias:'user001A',desc:'user001Desc',password:'23456789'},
	{name:'user002',sex:'Male',alias:'user002A',desc:'user002Desc',password:'13456789'},
	{name:'user003',sex:'Male',alias:'user003A',desc:'user003Desc',password:'12456789'},
	{name:'user004',sex:'Male',alias:'user004A',desc:'user004Desc',password:'12356789'},
	{name:'user005',sex:'Male',alias:'user005A',desc:'user005Desc',password:'12346789'},
	{name:'user006',sex:'Male',alias:'user006A',desc:'user006Desc',password:'12345789'},
	{name:'user007',sex:'Male',alias:'user007A',desc:'user007Desc',password:'12345689'},
	{name:'user008',sex:'Male',alias:'user008A',desc:'user008Desc',password:'12345679'},
	{name:'user009',sex:'Male',alias:'user009A',desc:'user009Desc',password:'12345678'},
	{name:'yang.yong@kortide.com',sex:'Male',alias:'user009A',desc:'user009Desc',password:'elastos'}
];

var genum_ShareType={
	normal:0,
	transfer:1
};
var ga_circle=[
	{name:'circle001',desc:'circle001desc',creator:'user001'},
	{name:'circle002',desc:'circle002desc',creator:'user002'},
	{name:'circle003',desc:'circle003desc',creator:'user003'},
	{name:'circle004',desc:'circle004desc',creator:'user004'},
	{name:'circle005',desc:'circle005desc',creator:'user005'}
];
var ga_circle_user=[
	{circle_name:'circle001',user_name:'user002'},
	{circle_name:'circle001',user_name:'user003'},
	{circle_name:'circle002',user_name:'user001'},
	{circle_name:'circle002',user_name:'user003'},
	{circle_name:'circle002',user_name:'user004'}
];
var ga_circle_file=[
	{circle_name:'circle001',file_fullname:'root/sync/img_001.jpg',shartype:genum_ShareType.normal},
	{circle_name:'circle001',file_fullname:'root/sync/img_003.jpg',shartype:genum_ShareType.transfer},
	{circle_name:'circle001',file_fullname:'root/sync/dir01/name0102.apk',shartype:genum_ShareType.normal}
];


/*-------------------------------cloud disk emulate data--------------------------------------*/

var ga_SuffixType={
	folder:'folder',
	mp4:'video',
	wav:'video',
	mp3:'audio',
	jpg:'picture',
	png:'picture',
	gif:'picture',
	doc:'document',
	pdf:'document',
	txt:'document',
	apk:'app',
	zip:'app',
	fav:'fav'
}

var go_DiskRoot_bak={
	name:'root',
	isFolder:true,
	subfolder:[
		{
			name:'sync',
			isFolder:true,
			subfolder:[
/*
				{
					name:'dir01',
					isFolder:true,
					subfolder:[
					]
				}
*/
			]
		}
	]
};

var go_DiskRoot = {

name:'root',
isFolder:true,
subfolder:[
{name:'.'},
{name:'..'},
{
	name:'myphoto',
	subfolder:[
		{name:'myphoto_001.jpg'},{name:'myphoto_002.jpg'},{name:'myphoto_003.jpg'},
		{name:'myphoto_004.jpg'},{name:'myphoto_005.jpg'},{name:'myphoto_006.jpg'},
		{name:'myphoto_007.jpg'},{name:'myphoto_008.jpg'},{name:'myphoto_009.jpg'},
		{name:'myphoto_010.jpg'},{name:'myphoto_011.jpg'},{name:'myphoto_012.jpg'},
		{name:'myphoto_013.jpg'},{name:'myphoto_014.jpg'}
	]
},
{
	name:'sync',
	isFolder:true,
	subfolder:[
		{
			name:'debug data'
		},
		{name:'.'},
		{name:'..'},
		{
			name:'dir01',
			isFolder:true,
			subfolder:[
				{
					name:'dir0100',
					isFolder:true,
					subfolder:[
						{name:'.'},
						{name:'..'},
						{
							name:'name010000.mp4'
						},
						{
							name:'name010001.apk'
						},
						{
							name:'name010002.zip'
						},
						{
							name:'name010003.jpg'
						},
						{
							name:'name010004.png'
						},
						{
							name:'name010005.gif'
						}
					]
				},
				{
					name:'name0101.apk'
				},
				{
					name:'name0102.apk'
				},
				{
					name:'name0103.zip'
				},
				{
					name:'name0104.jpg'
				},
				{
					name:'name0105.png'
				},
				{
					name:'name0106.gif'
				}
			]
		},
		{
			name:'dir02',
			isFolder:true,
			subfolder:[
				{
					name:'name0200.mp4'
				},
				{
					name:'name0201.apk'
				},
				{
					name:'name0202.zip'
				},
				{
					name:'name0203.jpg'
				},
				{
					name:'name0204.png'
				},
				{
					name:'name0205.gif'
				}
			]
		},
		{
			name:'dir03',
			isFolder:true,
			subfolder:[
				{name:'dir0300',isFolder:true,subfolder:[]},
				{name:'dir0301',isFolder:true,subfolder:[]},
				{name:'dir0302',isFolder:true,subfolder:[]},
				{name:'dir0303',isFolder:true,subfolder:[]},
				{name:'dir0304',isFolder:true,subfolder:[]},
				{name:'dir0305',isFolder:true,subfolder:[]},
				{name:'dir0306',isFolder:true,subfolder:[]},
				{name:'dir0307',isFolder:true,subfolder:[]},
				{name:'dir0308',isFolder:true,subfolder:[]},
				{name:'dir0309',isFolder:true,subfolder:[]},
				{name:'dir0310',isFolder:true,subfolder:[]},
				{name:'dir0311',isFolder:true,subfolder:[]},
				{name:'dir0312',isFolder:true,subfolder:[]},
				{name:'dir0313',isFolder:true,subfolder:[]},
				{name:'dir0314',isFolder:true,subfolder:[]},
				{name:'dir0315',isFolder:true,subfolder:[]},
				{name:'dir0316',isFolder:true,subfolder:[]},
				{name:'dir0317',isFolder:true,subfolder:[]},
				{name:'dir0318',isFolder:true,subfolder:[]},
				{name:'dir0319',isFolder:true,subfolder:[]},
				{name:'dir0320',isFolder:true,subfolder:[]},
				{name:'dir0321',isFolder:true,subfolder:[]},
				{name:'dir0322',isFolder:true,subfolder:[]},
				{name:'dir0323',isFolder:true,subfolder:[]},
				{name:'dir0324',isFolder:true,subfolder:[]},
				{name:'dir0325',isFolder:true,subfolder:[]},
				{name:'dir0326',isFolder:true,subfolder:[]},
				{name:'dir0327',isFolder:true,subfolder:[]},
				{name:'dir0328',isFolder:true,subfolder:[]},
				{name:'dir0329',isFolder:true,subfolder:[]}
			]
		},
		{
			name:'img_001.jpg'
		},
		{
			name:'name03.apk'
		},
		{
			name:'name04.zip'
		},
		{
			name:'img_002.jpg'
		},
		{
			name:'img_003.png'
		},
		{
			name:'img_004.jpg'
		},
		{
			name:'name08.mp4'
		},
		{
			name:'name09.wav'
		},
		{
			name:'name10.mp3'
		},
		{
			name:'name11.doc'
		},
		{
			name:'name12.pdf'
		},
		{
			name:'name13.fav'
		}
	]
},
{
	name:'mydoc',
	subfolder:[],
},
{
	name:'root_001.jpg',
}

]	//root.subfolder
};	//root

var genum_FileType={
	document:0,
	picture:1,
	audio:2,
	video:3,
	app:4,
	others:5
};

var gfn_setFullname = function(o,aa_path){
	if(o.subfolder){
		o.isFolder=true;
	}else if(typeof o.type!='number'){
		var sType=o.name.split('.').pop().toLowerCase();
		sType = ga_SuffixType[sType];
		o.type = genum_FileType[sType];
	}

	aa_path.push(o.name);
	o.fullname=aa_path.join('/');

	if(o.subfolder)
		for(var i=0,im=o.subfolder.length;i<im;i++)
			arguments.callee(o.subfolder[i],aa_path);

	aa_path.pop(o.name);
};

gfn_setFullname(go_DiskRoot,['']);

var go_UsrProfile={
	user:'strUser',
	password:'strPassword'
}

/*-------------------------------collection enumerator--------------------------------------*/

var TmpObjectSimpleResult=(function(){
	var _p=[];

	function fnRet(o){
		_p.push({
			status:o.status||null,
			message:o.message||null
		});

		this._id=_p.length-1;
	}

	fnRet.prototype={
		GetStatus:function(){return _p[this._id].status},
		GetMessage:function(){return _p[this._id].message}
	}

	return fnRet;
})();

var TmpObjectEnumerator=(function(){
	var _p=[];

	function fnRet(a){
		_p.push({
			d:a,
			s:0,
			e:a.length-1,
			c:0
		});

		this._k=_p.length-1;
	}

	fnRet.prototype={
		Current:function(){
			var a=_p[this._k];
			return a.d[a.c];
		},
		MoveNext:function(){
			var a=_p[this._k];
			a.c++;
			var b=true;
			if(a.c>a.e){
				a.c--;
				b=false;
			}

			return b;
		},
		Reset:function(){
			var a=_p[this._k];
			a.c=-1;

			return;
		}
	}

	return fnRet;
})();

/*-------------------------------FileItem--------------------------------------*/

var TmpFileItem=(function(){
	var _p=[];

	function fnRet(o){
		_p.push({
			name:o.name||null,
			fullname:o.fullname||null,
			//parent:o.parent||null,
			type:o.type,
			isFolder:o.isFolder||null,
			status:o.status||null
		});

		this._id=_p.length-1;
	}

	fnRet.prototype={
		GetName:function(){return _p[this._id].name},
		GetFullname:function(){return _p[this._id].fullname},
		GetParent:function(){return _p[this._id].parent},
		GetType:function(){return (this.IsFolder())?-1:_p[this._id].type;},
		IsFolder:function(){return _p[this._id].isFolder},
		GetStatus:function(){return _p[this._id].status}
	}

	return fnRet;
})();

/*-------------------------------CloudStorage--------------------------------------*/

var TmpCloudStorage=(function(){
	var _p=[];

	function _getSubFileItem(aData,sName){
		for(var i=0,im=aData.length;i<im;i++){
			if(aData[i].name==sName){
				return aData[i];
			}
		}
		return null;
	}

	function _getFileItemByName(as_parent,ao_disk){
		var aNames=as_parent.split('/');
		var oP={subfolder:[ao_disk]};

		for(var i=0,im=aNames.length;i<im;i++){
			if(aNames[i]=='')continue;

			oP=_getSubFileItem(oP.subfolder,aNames[i]);
			if(!oP)break;
		}

		return oP;
	}

	function fnRet(oDisk){
		_p.push({disk:oDisk||go_DiskRoot});

		this._k=_p.length-1;
		this.disk=_p[this._k].disk;
	}

	fnRet.prototype={
		Init:function(as_username,as_password){	/*no use,to be deleted*/
			//
		},
		addCallback:function(as_eventName,afn_callback){
			//
		},

		/*---------------------------------UserInfo----------------------------*/


//newFunction:{
		/*---------------------------------login/out----------------------------*/
		Login:function(as_username,as_password){	/*用户登录*/
			var oRet={
				status:'ok',
				message:'login success'
			}

			for(var i=0,im=ga_user.length;i<im;i++){
				if(ga_user[i].name==as_username)break;
			}
			var oUser;
			if(i<im)oUser=ga_user[i];

			//if(as_username!='huanger'){
			if(oUser){
				//if(as_password!='123456'){
				if(oUser.password!=as_password){
					oRet.status='fail',
					oRet.message='password error'
				}
			}else{
				oRet.status='fail',
				oRet.message='invalid username'
			}

			//return oRet;
			return new TmpObjectSimpleResult(oRet);
		},
		Logout:function(){
		},

		/*---------------------------------Firends & Groups----------------------------*/

		GetMyFriends:function(){	/*取得好友列表*/
			var oFriendItemData={
				name:'',
				sex:'',
				alias:'',
				desc:''
			}

			function clsFriendItem(o){}
			clsFriendItem.prototype={
				GetName:function(){return 'str_FriendName'},
				GetSex:function(){return 'Male/Female'},
				GetAlias:function(){return 'str_Alias'},
				GetDesc:function(){return 'str_Description'}
			}

			var oFriendItem=new clsFriendItem();
			var aFriendItems=[];
			for(var i=0,im=20;i<im;i++)aFriendItems.push(oFriendItem);

			return {
				status:'ok',
				message:'no error',

				count:20,
				items:new TmpObjectEnumerator(aFriendItems)
			}
		},

		GetMyShareCircles:function(){	/*取得我加入的分享圈*/
			var aShareCircleUserItems=[];
			var aShareCircleFileItems=[];

			/*----------------ShareCircleItem Definition---------begin------------------------*/
			var oShareCircleItemData={
				name:'',
				desc:'',

				creator:{},	/*FriendItem object*/

				usercount:1,
				useritems:new TmpObjectEnumerator(aShareCircleUserItems),	

				filecount:2,
				fileitems:new TmpObjectEnumerator(aShareCircleFileItems)	/*enumerator of FileItems*/
			}

			function clsShareCircleItem(o){}
			clsShareCircleItem.prototype={
				GetName:function(){return oShareCircleItemData.name},
				GetDesc:function(){return oShareCircleItemData.desc},

				GetUserCount:function(){return oShareCircleItemData.usercount},

				GetFileCount:function(){return oShareCircleItemData.filecount}
			}
			/*----------------ShareCircleItem Definition---------end------------------------*/

			var oShareCircle=new clsShareCircleItem();
			var aShareCircleItems=[];
			for(var i=0,im=20;i<im;i++)aShareCircleItems.push(oShareCircleItem);

			return {
				status:'',
				message:'',

				count:20,
				items:new TmpObjectEnumerator(aShareCircleItems)
			}
		},

		/*---------------------------------Cloud Share----------------------------*/
		GetSharedFileCount:function(){
			return 100;
		},
		GetSharedFiles:function(){
			var aFileItems=[];

			var oRet={
				status:'ok',
				message:'',
				count:78,
				items:new TmpObjectEnumerator(aFileItems)	/*enumrator for FileItems*/
			}

			if(TVUI.STORAGE.oLogInfo.status!='ok'){
				oRet.status='fail';
			}

			return oRet;
		},
//},

		/*---------------------------------Cloud Storage----------------------------*/
		GetAllFiles:function(as_parent,ai_sortType,ai_descending){
			var aFileItems=[];
			var oP = _getFileItemByName(as_parent,this.disk);

			var aP=oP&&oP.subfolder;
			if(!aP){
				alert('GetAllFiles.error:path invalid!--'+as_parent);
				return;
			}

			for(var i=0,im=aP.length;i<im;i++){
				aFileItems.push(
					new TmpFileItem(aP[i])
				);
			}

			var oEnumFiles = new TmpObjectEnumerator(aFileItems);

			return oEnumFiles;
		},
		GetFileByType:function(ai_fileType,ai_sortType,ai_descending){
			var aFileItems=[];

			var oT=this;
			(function(sPath){
				var enumTmp=oT.GetAllFiles(sPath,0,0);

				enumTmp.Reset();

				var bValid=enumTmp.MoveNext();;
				while(bValid){
					var oFileItem=enumTmp.Current();
					if(oFileItem.GetType()==ai_fileType){
							aFileItems.push(oFileItem);
					}else{
						if(oFileItem.IsFolder()){
							arguments.callee(oFileItem.GetFullname());
						}else{
							//
						}
					}

					bValid=enumTmp.MoveNext();
				}
			})(TVUI.STORAGE.sRootPath);

			return new TmpObjectEnumerator(aFileItems);
		},
		Search:function(as_keywords,as_parent){
			var aFileItems=[];

			var oT=this;
			(function(sPath){
				var enumTmp=oT.GetAllFiles(sPath,0,0);

				enumTmp.Reset();

				var bValid=enumTmp.MoveNext();;
				while(bValid){
					var oFileItem=enumTmp.Current();
					if(oFileItem.GetName().indexOf(as_keywords)>-1){
							aFileItems.push(oFileItem);
					}

					if(oFileItem.IsFolder()){
						arguments.callee(oFileItem.GetFullname());
					}

					bValid=enumTmp.MoveNext();
				}
			})(as_parent);

			return new TmpObjectEnumerator(aFileItems);
		},
		Copy:function(as_fullPath,as_destFolder){
			var oF = _getFileItemByName(as_fullPath,this.disk);
			var oT = _getFileItemByName(as_destFolder,this.disk);

			var oNew = JSON.parse(JSON.stringify(oF));
			gfn_setFullname(oNew,[TVUI.STORAGE.sCurPath]);
			oT.subfolder.push(oNew);

			var oFiltItem = new TmpFileItem(oNew);
			TVUI.STORAGE.onFileItemChanged(oFiltItem,TVUI.STORAGE.Added);
		},
		Move:function(as_fullPath,as_destFolder){
			alert('TmpCloudStorage.Move:from('+as_fullPath+')to('+as_destFolder+')');
		},
		CreateFolder:function(as_path,as_name){
			//alert('CreateFolder--'+as_path+'--'+as_name);

			var oTmp={
					name:as_name,
					fullname:as_path+'/'+as_name,
					isFolder:true,
					subfolder:[]
			}

			var oP = _getFileItemByName(as_path,this.disk);
			oP.subfolder.push(oTmp);

			var oFiltItem = new TmpFileItem(oTmp);
			TVUI.STORAGE.onFileItemChanged(oFiltItem,TVUI.STORAGE.Added);
		},
		Delete:function(as_fullPath){
			var as_path=as_fullPath.split('/');
			var ls_name=as_path.pop();
			var ls_path=as_path.join('/');

			var oP = _getFileItemByName(ls_path,this.disk);
			var oF = _getFileItemByName(as_fullPath,this.disk);

			var aTmp=[];
			var oTmp;
			var aP=oP.subfolder;
			while(aP.length){
				oTmp=aP.pop();
				if(oTmp==oF){
					break;
				}else{
					aTmp.push(oTmp);
				}
			}

			while(aTmp.length)aP.push(aTmp.pop());

			var oFiltItem = new TmpFileItem(oF);
			TVUI.STORAGE.onFileItemChanged(oFiltItem,TVUI.STORAGE.Deleted);
		},
		ClearCache:function(){
			//
		}
	}

	return fnRet;
})();

/*------------------------------------------end---------------------------------------------------------*/
