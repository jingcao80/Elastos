TVUI.CFG={
	//defaultVideoPath:'file:///D:/temp/video/',
	defaultVideoPath:'/storage/sdcard0/',	/*default path of video files*/

	maxFreeTimes:3000000,		/*free time,show demo*/

	endFlag:null			/*bound check,no use*/
};

TVUI.CMD=(function(){
var $=function(sid){return document.getElementById(sid)};
var P=TVUI;
var S=P.STORAGE;
var M=P.MENU;
var T = {
	/*--------------------------------storage--------------------------------------*/
	cmd_showMenu:function(bShow){
		//P.setCpo(bShow?M:P.HOME);
	},

	/*--------------------------------user menu begin--------------------------------------*/
	cmd_storage_login:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_login");
		P.setCpo(M);
	},
	cmd_storage_login_confirm:function(o) {
		var blogin=S.login();

		if(blogin){
			P.setCpo(null);
			P.setCpo(P.HOME);
		}
	},
	cmd_storage_regist:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_regist");
		P.setCpo(M);
	},
	cmd_storage_regist_confirm:function(o) {
		this.cmd_storage_popinfo('cmd_storage_regist_confirm','To be finished, Come Soon!');
	},
	cmd_storage_usermenu:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_usermenu");
		P.setCpo(P.cpo==P.HOME?M:P.HOME);
	},

	cmd_storage_message:function(o){
		P.setCpo(null);
		M.setName("menu_storage_message");
		P.setCpo(M);
	},

	cmd_storage_usermenu_minisocials:function(o) {
		var oDomNewWindow=$('dom_newwindow');
		oDomNewWindow.href='http://42.121.86.90/task/minisocials/huanghao/msui_0916/index.html';
		oDomNewWindow.click();

		P.setCpo(null);
		P.setCpo(P.cpo==P.HOME?M:P.HOME);
	},
	cmd_storage_usermenu_miniportals:function(o) {
		var oDomNewWindow=$('dom_newwindow');
		oDomNewWindow.href='http://42.121.86.90/task/miniportal/singer/liyonggang/MiniSinger_0928/index.html';
		oDomNewWindow.click();

		P.setCpo(null);
		P.setCpo(P.cpo==P.HOME?M:P.HOME);
	},
	cmd_storage_usermenu_account_set:function(o) {
		var oDomNewWindow=$('dom_newwindow');
		//oDomNewWindow.href='http://42.121.86.90/task/storage/UIDS/%E9%80%9A%E8%AE%AF%E5%BD%95.html';
		oDomNewWindow.href='http://42.121.86.90/task/storage/UIDS/帐号设置.html';
		oDomNewWindow.click();

		P.setCpo(null);
		P.setCpo(P.cpo==P.HOME?M:P.HOME);
	},
	cmd_storage_usermenu_account_bind:function(o) {
		this.cmd_storage_popinfo('Come Soon','cmd_storage_usermenu_account_bind');

		$('dom_storage_usermenu_account_bind').setAttribute('cmd','cmd_storage_usermenu_account_unbind');
		$('menu_storage_usermenu_account_bind_text').innerHTML='解除该设备为当前帐户server';
	},
	cmd_storage_usermenu_account_unbind:function(o) {
		this.cmd_storage_popinfo('Come Soon','cmd_storage_usermenu_account_unbind');

		$('dom_storage_usermenu_account_bind').setAttribute('cmd','cmd_storage_usermenu_account_bind');
		$('menu_storage_usermenu_account_bind_text').innerHTML='绑定该设备为当前帐户server';
	},
	cmd_storage_usermenu_logout:function(o) {
		//var oUserName = $('current_username');

		//oUserName.parentNode.setAttribute('cmd','cmd_storage_login');

		//oUserName.innerHTML='登录';
		//$('main_fileitems').style.display='none';

		S.logout();

		P.setCpo(null);
		P.setCpo(P.HOME);
	},
	/*--------------------------------user menu end--------------------------------------*/

	cmd_storage_menu:function(o) {
		P.setCpo(null);
		M.setName("menu_main");
		P.setCpo(P.cpo==P.HOME?M:P.HOME);
		//P.setCpo(P.cpo==M?null:M);
	},
	cmd_storage_file_search:function(){
		var sKeyword=$("keyword").value;
		if(!sKeyword)return;

		S.fileItemSearchRender(sKeyword);
	},

	cmd_storage_file_clearkeyword:function(){
		$("keyword").value='';
	},

	cmd_storage_sortbytime:function(o) {
		this.cmd_storage_popinfo('cmd_storage_sortbytime','Datetime Needed, Come Soon!');
	},
	cmd_storage_sortbyname:function(o) {
		S.sortByNameCurFileItemInfo();
		S.iCurRc=0;
		S.fileItemRender();
	},
	cmd_storage_file_attach:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_file_attach");
		P.setCpo(M);
	},
	cmd_storage_file_newfolder:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_file_newfolder");
		P.setCpo(M);
	},
	cmd_storage_file_newfolder_confirm:function(o) {
		//this.cmd_storage_popinfo('cmd_storage_file_newfolder_confirm','Create New Folder, Come Soon!');

		S.createFolder();

		P.setCpo(null);
		P.setCpo(P.HOME);
	},

	cmd_storage_fileitem:function(o) {
		//log('CMD.cmd_storage_fileitem:begin')

		var oP=S.getFileItemDom(o);
		if(!oP)return;

		if(M.visible&&M.name=="menu_storage_fileitem"&&M.preTag==oP.id){
			M.hide();
		}else{
			M.preTag=oP.id;

			var sFileInfo=oP.getAttribute('fileinfo');
			if(!sFileInfo.length)return;

			var oFileInfo=JSON.parse(sFileInfo);

			if(!oFileInfo)return;

			S.setFullName(oFileInfo.fullname);

			var sFileType=oFileInfo.isFolder?'folder':S.getFileTypeString(oFileInfo.type);
			var sMenuName="menu_storage_fileitem_"+sFileType;

			var aSeq=oP.id.split('_');

			var iCol=parseInt(aSeq.pop());
			var iRow=parseInt(aSeq.pop());

			var iLeft=(iCol+1)*22.5-25;
			iLeft=Math.max(iLeft,0.5);
			iLeft=Math.min(iLeft,61.5);
			var sLeft=iLeft+'%';

			var iTop=(iRow+1)*25+11;
			var sTop=iTop+'%';

			P.setCpo(null);
			M.setName(sMenuName);
			M.move(sLeft,sTop);
			P.setCpo(M);
		}

		//log('CMD.cmd_storage_fileitem:end')
	},

	cmd_storage_folderitem:function(o){
		var oP=S.getFileItemDom(o);
		if(!oP)return;

		var sFileInfo=oP.getAttribute('fileinfo');
		var oFileInfo=JSON.parse(sFileInfo);

		if(!oFileInfo)return;

		S.FOLDER.setCurPath(oFileInfo.name);
		S.FOLDER.typeRender();
	},

	cmd_storage_groupitem:function(o) {
		this.cmd_storage_sharecircle_create(o);
	},
	cmd_storage_useritem:function(o) {
		var oP=S.getFileItemDom(o);
		if(!oP)return;

		var a=oP.getElementsByClassName('rightcheck');
		if(a.length){
			var o=a[0];
			var s=o.style.display;
			o.style.display=(s.length)?'':'none';
		}

		/*save check status,to be continue*/
	},

	cmd_storage_file_share:function(o) {
		P.setCpo(null);

		S.fnShareCircleRender();
		M.setName("menu_storage_file_sharecircle");

		P.setCpo(M);
	},

	cmd_storage_share_confirm:function(o) {
		this.cmd_storage_popinfo('cmd_storage_share_confirm','Share With Your Friends, OK!');
	},

	cmd_storage_sharecircle_create:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_file_share");
		P.setCpo(M);
	},

	cmd_storage_file_sharecircle:function(o) {
		//this.cmd_storage_popinfo('cmd_storage_file_share','Share With Your Friends, Come Soon!');

		//var oDomNewWindow=$('dom_newwindow');
		//oDomNewWindow.href='index_storage_share.htm';
		//oDomNewWindow.click();

		//P.setCpo(null);
		//P.setCpo(P.cpo==P.HOME?M:P.HOME);

		P.setCpo(null);
		M.setName("menu_storage_file_share");
		P.setCpo(M);
	},
	cmd_storage_file_privilege:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_privilege','权限设置, OK!');
	},
	cmd_storage_file_noshare:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_noshare','取消分享, OK!');
	},
	cmd_storage_file_pin:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_file_pin");
		P.setCpo(M);
	},
	cmd_storage_file_unpin:function(o) {
		P.setCpo(null);
		M.setName("menu_storage_file_unpin");
		P.setCpo(M);
	},
	cmd_storage_file_mark:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_mark','收藏文件,OK!');
	},
	cmd_storage_file_mark_word:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_mark_word','设置文件分类标签, OK!');
	},
	cmd_storage_file_copy:function(o) {
		S.fileCopy();

		S.FOLDER.typeRender();

		P.setCpo(null);
		M.setName("menu_storage_file_copy");
		P.setCpo(M);
	},
	cmd_storage_paste:function(o) {
		S.filePaste();

		P.setCpo(null);
		P.setCpo(P.HOME);
	},
	cmd_storage_paste_confirm:function(o) {
		var bSUCCESS=S.FOLDER.filePaste();

		if(bSUCCESS){
			P.setCpo(null);
			P.setCpo(P.HOME);
		}
	},

	cmd_storage_file_folder_open:function(o) {
		var sId = M.preTag;
		var oItemDom=$(sId);
		if(!oItemDom)return;

		var sFileInfo=oItemDom.getAttribute('fileinfo');
		var oFileInfo=JSON.parse(sFileInfo);
		if(!oFileInfo)return;

		S.setCurPath(oFileInfo.name);
		//P.CMD.cmd_storage_cat_file();
		S.fileItemTypeRender();

		P.setCpo(null);
		P.setCpo(P.HOME);
	},
	cmd_storage_file_run:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_run','运行应用, OK!');
	},
	cmd_storage_file_browse:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_browser','查看文件, OK!');
	},
	cmd_storage_file_open_picture:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_open_picture','查看相片, OK!');
	},
	cmd_storage_file_play_video:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_play_video','播放视频, OK!');
	},
	cmd_storage_file_play_audio:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_play_audio','播放音乐, OK!');
	},
	cmd_storage_file_open_others:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_open_others','打开其他类型文件, OK!');
	},
	cmd_storage_file_detail:function(o) {
		var sId = M.preTag;
		var oItemDom=$(sId);
		if(!oItemDom)return;

		var sFileInfo=oItemDom.getAttribute('fileinfo');
		var oFileInfo=JSON.parse(sFileInfo);
		if(!oFileInfo)return;

		P.setCpo(null);
		M.setName("menu_storage_file_detail");
		P.setCpo(M);

		/*use frame*/
		//var oDomNewWindow=$('dom_newwindow');
		//oDomNewWindow.href='index_storage_detail.htm';
		//oDomNewWindow.click();
	},
	cmd_storage_file_delete:function(o) {
		S.fileDelete();

		P.setCpo(null);
		P.setCpo(P.HOME);
	},

	cmd_storage_file_camera:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_camera','Take Photos,Save & Share, Come Soon!');
	},
	cmd_storage_file_local:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_camera','Add Local Files To Asc, Come Soon!');
	},
	cmd_storage_file_mobile:function(o) {
		this.cmd_storage_popinfo('cmd_storage_file_camera','Add Mobile Files To Asc, Come Soon!');
	},

	cmd_storage_unpin:function(o) {
		this.cmd_storage_popinfo('cmd_storage_unpin','Unpin Capsule, Come Soon!');
	},

	cmd_storage_cancel:function(o) {
		P.setCpo(null);
		P.setCpo(P.HOME);
	},

	cmd_storage_file_back:function(){
		S.delCurPath();
		S.fileItemTypeRender();
	},
	cmd_storage_folder_back:function(){
		S.FOLDER.delCurPath();
		S.FOLDER.typeRender();
	},

	cmd_storage_cat_file:function(o) {
		S.fileItemTypeRender();
	},
	cmd_storage_cat_app:function(o) {
		S.fileItemTypeRender(S.enumFileType['app']);
	},
	cmd_storage_cat_image:function(o) {
		S.fileItemTypeRender(S.enumFileType['picture']);
	},
	cmd_storage_cat_video:function(o) {
		S.fileItemTypeRender(S.enumFileType['video']);
	},
	cmd_storage_cat_music:function(o) {
		S.fileItemTypeRender(S.enumFileType['audio']);
	},
	cmd_storage_cat_doc:function(o) {
		S.fileItemTypeRender(S.enumFileType['document']);
	},
	cmd_storage_cat_fav:function(o) {
		S.fileItemTypeRender(S.enumFileType['fav']);
	},

	cmd_storage_scroll_left:function(o) {
		S.leftScroll();
	},
	cmd_storage_scroll_right:function(o) {
		S.rightScroll();
	},

	cmd_storage_folder_scroll_left:function(o) {
		S.FOLDER.leftScroll();
	},
	cmd_storage_folder_scroll_right:function(o) {
		S.FOLDER.rightScroll();
	},

	cmd_storage_group_scroll_left:function(o) {
		//S.GROUP.leftScroll();
	},
	cmd_storage_group_scroll_right:function(o) {
		//S.GROUP.rightScroll();
	},

	cmd_storage_groupuser_scroll_left:function(o) {
		//S.GROUPMEMBER.leftScroll();
	},
	cmd_storage_groupuser_scroll_right:function(o) {
		//S.GROUPMEMBER.rightScroll();
	},

	cmd_storage_detailuser_scroll_left:function(o) {
		//S.DETAILMEMBER.leftScroll();
	},
	cmd_storage_detailuser_scroll_right:function(o) {
		//S.DETAILMEMBER.rightScroll();
	},

	cmd_storage_popinfo:function(sTitle,sText) {
		P.setCpo(null);

		$("menu_storage_popinfo_title").innerHTML='<span>'+(sTitle||'title')+'</span>';
		$("menu_storage_popinfo_text").innerHTML='<span>'+(sText||'text')+'</span>';

		M.setName("menu_storage_popinfo");

		P.setCpo(M);
	}

};	//return

return T;

})();
