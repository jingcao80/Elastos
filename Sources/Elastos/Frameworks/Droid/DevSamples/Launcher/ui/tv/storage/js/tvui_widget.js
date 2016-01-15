TVUI.WGT={

init:function(){
	//TVUI.WGT.wgtWeather.run();
	TVUI.WGT.wgtDateTime.run();
}

};

TVUI.WGT.wgtDateTime=(function(){

var num=0;
var mw=0;
var mh=0;

return {

sid:'app_wgt_datetime',

font:16,

setFont:function(){
/*
	num++;
	//if(num==1)alert('ss--'+document.body.offsetWidth+'--'+document.body.offsetHeight+'--'+this.font);

	var imw=document.body.offsetWidth;
	var imh=document.body.offsetHeight;

	var o=document.getElementById('app_wgt_weather');
	var iLeft=o.offsetLeft+o.offsetWidth+20;

	o=document.getElementById('app_wgt_datetime');
	o.style.left=iLeft+'px';

	//iLeft=o.offsetLeft+o.offsetWidth+20;

	//o=document.getElementById('bar_top_01');
	//o.style.left=iLeft+'px';

//if(num==1)
//alert(iLeft);
*/
	this.font=16;
},

render:function(){

	this.setFont();

	var oDT=document.getElementById(this.sid);
	if(!oDT)return;

	var oDate=new Date();
	var sDate=oDate.getFullYear()+'/'+(oDate.getMonth()+1)+'/'+oDate.getDate();
	var sWeekDay='星期'+['日','一','二','三','四','五','六'][oDate.getDay()];
	var sTime=('0'+oDate.getHours()).slice(-2)+':'+('0'+oDate.getMinutes()).slice(-2)+':'+('0'+oDate.getSeconds()).slice(-2);

	var aHTML=[];

	aHTML.push('<div style="left:5%;top:5%;font-size:'+this.font+'px;">');
	aHTML.push(sDate);
	aHTML.push('</div>');

	aHTML.push('<div style="left:65%;top:5%;font-size:'+this.font+'px;">');
	aHTML.push(sWeekDay);
	aHTML.push('</div>');

	aHTML.push('<div style="left:5%;top:50%;font-size:'+this.font+'px;">');
	aHTML.push(sTime);
	aHTML.push('</div>');

	oDT.innerHTML=aHTML.join('');
},	//render

run:function(){
	TVUI.WGT.wgtDateTime.render();

	setTimeout(arguments.callee,1000);
}	//run

};	//return

})();	//TVUI.WGT.wgtDateTime

TVUI.WGT.wgtWeather=(function(){

return {

sid:'app_wgt_weather',

render:function(){
	var oWT=document.getElementById(this.sid);
	if(!oWT)return;

	var aHTML=[];

	aHTML.push('<div style="left:-40px;top:0px;width:220px;height:100%;">');
	aHTML.push('	<script type="text/javascript" src="http://ext.weather.com.cn/17259.js"></script>');
	aHTML.push('</div>');

	aHTML.push('<div style="width:100%;height:100%;background:rgba(0,0,256,0.3)"></div>');

	oWT.innerHTML=aHTML.join('');
},
run:function(){
	TVUI.WGT.wgtWeather.render();
}

}	//return

})();	//TVUI.WGT.wgtWeather

//alert('tvui_widget OK');
