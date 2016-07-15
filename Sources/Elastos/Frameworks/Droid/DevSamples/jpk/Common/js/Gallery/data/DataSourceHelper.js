module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

var DataSourceHelper =  {

//public:
SetRootPath : function (asRootPath) {
    this.ClearAll();
    this.sRootPath = asRootPath;
},
GetImageFileMap : function () {
    this.Init();
    return this.sImageSourcePathMap;
},
GetImagePathList : function () {
    this.Init();
    if (this.sPath.length > 1) {
        this.SortListByTime();
    }
    return this.sPath;
},
GetItemList : function (asKey) {
    this.Init();
    var ret = this.sImageSourcePathMap[asKey];
    return ret;
},
TimeComparator : function (lhs, rhs) {
    var strL = this.sCurrentPath + lhs;
    var strR = this.sCurrentPath + rhs;
    var file1 = Core_New('Elastos.IO.CFile', strL);
    var file2 = Core_New('Elastos.IO.CFile', strR);
    var last1 = file1.GetLastModified();
    var last2 = file2.GetLastModified();

    var diff = last1 - last2;

    if (diff > 0) return 1;
    if (diff < 0) return 0;

    var result = 0;
    if (this.sCurrentPath.length > 0) {
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    else {
        var lhsDir = file1.GetParent();
        var rhsDir = file2.GetParent();
        var lhsSub = lhsDir.split(this.PATH_SPLITE).pop();
        var rhsSub = rhsDir.split(this.PATH_SPLITE).pop();
        //result = lhs.Compare(rhs);
        result = (lhs > rhs);
    }
    var ret = result > 0 ? 1 : 0;

    return ret;
},
IsImageFile : function (asFileName) {
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
},

//private:
Init : function () {
    if (this.sInited) return;

    var dir = Core_New('Elastos.IO.CFile', this.sRootPath);

    var ret = dir.IsDirectory();
    if (!ret) {
        elog("sRootPath:" + this.sRootPath + " is not a valid directory!");
    }

    this._GetImageFileMap(this.sRootPath);

    this.sInited = true;
},
_GetImageFileMap : function (rootPath) {
    var dir = Core_New('Elastos.IO.CFile', rootPath);
    var files = dir.ListFiles();
    if (!files || !files.length) return;

    var itemList = [];
    for (var i=0, im=files.length; i<im; i++) {
        var file = files[i];
        if (!file.CanRead()) continue;

        if (file.IsDirectory()) {
            var absPath = file.GetAbsolutePath();
            this._GetImageFileMap(absPath);
        }
        else {
            var name = file.GetName();
            if (name) itemList.push(name);
        }
    }

    if (itemList.length > 0) {
        this.sCurrentPath = rootPath + this.PATH_SPLITE;
        itemList.sort(this.TimeComparator.bind(this));

        this.sImageSourcePathMap[rootPath] = itemList;
        this.sPath.push(rootPath);
    }
},
ClearAll : function () {  //~DataSourceHelper()
    this.sPath = [];
    this.sImageSourcePathMap = {};
    this.sInited = false;
},
SortListByTime : function () {
    var pathMap = this.sImageSourcePathMap;
    var pathList = this.sPath;

    var albumList = [];
    for (var i=0, im=pathList.length; i<im; i++) {
        var path = pathList[i];
        var fileList = pathMap[path];
        path += this.PATH_SPLITE;
        path += fileList[0];
        albumList.push(path);
    }

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
},

//public:
ROOT_PATH : "/data/temp/node/disk/testdisk",
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

};  //DataSourceHelper

DataSourceHelper.SetRootPath(DataSourceHelper.ROOT_PATH);

return DataSourceHelper;

};  //module.exports
