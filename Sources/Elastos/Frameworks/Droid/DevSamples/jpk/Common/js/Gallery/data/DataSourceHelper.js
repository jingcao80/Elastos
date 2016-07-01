module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

return {

//public:
SetRootPath : function (asRootPath) {
    elog('=============DataSourceHelper::SetRootPath.begin===============');
    this.ClearAll();
    this.sRootPath = asRootPath;
    elog('=============DataSourceHelper::SetRootPath.end===============');
},
GetImageFileMap : function () {
    elog('=============DataSourceHelper::GetImageFileMap.begin===============');
    this.Init();
    elog('=============DataSourceHelper::GetImageFileMap.end===============');
    return this.sImageSourcePathMap;
},
GetImagePathList : function () {
    elog('=============DataSourceHelper::GetImagePathList.begin===============');
    this.Init();
    elog('=============DataSourceHelper::GetImagePathList.begin=======1========');
    if (this.sPath.length > 1) {
        elog('=============DataSourceHelper::GetImagePathList.begin=======2========');
        this.SortListByTime();
        elog('=============DataSourceHelper::GetImagePathList.begin=======3========');
    }
    elog('=============DataSourceHelper::GetImagePathList.end===============');
    return this.sPath;
},
GetItemList : function (asKey) {
    elog('=============DataSourceHelper::GetItemList.begin===============');
    this.Init();
    var ret = this.sImageSourcePathMap[asKey];
    elog('=============DataSourceHelper::GetItemList.end===============');
    return ret;
},
TimeComparator : function (lhs, rhs) {

    elog('=============DataSourceHelper::TimeComparator.begin==============='+typeof(this.sCurrentPath));

    elog('=============DataSourceHelper::TimeComparator.begin======0.0=========path:'+this.sCurrentPath);
    var strL = this.sCurrentPath + lhs;
    var strR = this.sCurrentPath + rhs;
    elog('=============DataSourceHelper::TimeComparator.begin======0.1=========strL:'+strL);
    var file1 = Core_New('Elastos.IO.CFile', strL);
    elog('=============DataSourceHelper::TimeComparator.begin======0.2=========strR:'+strR);
    var file2 = Core_New('Elastos.IO.CFile', strR);
    elog('=============DataSourceHelper::TimeComparator.begin======0.3=========');
    var last1 = file1.GetLastModified();
    elog('=============DataSourceHelper::TimeComparator.begin======0.4=========');
    var last2 = file2.GetLastModified();
    elog('=============DataSourceHelper::TimeComparator.begin======1=========');

    var diff = last1 - last2;

    if (diff > 0) return 1;
    if (diff < 0) return 0;

    elog('=============DataSourceHelper::TimeComparator.begin======2=========');

    var result = 0;
    elog('=============DataSourceHelper::TimeComparator.begin======2.1========='+typeof(this.sCurrentPath));
    if (this.sCurrentPath.length > 0) {
        elog('=============DataSourceHelper::TimeComparator.begin======3=========');
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    else {
        elog('=============DataSourceHelper::TimeComparator.begin======4=========');
        var lhsDir = file1.GetParent();
        elog('=============DataSourceHelper::TimeComparator.begin======5=========');
        var rhsDir = file2.GetParent();
        elog('=============DataSourceHelper::TimeComparator.begin======6=========');
        var lhsSub = lhsDir.split(this.PATH_SPLITE).pop();
        var rhsSub = rhsDir.split(this.PATH_SPLITE).pop();
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    var ret = result > 0 ? 1 : 0;

    elog('=============DataSourceHelper::TimeComparator.end===============');
    return ret;
},
IsImageFile : function (asFileName) {
    elog('=============DataSourceHelper::IsImageFile.begin===============');
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
    elog('=============DataSourceHelper::IsImageFile.end===============');
},

//private:
Init : function () {
    elog('=============DataSourceHelper::Init.begin===============');

    if (this.sInited) return;

    var dir = Core_New('Elastos.IO.CFile', this.sRootPath);

    var ret = dir.IsDirectory();
    if (!ret) {
        elog("sRootPath:" + this.sRootPath + " is not a valid directory!");
    }

    this._GetImageFileMap(this.sRootPath);

    this.sInited = true;

    elog('=============DataSourceHelper::Init.end===============');
},
_GetImageFileMap : function (rootPath) {
    elog('=============DataSourceHelper::_GetImageFileMap.begin===============');

    var dir = Core_New('Elastos.IO.CFile', rootPath);
    elog('=============DataSourceHelper::_GetImageFileMap.begin=======0.1========');
    var files = dir.ListFiles();
    elog('=============DataSourceHelper::_GetImageFileMap.begin=======0.2========');
    if (!files || !files.length) return;

    elog('=============DataSourceHelper::_GetImageFileMap.begin=======0.3========');

    var itemList = [];
    for (var i=0, im=files.length; i<im; i++) {
        var file = files[i];
        if (!file.CanRead()) continue;

        if (file.IsDirectory()) {
            var absPath = file.GetAbsolutePath();
            elog('=============DataSourceHelper::_GetImageFileMap.subdir.begin===============');
            this._GetImageFileMap(absPath);
            elog('=============DataSourceHelper::_GetImageFileMap.subdir.end===============');
        }
        else {
            var name = file.GetName();
            if (name) itemList.push(name);
        }
    }

    elog('=============DataSourceHelper::_GetImageFileMap.begin======1========='+JSON.stringify(itemList));
    elog('=============DataSourceHelper::_GetImageFileMap.begin======1.1========='+itemList);

    if (itemList.length > 0) {
        this.sCurrentPath = rootPath + this.PATH_SPLITE;
    elog('=============DataSourceHelper::_GetImageFileMap.begin======2=========');
        itemList.sort(this.TimeComparator.bind(this));
    elog('=============DataSourceHelper::_GetImageFileMap.begin======3========='+itemList);

        this.sImageSourcePathMap[rootPath] = itemList;
        this.sPath.push(rootPath);

    }

    elog('=============DataSourceHelper::_GetImageFileMap.end========'+this.sImageSourcePathMap);
},
ClearAll : function () {  //~DataSourceHelper()
    elog('=============DataSourceHelper::ClearAll.begin===============');
    this.sPath = [];
    this.sImageSourcePathMap = {};
    this.sInited = false;
    elog('=============DataSourceHelper::ClearAll.end===============');
},
SortListByTime : function () {
    elog('=============DataSourceHelper::SortListByTime.begin===============');
    var pathMap = this.sImageSourcePathMap;
    var pathList = this.sPath;

    //elog('=============DataSourceHelper::SortListByTime.begin======0.1========='+JSON.stringify(pathList));
    //elog('=============DataSourceHelper::SortListByTime.begin======0.2========='+JSON.stringify(pathMap));

    var albumList = [];
    for (var i=0, im=pathList.length; i<im; i++) {
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.01========');
        var path = pathList[i];
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.02========');
        var fileList = pathMap[path];
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.03========');
        path += this.PATH_SPLITE;
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.04========');
        path += fileList[0];
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.05========');
        albumList.push(path);
        //elog('=============DataSourceHelper::SortListByTime.begin=======0.06========');
    }

    elog('=============DataSourceHelper::SortListByTime.begin=======1========'+JSON.stringify(albumList));

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
    elog('=============DataSourceHelper::SortListByTime.end==============='+JSON.stringify(this.sPath));
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
