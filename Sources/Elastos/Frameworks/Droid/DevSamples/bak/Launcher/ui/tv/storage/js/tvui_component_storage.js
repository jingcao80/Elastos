
TVUI.COMPONENTS.clsGrid=(function(){
	function fnRet(sClassName){
		this.init(sClassName);
	}

	fnRet.prototype.gridRender=function(o){
		var sGrid=o.getAttribute('grid');
		if(!sGrid)return;

		sGrid=sGrid.replace(/'/g,'"');
		var oGrid=JSON.parse(sGrid);

		o=o.childNodes[1];

		var sRowHTML=o.outerHTML.replace(/: /g,':');
		var sColHTML=o.innerHTML.replace(/: /g,':');
		var aRowHTML=sRowHTML.split(sColHTML);

		var aColHTML=sColHTML.split('%%');
		var imRow=oGrid.tab_row.length;
		var imColumn=oGrid.tab_column.length;

		var iCellWidth=Math.floor(100/imColumn);

		var ss,iHeight,iTop;
		var aGridHTML=[];
		for(var iRow=0;iRow<imRow;iRow++){
			iHeight=Math.floor(100/imRow);
			iTop=iHeight*iRow;

			ss=aRowHTML[0].replace('height:0%','height:'+iHeight+'%').replace('top:0%','top:'+iTop+'%');
			aGridHTML.push(ss);

			for(var iColumn=0;iColumn<imColumn;iColumn++){
				aGridHTML.push(aColHTML[0]);

				var aTmp=aColHTML[1].split('_');
				aTmp.pop();aTmp.pop();
				aTmp.push(iRow);aTmp.push(iColumn);
				//aGridHTML.push(aColHTML[1].split('_').shift()+'_'+iRow+'_'+iColumn);
				aGridHTML.push(aTmp.join('_'));

				ss=aColHTML[2].replace('left:0%','left:'+(iColumn*iCellWidth)+'%');
				ss=ss.replace('width:0%','width:'+iCellWidth+'%');
				aGridHTML.push(ss);

				ss=oGrid.tab_par+',['+oGrid.tab_row[iRow]+','+oGrid.tab_column[iColumn]+']';
				aGridHTML.push(ss);

				aGridHTML.push(aColHTML[4]);
			}

			aGridHTML.push(aRowHTML[1]);
		}

		o.parentNode.innerHTML=aGridHTML.join('');
	}

	fnRet.prototype.init=function(as_ClassName){
		this.classname=as_ClassName;

		var aDomGrid=document.getElementsByClassName(as_ClassName);
		for(var i=0,im=aDomGrid.length;i<im;i++)this.gridRender(aDomGrid[i]);
	}

	fnRet.instances=[];
	fnRet.createInstance=function(sClassName){
		this.instances.push(new this(sClassName));
	}
	fnRet.createInstances=function(a){
		for(var i=0,im=a.length;i<im;i++)this.createInstance(a[i]);
	}

	return fnRet;
})();		/*--------clsGrid end--------*/


/*-------------------------------end-------------------------------------*/
