TVUI.fps=(function(){			//----fps begin----
	var _obj=document.getElementById('FPS');

	var _num=0;

	var _freeTimes=0;
	//var _maxFreeTimes=300;
	//var _maxFreeTimes=5;

	//var _loadingTimes=5;
	var _loadingTimes=0;

	var _bDemo=true;

	var _loading=true;

	function _render() {
		if(!_obj)return;
		_obj.innerHTML = _num + ' FPS';
	}

	return {
		run : function(){
			_num++;
		},
		psRun : function () {
			_render();
			_num = 0;

			_freeTimes++;

			if(_loading){
				if(_freeTimes>=_loadingTimes){
					_loading=false;
					_freeTimes=0;
					_bDemo=false;
					TVUI.showDemo(_bDemo);
				}

			}else{
				if(_freeTimes>TVUI.CFG.maxFreeTimes){
					if(!_bDemo){
						_bDemo=true;
						TVUI.showDemo(_bDemo);
					}
				}

			}

		},
		show:function(bVisible){
			_obj.style.display=bVisible?'':'none';
		},
		actHome : function(){
			if(_loading)return;

			_freeTimes=0;
			_bDemo=false;
			TVUI.showDemo(_bDemo);
		}
	}
})();					//----fps end----
