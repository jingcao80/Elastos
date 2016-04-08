TVUI.LETV=(function(){

var oImgs=[
	document.getElementById('app_tv_icons_00'),
	document.getElementById('app_tv_icons_01'),
	document.getElementById('app_tv_icons_02'),
	document.getElementById('app_tv_icons_03')
];

var bSleep = true;

var canvas;
var ctx;

var aImages = [];
var oImgBgPat;
var oImgBg;

var points = [];
var triangles = [];

var textureWidth, textureHeight;
var lev = 3;
var angle = 0;
var step = 0.6;

// scene faces (6 faces)
var faces  = [
	[0,1,2,3],
	[1,5,6,2],
	[5,4,7,6],
	[4,0,3,7],
	[0,4,5,1],
	[3,2,6,7]
];

// scene vertices
var vertices = [
	new Point3D(-2,-1,2),
	new Point3D(2,-1,2),
	new Point3D(2,1,2),
	new Point3D(-2,1,2),
	new Point3D(-2,-1,-2),
	new Point3D(2,-1,-2),
	new Point3D(2,1,-2),
	new Point3D(-2,1,-2)
];


function Point3D(x,y,z) {
	this.x = x;
	this.y = y;
	this.z = z;
}	//Point3D

Point3D.prototype.rotateX = function(angle) {
	var rad, cosa, sina;
	var x,y,z;

	rad = angle * Math.PI / 180;
	cosa = Math.cos(rad);
	sina = Math.sin(rad);

	x = this.x;
	y = this.y * cosa - this.z * sina;
	z = this.y * sina + this.z * cosa;

	return new Point3D(x, y, z);
}
Point3D.prototype.rotateY = function(angle) {
	var rad, cosa, sina, x, y, z;

	rad = angle * Math.PI / 180;
	cosa = Math.cos(rad);
	sina = Math.sin(rad);

	x = this.x * cosa + this.z * sina ;
	y = this.y;
	z = - this.x * sina + this.z * cosa;

	return new Point3D(x,y, z);
}
Point3D.prototype.rotateZ = function(angle) {
	var rad, cosa, sina, x,y,z;

	rad = angle * Math.PI / 180;
	cosa = Math.cos(rad);
	sina = Math.sin(rad);
	x = this.x * cosa - this.y * sina;
	y = this.x * sina + this.y * cosa;
	z = this.z;

	return new Point3D(x, y, z);
}
Point3D.prototype.projection = function(viewWidth, viewHeight, fov, viewDistance) {
	var factor, x, y

	factor = fov / (viewDistance + this.z)
	x = this.x * factor + viewWidth / 2
	y = this.y * factor + viewHeight / 2

	return new Point3D(x, y, this.z)
}

// array of photos

function _loadImg(){
	var oImg;
	var _num=0;

	var aImgs = [
		'3dshow/images/pic1.png',
		'3dshow/images/pic2.png',
		'3dshow/images/pic3.png',
		'3dshow/images/pic4.png'
	];
	//var sImgBg = '3dshow/images/bg_blue.jpg';
	var sImgBg = '3dshow/images/pic1.png';

	function _imgLoaded(){
		_num++;
		if(_num>aImgs.length){
			_onload();
		}
	}

	for (var i = 0; i < aImgs.length; i++) {
		oImg = new Image();
		oImg.src = aImgs[i];
		aImages.push(oImg);

		oImg.onload = function () {
			textureWidth = oImg.width;
			textureHeight = oImg.height;

			_imgLoaded();
		}
	}

	oImgBg = new Image();
	oImgBg.onload = function () {
		_imgLoaded();
	};
	oImgBg.src = sImgBg;

};	//_loadImg


function _drawScene() {
	if(bSleep)return;

	if(!ctx)return;

	var ia,cnt1,cnt0;

	/*get current icon number*/
	ia=Math.ceil(angle)%360;
	cnt1=Math.max(Math.ceil(ia/90)-1,0);
	//cnt0=(cnt1+3)%4;
	if(TVUI.LETV.curcnt!=cnt1){
		cnt0=TVUI.LETV.curcnt;
		TVUI.LETV.curcnt=cnt1;
		document.getElementById('app_tv_icons_0'+cnt1).src='3dshow/images/icons_01.png';
		document.getElementById('app_tv_icons_0'+cnt0).src='3dshow/images/icons_00.png';

		var oo=document.getElementById('app_tv_show_cont_imgs');
		oo.style.left=-TVUI.LETV.curcnt*100+'%';
	}

if(1==1){
}else{
	// clear context
	ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

	//ctx.drawImage(oImgBg, 0, 0);

	// rotate scene
	var t = new Array();
	for (var iv = 0; iv < vertices.length; iv++) {
		var v = vertices[iv];

		//var r=v;
		//var r = v.rotateX(angle);
		var r = v.rotateY(angle);
 		//var r = v.rotateZ(angle);

		//var r = v.rotateX(angle).rotateY(angle/2);
		//var r = v.rotateX(angle).rotateY(angle);
		//var r = v.rotateX(angle).rotateY(angle).rotateZ(angle);

		var prj = r.projection(ctx.canvas.width, ctx.canvas.height, 1000, 3);
		//var prj = r.projection(ctx.canvas.width, ctx.canvas.height, 600,3);
		t.push(prj)
	}

	var avg_z = new Array();
	for (var i = 0; i < faces.length; i++) {
		var f = faces[i];
		avg_z[i] = {"ind":i, "z":(t[f[0]].z + t[f[1]].z + t[f[2]].z + t[f[3]].z) / 4.0};
	}

	// get around through all faces
	for (var i = 0; i < faces.length; i++) {
		var f = faces[avg_z[i].ind];
		//var f = faces[i];

		if (t[f[3]].z+t[f[2]].z+t[f[1]].z+t[f[0]].z > -3) {

			// draw surfaces
			//if(1==2){	//----bak begin----
			if(i>3){	//----bak begin----
				ctx.save();

				ctx.fillStyle = "rgba(160,180,160,0.6)";
				//ctx.fillStyle = "rgba(0,255,255,0.6)";

				//oImgBgPat=oImgBgPat||ctx.createPattern(oImgBg,"repeat");
				//ctx.fillStyle = oImgBgPat;

				//ctx.rotate(angle * Math.PI / 180);
				//ctx.scale(10,10);
				//ctx.translate(angle*10,angle*10);
				//ctx.globalAlpha=0.3;

				ctx.beginPath();
				ctx.moveTo(t[f[0]].x,t[f[0]].y);
				ctx.lineTo(t[f[1]].x,t[f[1]].y);
				ctx.lineTo(t[f[2]].x,t[f[2]].y);
				ctx.lineTo(t[f[3]].x,t[f[3]].y);
				ctx.closePath();
				ctx.fill();

				ctx.restore();
			}	//----bak end----

			// draw stretched images
			if (i < 4) {
				var ip = points.length;
				while (--ip > -1) {
					var p = points[ip];
					var mx = t[f[0]].x + p.ny * (t[f[3]].x - t[f[0]].x);
					var my = t[f[0]].y + p.ny * (t[f[3]].y - t[f[0]].y);
					p.px = (mx + p.nx * (t[f[1]].x + p.ny * (t[f[2]].x - t[f[1]].x) - mx)) + p.ox;
					p.py = (my + p.nx * (t[f[1]].y + p.ny * (t[f[2]].y - t[f[1]].y) - my)) + p.oy;
				}

				var n = triangles.length;
				while (--n > -1) {
					var tri = triangles[n];
					var p0 = tri.p0;
					var p1 = tri.p1;
					var p2 = tri.p2;

					var xc = (p0.px + p1.px + p2.px) / 3;
					var yc = (p0.py + p1.py + p2.py) / 3;

					ctx.save();

					ctx.beginPath();
					ctx.moveTo((1.05 * p0.px - xc * 0.05), (1.05 * p0.py - yc * 0.05));
					ctx.lineTo((1.05 * p1.px - xc * 0.05), (1.05 * p1.py - yc * 0.05));
					ctx.lineTo((1.05 * p2.px - xc * 0.05), (1.05 * p2.py - yc * 0.05));
					ctx.closePath();

					ctx.clip();

					// transformation
					var d = p0.tx * (p2.ty - p1.ty) - p1.tx * p2.ty + p2.tx * p1.ty + (p1.tx - p2.tx) * p0.ty;

					ctx.transform(
						-(p0.ty * (p2.px - p1.px) -  p1.ty * p2.px  + p2.ty *  p1.px + (p1.ty - p2.ty) * p0.px) / d, // m11
						(p1.ty *  p2.py + p0.ty  * (p1.py - p2.py) - p2.ty *  p1.py + (p2.ty - p1.ty) * p0.py) / d, // m12
						(p0.tx * (p2.px - p1.px) -  p1.tx * p2.px  + p2.tx *  p1.px + (p1.tx - p2.tx) * p0.px) / d, // m21
						-(p1.tx *  p2.py + p0.tx  * (p1.py - p2.py) - p2.tx *  p1.py + (p2.tx - p1.tx) * p0.py) / d, // m22
						(p0.tx * (p2.ty * p1.px  -  p1.ty * p2.px) + p0.ty * (p1.tx *  p2.px - p2.tx  * p1.px) + (p2.tx * p1.ty - p1.tx * p2.ty) * p0.px) / d, // dx
						(p0.tx * (p2.ty * p1.py  -  p1.ty * p2.py) + p0.ty * (p1.tx *  p2.py - p2.tx  * p1.py) + (p2.tx * p1.ty - p1.tx * p2.ty) * p0.py) / d  // dy
					);

					ctx.drawImage(aImages[i], 0, 0);

					ctx.restore();
				}
			}
		}
	}
}

	// shift angle and redraw scene
	angle += step;
	setTimeout(_drawScene, 40);
};	//_drawScene

function _onload(){
	TVUI.bAllReady=true;

	//creating canvas objects
	canvas = document.getElementById('app_tv_show');

	if(!canvas){
		var oo=document.getElementById('app_tv_show_cont');
		var iw=oo.offsetWidth;
		var ih=oo.offsetHeight;
		if(iw==0){
			setTimeout(arguments.callee,1000);
			return;
		}else{
			//alert(iw+'--'+ih);
		}

		oo.innerHTML=[
			'<canvas id="app_tv_show" width="',
			iw,
			'px" height="',
			ih,
			'px"></canvas>'
		].join('');
		canvas = document.getElementById('app_tv_show');
	}

	ctx = canvas.getContext('2d');

	/*----test begin----*/
	ctx.translate(170,100);
	ctx.scale(0.6,0.6);

	//ctx.translate(270,200);
	//ctx.scale(0.03,0.03);

	ctx.globalAlpha=1;
	/*----test end------*/

	// prepare points
	for (var i = 0; i <= lev; i++) {
		for (var j = 0; j <= lev; j++) {
			var tx = (i * (textureWidth / lev));
			var ty = (j * (textureHeight / lev));

			points.push({
				tx: tx,
				ty: ty,
				//nx: tx / textureWidth,
				nx: i / lev,
				//ny: ty / textureHeight,
				ny: j / lev,
				ox: i,
				oy: j,

				px: null,
				py: null
			});
		}
	}

	// prepare triangles ----
	var levT = lev + 1;
	for (var i = 0; i < lev; i++) {
		for (var j = 0; j < lev; j++) {
			triangles.push({
				p0: points[j + i * levT],
				p1: points[j + i * levT + 1],
				p2: points[j + (i + 1) * levT],
				up: true
			});

			triangles.push({
				p0: points[j + (i + 1) * levT + 1],
				p1: points[j + (i + 1) * levT],
				p2: points[j + i * levT + 1],
				up: false
			});
		}
	}

	_play();

	//TVUI.bAllReady=true;
};	//_onload

function _play(){
	if(!bSleep)return;	//no repeat run.

	if(!ctx)return;

	bSleep=false;
	_drawScene();
};
function _stop(){
	bSleep=true;
};
function _init(){
	_loadImg();
};

return {
	curcnt:1,
	videos:[
		'video1.avi',
		'video2.avi',
		'video3.avi',
		'video4.avi'
	],

	setAngle:function(aiAngle){
		angle=aiAngle;
	},
	getCurVideoName:function(){
		return TVUI.CFG.defaultVideoPath+this.videos[this.curcnt];
	},
	play:function(){
		_play();
	},
	stop:function(){
		_stop();
	},
	init:function(){
		_init();
	}
};

})();

