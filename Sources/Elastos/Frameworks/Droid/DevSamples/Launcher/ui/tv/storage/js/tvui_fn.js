function DivCanvas(sid,src,bSquare,alpha){

	sid=sid||"bg_main";

	var _par,_cvs,_ctx;
	var mw,mh;
	var img,iw,ih;

	var pw,ph;

	var alpha;

	var _bLoaded=false;
	var _bSquare=bSquare||false;

	var _flag=1;

	var _delay=0;

	var position={
		x:0,y:0,w:0,h:0
	};

	this._init=function(){
		_par=sid;
		if(typeof sid=='string')_par=document.getElementById(_par);
		if(!_par)return;

		alpha=alpha||parseFloat(_par.getAttribute('alpha'))||1;

		_cvs=document.createElement('canvas');
		_par.appendChild(_cvs);
		_ctx=_cvs.getContext('2d');

		img=new Image();
		img.src=src||_par.getAttribute("src");

		img.onload=function(){
			_bLoaded=true;

			iw=img.width;
			ih=img.height;

			_resize();
		}
	}

	function _resize(){
		if(!_bLoaded)return;

		if(_delay>0){
			_delay--;
			return;
		}

		var imw=_par.offsetWidth;
		var imh=_par.offsetHeight;
		if(_bSquare){
			var i=Math.min(imw,imh);
			imw=i;imh=i;
		}

		if(imw==mw&&imh==mh&&sid=="bg_main")return;

		mw=imw;
		mh=imh;

		if(!pw){
			pw=mw;ph=mh;
		}else{

			if(_bSquare){
				mw=Math.ceil(pw*(0.5+alpha));
				mh=Math.ceil(ph*(0.5+alpha));
				_par.style.width=mw+'px';
				_par.style.height=mh+'px';
			}
		}
		_cvs.setAttribute('width',mw+'px');
		_cvs.setAttribute('height',mh+'px');

		var cw=_cvs.width;
		var ch=_cvs.height;
		var rw=cw/iw;
		var rh=ch/ih;

		if(_bSquare){
			rw=rh;
		}

		_ctx.scale(rw,rh);

		if(sid!="bg_main"){	//dynamic demo
			if(alpha==1||alpha==0){
				_delay=20;
				_flag*=-1;
			}
			alpha+=_flag*0.01;
			alpha=Math.min(1,alpha);
			alpha=Math.max(0,alpha);

			_ctx.globalAlpha=alpha;
		}


		_ctx.drawImage(img,0,0);
	}

	this.resize=_resize;
}

DivCanvas.prototype.init=function(){
	this._init();
}
DivCanvas.prototype.run=function(){
	this.resize();
}

//alert('tvui_fn OK');
