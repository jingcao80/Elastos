module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

return {

//public:
SetRootPath : function (asRootPath) {
    whmlog('=============DataSourceHelper::SetRootPath.begin===============');
    this.ClearAll();
    this.sRootPath = asRootPath;
    whmlog('=============DataSourceHelper::SetRootPath.end===============');
},
GetImageFileMap : function () {
    whmlog('=============DataSourceHelper::GetImageFileMap.begin===============');
    this.Init();
    whmlog('=============DataSourceHelper::GetImageFileMap.end===============');
    return this.sImageSourcePathMap;
},
GetImagePathList : function () {
    whmlog('=============DataSourceHelper::GetImagePathList.begin===============');
    this.Init();
    whmlog('=============DataSourceHelper::GetImagePathList.begin=======1========');
    if (this.sPath.length > 1) {
        whmlog('=============DataSourceHelper::GetImagePathList.begin=======2========');
        this.SortListByTime();
        whmlog('=============DataSourceHelper::GetImagePathList.begin=======3========');
    }
    whmlog('=============DataSourceHelper::GetImagePathList.end===============');
    return this.sPath;
},
GetItemList : function (asKey) {
    whmlog('=============DataSourceHelper::GetItemList.begin===============');
    this.Init();
    var ret = this.sImageSourcePathMap[asKey];
    whmlog('=============DataSourceHelper::GetItemList.end===============');
    return ret;
},
TimeComparator : function (lhs, rhs) {

    whmlog('=============DataSourceHelper::TimeComparator.begin==============='+typeof(this.sCurrentPath));

    strL = this.sCurrentPath + lhs;
    strR = this.sCurrentPath + rhs;
    var file1 = Core_New('CFile', strL);
    var file2 = Core_New('CFile', strR);
    var last1 = file1.LastModified();
    var last2 = file2.LastModified();
    whmlog('=============DataSourceHelper::TimeComparator.begin======1=========');

    var diff = last1 - last2;

    if (diff > 0) return 1;
    if (diff < 0) return 0;

    whmlog('=============DataSourceHelper::TimeComparator.begin======2=========');

    var result = 0;
    whmlog('=============DataSourceHelper::TimeComparator.begin======2.1========='+typeof(this.sCurrentPath));
    if (this.sCurrentPath.length > 0) {
        whmlog('=============DataSourceHelper::TimeComparator.begin======3=========');
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    else {
        whmlog('=============DataSourceHelper::TimeComparator.begin======4=========');
        var lhsDir = file1.GetParent();
        whmlog('=============DataSourceHelper::TimeComparator.begin======5=========');
        var rhsDir = file2.GetParent();
        whmlog('=============DataSourceHelper::TimeComparator.begin======6=========');
        var lhsSub = lhsDir.split(this.PATH_SPLITE).pop();
        var rhsSub = rhsDir.split(this.PATH_SPLITE).pop();
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    var ret = result > 0 ? 1 : 0;

    whmlog('=============DataSourceHelper::TimeComparator.end===============');
    return ret;
},
IsImageFile : function (asFileName) {
    whmlog('=============DataSourceHelper::IsImageFile.begin===============');
    var result = false;

    var a = asFileName.split('.');
    if (a.length < 2) {
        result = false;
    }
    else {
        var end = '.' + a.pop();
        result = '.jpg.png.jpeg.bmp.webp'.indexOf(end) >= 0;
    }

    return result;
    whmlog('=============DataSourceHelper::IsImageFile.end===============');
},

//private:
Init : function () {
    whmlog('=============DataSourceHelper::Init.begin===============');
    if (this.sInited) return;

    whmlog('=============DataSourceHelper::Init.begin=====1=========='+this.sRootPath+'=='+Core_New);
    var dir = Core_New('CFile', this.sRootPath);
    whmlog('=============DataSourceHelper::Init.begin=====1==========');
    var ret = dir.IsDirectory();
    whmlog('=============DataSourceHelper::Init.begin=====2==========');
    if (!ret) {
        whmlog("sRootPath:" + this.sRootPath + " is not a valid directory!");
    }
    whmlog('=============DataSourceHelper::Init.begin=====3==========');

    this._GetImageFileMap(this.sRootPath);
    whmlog('=============DataSourceHelper::Init.begin=====4==========');

    this.sInited = true;
    whmlog('=============DataSourceHelper::Init.end===============');
},
_GetImageFileMap : function (rootPath) {
    whmlog('=============DataSourceHelper::_GetImageFileMap.begin===============');
    var dir = Core_New('CFile', rootPath);
    var files = dir.ListFiles();
    if (!files || !files.length) return;

    var itemList = [];
    for (var i=0, im=files.length; i<im; i++) {
        var file = files[i];
        if (!file.CanRead()) continue;

        if (file.IsDirectory()) {
            var absPath = file.GetAbsolutePath();
            whmlog('=============DataSourceHelper::_GetImageFileMap.subdir.begin===============');
            this._GetImageFileMap(absPath);
            whmlog('=============DataSourceHelper::_GetImageFileMap.subdir.end===============');
        }
        else {
            var name = file.GetName();
            if (name) itemList.push(name);
        }
    }

    whmlog('=============DataSourceHelper::_GetImageFileMap.begin======1========='+JSON.stringify(itemList));
    whmlog('=============DataSourceHelper::_GetImageFileMap.begin======1.1========='+itemList);

    if (itemList.length > 0) {
        this.sCurrentPath = rootPath + this.PATH_SPLITE;
    whmlog('=============DataSourceHelper::_GetImageFileMap.begin======2=========');
        itemList.sort(this.TimeComparator.bind(this));
    whmlog('=============DataSourceHelper::_GetImageFileMap.begin======3========='+itemList);

        this.sImageSourcePathMap[rootPath] = itemList;
        this.sPath.push(rootPath);

    }
    whmlog('=============DataSourceHelper::_GetImageFileMap.end========1======='+this.sImageSourcePathMap);
    whmlog('=============DataSourceHelper::_GetImageFileMap.end========2======='+JSON.stringify(this.sImageSourcePathMap));
    //aa = bb;
},
ClearAll : function () {  //~DataSourceHelper()
    whmlog('=============DataSourceHelper::ClearAll.begin===============');
    this.sPath = [];
    this.sImageSourcePathMap = {};
    this.sInited = false;
    whmlog('=============DataSourceHelper::ClearAll.end===============');
},
SortListByTime : function () {
    whmlog('=============DataSourceHelper::SortListByTime.begin===============');
    var pathMap = this.sImageSourcePathMap;
    var pathList = this.sPath;

    //whmlog('=============DataSourceHelper::SortListByTime.begin======0.1========='+JSON.stringify(pathList));
    //whmlog('=============DataSourceHelper::SortListByTime.begin======0.2========='+JSON.stringify(pathMap));

    var albumList = [];
    for (var i=0, im=pathList.length; i<im; i++) {
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.01========');
        var path = pathList[i];
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.02========');
        var fileList = pathMap[path];
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.03========');
        path += this.PATH_SPLITE;
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.04========');
        path += fileList[0];
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.05========');
        albumList.push(path);
        //whmlog('=============DataSourceHelper::SortListByTime.begin=======0.06========');
    }

    whmlog('=============DataSourceHelper::SortListByTime.begin=======1========'+JSON.stringify(albumList));

    this.sCurrentPath = '';
    albumList.sort(this.TimeComparator.bind(this));
    this.sPath = [];
    for (var i = 0, im = albumList.length; i < im; i++) {
        var path = albumList[i];
        var aa = path.split(this.PATH_SPLITE);
        aa.pop();
        var end = aa.join(this.PATH_SPLITE);
        this.sPath.push(end);
    }
    whmlog('=============DataSourceHelper::SortListByTime.end==============='+JSON.stringify(this.sPath));
},

//public:
ROOT_PATH : "/data/temp/testdisk",
PATH_SPLITE : "/",
SOURCE_PATH : "source_path",
SOURCE_DESC : "source_desc",
SOURCE_INDEX : "source_index",

//private:
TAG : "DataSourceHelper",
sVersion : 0,
sRootPath : '',
sCurrentPath : '',
sInited : false,

sPath : [], //['']
sImageSourcePathMap : {},   //{a:['b']}

};  //return

};  //module.exports
