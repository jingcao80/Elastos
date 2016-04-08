TVUI.EVT=(function(){

var _mouse={};
var _keyboard={};
var _touch={};

function _in(aa,ae){
	for(var i=0,im=aa.length;i<im;i++){
		if(aa[i]==ae)return true;
	}
	return false;
}
function _findInCss(p,tag){
	while(p){
		if(p.className&&p.className.indexOf(tag)>-1)return p;
		p=p.parentNode;
	}
	return null;
}

var _oPressTime={
	startTime:null,
	endTime:null,

	setStartTime:function(dt){
		this.startTime=dt;
	},
	setEndTime:function(dt){
		this.endTime=dt;
	},
	getPressTime:function(){
		var b=this.endTime&&this.startTime;
		if(!b)return 0;

		var iPressTime=this.endTime.getTime()-this.startTime.getTime();

		this.startTime=null;
		this.endTime=null;

		return iPressTime;
	}
};

/*----------------------------------------------_oState begin---------------------------------------*/
var _oState=(function(){

var _bSetting=false;

var _p={
	px:0,
	py:0,
	x:0,
	y:0,
	mx:0,	//行
	my:0,	//列
	checkValid:function(){
		var t=this;

		t.x=Math.max(t.x,0);
		t.y=Math.max(t.y,0);
		t.x=Math.min(t.x,_p.mx);
		t.y=Math.min(t.y,_p.my);
	}
};

function _getNextCode(aiKeyCode){
	switch(aiKeyCode){
		case _KB.UP:
			_p.x--;
			break;
		case _KB.DOWN:
			_p.x++;
			break;
		case _KB.LEFT:
			_p.y--;
			//if(!_bSetting&&_p.x==_p.mx)_p.y--;
			break;
		case _KB.RIGHT:
			_p.y++;
			//if(!_bSetting&&_p.x==_p.mx)_p.y++;
			break;
		default:
			//
	}

	_p.checkValid();
}

function _getDomObj(aiKeyCode){
	_getNextCode(aiKeyCode);

	return _oState.oMatrix[_p.px][_p.py][_p.x][_p.y];
}

return {
	oTabs:document.getElementsByClassName('kb'),
	oMatrix:[],

	oFocus:null,
	oBlur:null,

	setMatrix:function(){
		var oTab,sTab;
		var aC,aR,aL,iC,iM;
		var i,im,j,jm;

		for(i=0,im=this.oTabs.length;i<im;i++){
			oTab=this.oTabs[i];
			sTab=oTab.getAttribute('tab');
			if(!sTab){
				//alert('tvui_evit.js.error--no tab');
				continue;
			}

			aL=sTab.replace(/[\[\]]/g,'').split(',');
			for(aR=[],j=0,jm=aL.length;j<jm;j++){
				aC=aL[j].split('-');
				aR.push([parseInt(aC[0]),parseInt(aC.pop())]);
			}

			iC=0;
			iM=aL.length;
			(function(ao){
				if(iC==iM){ao.obj=oTab;return;}

				for (var i=aR[iC][0],im=aR[iC][1];i<=im;i++){
					ao[i]=ao[i]||[];
					iC++;arguments.callee(ao[i]);iC--;
				}
			})(this.oMatrix);
		}

		_p.mx=this.oMatrix[0][0].length -1;
		_p.my=this.oMatrix[0][0][0].length - 1;
	},

	getNextFocusNode:function(aiKeyCode){
		var _t=this||_OState;

		var oTabs,oPrev,oNext;

		oTabs=_t.oTabs
		oPrev=this.getFocus();

		oCurrent=_getDomObj(aiKeyCode);

		if(oCurrent)oCurrent=oCurrent.obj;

		var bGetNext;
		if(aiKeyCode==_KB.UP||aiKeyCode==_KB.DOWN){
			bGetNext=(_p.x>0&&_p.x<_p.mx);
		}else{
			bGetNext=(_p.y>0&&_p.y<_p.my);
		}

		if(oCurrent==oPrev&&bGetNext){
			_oState.getNextFocusNode(aiKeyCode);
			return;
		}
		if(!oCurrent){
			alert('getNextFocusNode:no obj--'+_p.x+'--'+_p.y);
			return;
		}

		_t.setBlur(oPrev);
		_t.setFocus(oCurrent);
	},

	getFocus:function(){
		_bSetting = (TVUI.cpo==TVUI.IC_SETTING);
		if(_bSetting){
			if(_p.px==0){
				_p.px=1;_p.mx=2;_p.my=6;	/*bug:max value to be calculated*/
				_p.checkValid();
			}
		}else{
			if(_p.px==1){
				_p.px=0;_p.mx=6;_p.my=14;	/*bug:max value to be calculated*/
				_p.checkValid();
			}
		}

		this.oFocus =  this.oMatrix[_p.px][_p.py][_p.x][_p.y].obj;

		return this.oFocus;
	},

	setFocus:function(o){
		if(!o)return;

		this.oFocus=o;

		var s=o.className.replace('_out','_over');
		o.className=s;

		if(s.indexOf('_icon_over')>-1){
			var oImg=o.getElementsByTagName('IMG')[0];
			if(oImg.src.indexOf('_d.png')<0){
				oImg.src=oImg.src.replace('.png','_d.png');
			}
		}
	},
	setBlur:function(o){
		if(!o) return;

		this.oBlur=o;

		var s=o.className.replace('_over','_out');
		o.className=s;

		if(s.indexOf('_icon_out')>-1){
			var oImg=o.getElementsByTagName('IMG')[0];
			oImg.src=oImg.src.replace('_d.png','.png');
		}
	},
	setConfirm:function(oTarget){
		oTarget=oTarget||this.oFocus;

		//this.setFocus(oTarget);

		//--------play video when TV clicked.--------
		var op=oTarget;
		while(op&&op.id!='app_tv')op=op.parentNode;
		op&&TVUI.LETV&&TVUI.LETV.play();

		var cmd=TVUI.FN.findByAttr(oTarget,'cmd');
		if(cmd){
			if(cmd=='cmd_ui_shortcutSetting_ret')this.setBlur(oTarget);

			if(_oPressTime.getPressTime()>500&&cmd.indexOf('cmd_ic_setting_')>-1){
				cmd='cmd_shortcutSetting';
			}

			var bEdit=!!document.getElementById('app_ic_setting_edit');

			if(cmd.indexOf('cmd_ic_setting_')==0&&cmd!='cmd_ic_setting_check'){
				_bSetting=(TVUI.cpo==TVUI.IC_SETTING);
				if(_bSetting&&bEdit){
					oTarget=oTarget.getElementsByTagName('IMG')[1]
					cmd=TVUI.FN.findByAttr(oTarget,'cmd');					
				}
			}

			cmd&&TVUI.CMD[cmd]&&TVUI.CMD[cmd](oTarget);
		}
	},
	resume:function(){
	}
};	//return;

})();

/*----------------------------------------------_oState end---------------------------------------*/

var _KB={
	SHUTDOWN:27,
	ESC:27,

	LEFT:37,
	RIGHT:39,

	UP:38,
	DOWN:40,

	ENTER_PC:13,
	ENTER_BOX:14,

	MENU:91,
	BACKSPACE:8,

	HOME:36,

	END:35,

	SPACE:32,

	PGUP:33,
	PGDN:34
}

var T={
	//-------------------------------mouse events-------------------------------
	mousemove:function(e){
		//if(TVUI.DEMO.visible){
		//	TVUI.fps.actHome();
		//	return;
		//}
	},
	mousedown:function(e){
		_oPressTime.setStartTime(new Date());
	},
	mouseup:function(e){
		_oPressTime.setEndTime(new Date());
	},
	mouseover:function(e){
		var oEvt=window.event||e;
		var oTarget=oEvt.srcElement||oEvt.target;

		_oState.setFocus(_findInCss(oTarget,'_out'));

		window.focus();
	},
	mouseout:function(e){
		var oEvt=window.event||e;
		var oTarget=oEvt.srcElement||oEvt.target;

		_oState.setBlur(_findInCss(oTarget,'_over'));
	},
	click:function(e){
		if(TVUI.DEMO.visible){
			TVUI.fps.actHome();
			return;
		}

		var oEvt=window.event||e;
		var oTarget=oEvt.srcElement||oEvt.target;

		_oState.setConfirm(oTarget);
	},

	//-------------------------------touch events-------------------------------
	/*... add code here ...*/

	//-------------------------------keyboard events-------------------------------

	keydown:function(e){

		if(TVUI.DEMO.visible){
			TVUI.fps.actHome();
			return;
		}

		var oEvt=window.event||e;
		var oTarget=oEvt.srcElement||oEvt.target;

		switch(oEvt.keyCode){
			case _KB.BACKSPACE:
				var oHome=document.getElementById('app_ic_setting_btn_confirm');
				oHome&&oHome.click();
				break;
			case _KB.ENTER_PC:
				_oState.setConfirm();
				break;
			case _KB.ENTER_BOX:
				_oState.setConfirm();
				break;
			case _KB.MENU:
				break;
			default:
				if(_in([_KB.UP,_KB.DOWN,_KB.LEFT,_KB.RIGHT],oEvt.keyCode)){
					_oState.getNextFocusNode(oEvt.keyCode);
				}
		}
	},

	initKeyboardState:function(){
		_oState.setMatrix();
	},
	init:function(){
		//
	}
};	//T

return T;

})();

