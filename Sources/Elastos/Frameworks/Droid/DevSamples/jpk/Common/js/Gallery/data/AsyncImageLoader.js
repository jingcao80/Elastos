module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

function extend (a,b) {
    for (var p in b) a[p] = b[p];
}

var sFileName = '/data/temp/node/JSGallery/data/ImageCacheService.js';
var ImageCacheService = require(sFileName)(aoElastos);

//main class
function AsyncImageLoader () {};

//private static property:
var TAG = "AsyncImageLoader";
var sImgWidth = 320;
var sImgHeight = 320;

var sCacheService=null;

var sImageCache = {};
var sImageSampleStatus = {};
var sHighImageCache = {};
var sHighImageWH = {};
var sImageLoaderParamCache = {};

var sLock = null;
var sStatusLock = null;
var sImageWHLock = null;

//class
function ILoadImageCallback () {
    this.ImageLoaded = function (aoImageDrawable, aoImageView) {
        ///* [in] */ IDrawable* imageDrawable,
        ///* [in] */ IImageView* imageView) = 0;
    };
}

//class
function ImageLoaderParam (aoImageView, aoCallback) {
    this.mImageView = aoImageView;  //IImageView
    this.mCallback = aoCallback;    //ILoadImageCallback
}

//private internal class
var ImageLoaderThread = (function(){
    function _ImageLoaderThread (abIsHigh, asImageUrl) {
        this.mIsHigh = abIsHigh;
        this.mImageUrl = asImageUrl;
    }

    //private method
    function LoadImageFromUrl (path) {
        elog("====LoadImageFromUrl====begin====path:"+path);

        var cacheService = GetCacheService();
        elog("====LoadImageFromUrl====begin====1====");
        var last = 0;
        var file = Core_New("Elastos.IO.CFile", path);
        elog("====LoadImageFromUrl====begin====1.1====");
        //var last = file.LastModified();
        var last = file.GetLastModified();
        elog("====LoadImageFromUrl====begin====1.2====");

//var aa=[];
//for (var p in file)aa.push(p);
//elog("====CFile methods:["+aa.join("][")+"]");

        //var hashkey = path.GetHashCode();
        var hashkey = file.GetHashCode();
        elog("====LoadImageFromUrl====begin====1.3====");
        var len = file.GetLength();
        elog("====LoadImageFromUrl====begin====2====");
        var bitmap = cacheService.GetImageData(hashkey, this.mIsHigh, len, last);
        elog("====LoadImageFromUrl====begin====3===="+bitmap);
        if (bitmap) {
            //Mutex::Autolock lock(sStatusLock);
            sImageSampleStatus[path] = true;
            var bitmapDrawable = Droid_New(bitmap);
            return bitmapDrawable;
        }
        elog("====LoadImageFromUrl====begin====4====");

        var opt;
        var inSampleSize = 0;
        var opt = Droid_New("Elastos.Droid.Graphics.CBitmapFactoryOptions");

        opt.SetInJustDecodeBounds(true);
        var factory = Droid_New("Elastos.Droid.Graphics.CBitmapFactory");

        var bitmap = factory.DecodeFile(path, opt);
        elog("====LoadImageFromUrl====begin====5====");

        if (this.mIsHigh) {
            elog("====LoadImageFromUrl====begin====5.1====");
            inSampleSize = ComputeSampleSize(opt, -1, 1280 * 672 * 4);
        }
        else {
            elog("====LoadImageFromUrl====begin====5.2====");
            inSampleSize = ComputeSampleSize(opt, -1, sImgWidth * sImgHeight);
        }
        elog("====LoadImageFromUrl====begin====6====");

        opt.SetInSampleSize(inSampleSize);
        opt.SetInJustDecodeBounds(false);

        //Mutex::Autolock lock(sStatusLock);
        sImageSampleStatus[path] = (inSampleSize > 1 || this.mIsHigh);
        elog("====LoadImageFromUrl====begin====7====");

        //bitmap = null;
        bitmap = factory.DecodeFile(path, opt);
        elog("====LoadImageFromUrl====begin====8===="+bitmap);

//var aa=[];
//for(var pp in bitmap)aa.push(pp);
//elog("====bitmap methods:["+aa.join("][")+"]");


        var bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable",bitmap);
        elog("====LoadImageFromUrl====begin====9====");
        if (inSampleSize > 1 && bitmap != null) {
            elog("====LoadImageFromUrl====begin====10====");
            cacheService.PutImageData(hashkey, this.mIsHigh, len, last, bitmap);
        }

        elog("====LoadImageFromUrl====end====");

        // return bitmapDrawable;
    }

    function ComputeSampleSize (options, minSideLength, maxNumOfPixels) {
        elog("====ComputeSampleSize====begin====");

        var initialSize = ComputeInitialSampleSize(options, minSideLength, maxNumOfPixels);
        var roundedSize = 0;

        if (initialSize <= 8 ) {
            roundedSize = 1;
            while (roundedSize < initialSize) {
                roundedSize <<= 1;
            }
        }
        else {
            roundedSize = (initialSize + 7) / 8 * 8;
        }

        elog("====ComputeSampleSize====end====");

        return roundedSize;
    }

    function ComputeInitialSampleSize (options, minSideLength, maxNumOfPixels) {
        elog("====ComputeInitialSampleSize====begin====");

        var w = options.GetOutWidth();
        var h = options.GetOutHeight();

        if (this.mIsHigh) {
            //Mutex::Autolock lock(sImageWHLock);
            sHighImageWH[mImageUrl] = [w, h];
        }

        var lowerBound = (maxNumOfPixels == -1/*UNCONSTRAINED*/) ? 1 :
            Math.ceil(Math.sqrt(w * h / maxNumOfPixels));
        var upperBound = (minSideLength == -1/*UNCONSTRAINED*/) ? 128 :
            Math.min(Math.Floor(w / minSideLength),
                Math.floor(h / minSideLength));

        elog("====ComputeInitialSampleSize====end====");

        if (upperBound < lowerBound) {
            // return the larger one when there is no overlapping zone.
            return lowerBound;
        }

        if ((maxNumOfPixels == -1/*UNCONSTRAINED*/) && (minSideLength == -1/*UNCONSTRAINED*/)) {
            return 1;
        }
        else if (minSideLength == -1/*UNCONSTRAINED*/) {
            return lowerBound;
        }
        else {
            return upperBound;
        }

    }

    //public method:
    extend(_ImageLoaderThread.prototype, {
        _LoadImageFromUrl : LoadImageFromUrl,

        start : function () {
            this.Run();
        },

        Run : function () {
            var drawable = this._LoadImageFromUrl(this.mImageUrl);
            AsyncImageLoader.DrawableLoaded(this.mImageUrl, this.mIsHigh, drawable);
        },
    }); //extend

    return _ImageLoaderThread;
})();   //ImageLoaderThread

//public static
function LoadDrawable (imageUrl, isHigh, imageView, imageCallback) {

    elog('====AsyncImageLoader.LoadDrawable.begin======1==='+imageUrl);

    var drawble;

    //Mutex::Autolock lock(sLock);

    if (isHigh) {
        drawable = sHighImageCache[imageUrl];
    }
    else {
        drawable = sImageCache[imageUrl];
    }
    if (drawable) return drawable;

    elog('====AsyncImageLoader.LoadDrawable.begin========2========');

    var key = this.MakeImageParamKey(imageUrl, isHigh);
    elog('====AsyncImageLoader.LoadDrawable.begin========3========'+key);
    var list = sImageLoaderParamCache[key];
    elog('====AsyncImageLoader.LoadDrawable.begin========4========');
    if (list) {
        elog('====AsyncImageLoader.LoadDrawable.begin========5========');
        var needAdd = true;
        for (var i=0, im=list.length; i<im; i++) {
            if (list[i] == imageView) {
                needAdd = false;
                break;
            }
        }
        elog('====AsyncImageLoader.LoadDrawable.begin========5.1========');
        if (needAdd) {
            elog('====AsyncImageLoader.LoadDrawable.begin========5.2========');
            list.push({
                mImageView : imageView,
                mCallback : imageCallback,
            });
        }
    }
    else {
        elog('====AsyncImageLoader.LoadDrawable.begin========6========');
        sImageLoaderParamCache[key] = [
            {
                mImageView : imageView,
                mCallback : imageCallback,
            }
        ];
    }

    elog('====AsyncImageLoader.LoadDrawable.begin=========7====url:'+imageUrl);

    var loadRunnable = new ImageLoaderThread(isHigh, imageUrl);

    elog('====AsyncImageLoader.LoadDrawable.begin=========8====');
    loadRunnable.start();
    elog('====AsyncImageLoader.LoadDrawable.begin=========9====');

    return drawble;
}

function DrawableLoaded (imageUrl, isHigh, drawable){
    elog('====AsyncImageLoader.DrawableLoaded.begin=========');

    //Mutex::Autolock lock(sLock);

    if (isHigh) {
        sHighImageCache[imageUrl] = drawable;
    }
    else {
        sImageCache[imageUrl] = drawable;
    }

    var key = this.MakeImageParamKey(imageUrl, isHigh);
    elog('====AsyncImageLoader.DrawableLoaded.begin====1=====key:' + key + ' Cache:' + JSON.stringify(sImageLoaderParamCache));
    var list = sImageLoaderParamCache[key];
    elog('====AsyncImageLoader.DrawableLoaded.begin====2=====');
    if (list) {
        for (var i=0, im=list.length; i<im; i++) {
            var param = list[i];    //ImageLoaderParam
            elog('====AsyncImageLoader.DrawableLoaded.begin====3=====');
            param.mCallback.ImageLoaded(drawable, param.mImageView);
            elog('====AsyncImageLoader.DrawableLoaded.begin====4=====');
        }
    }

    elog('====AsyncImageLoader.DrawableLoaded.end=========');
}

function NeedLoadHighDrawable (imageUrl) {
    //Mutex::Autolock lock(sStatusLock);
    return sImageSampleStatus[imageUrl];
    //return true;
}

function GetOrigionWidthAndHeight (imageUrl, w, h) {
// Boolean AsyncImageLoader::GetOrigionWidthAndHeight(
//     /* [in] */ const String& imageUrl,
//     /* [out] */ Int32* w,
//     /* [out] */ Int32* h)

//     // VALIDATE_NOT_NULL(w);
//     *w = 0;
//     // VALIDATE_NOT_NULL(h);
//     *h = 0;

//     Mutex::Autolock lock(sImageWHLock);
//     HashMap<String, AutoPtr< ArrayOf<Int32> > >::Iterator it = sHighImageWH.Find(imageUrl);
//     if (it != sHighImageWH.End()) {
//         AutoPtr< ArrayOf<Int32> > tmpArray = it->mSecond;
//         *w = (*tmpArray)[0];
//         *h = (*tmpArray)[1];
//         return TRUE;
//     }
//     AutoPtr<IBitmapFactoryOptions> opt;
//     CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opt);
//     opt->SetInJustDecodeBounds(TRUE);
//     AutoPtr<IBitmapFactory> factory;
//     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
//     AutoPtr<IBitmap> bitmap;
//     if (FAILED(factory->DecodeFile(imageUrl, opt, (IBitmap**)&bitmap)))
//         return FALSE;
//     opt->GetOutWidth(w);
//     opt->GetOutHeight(h);
//     return TRUE;
}

function ClearAll () {
    sImageCache = {};
    sImageSampleStatus = {};
    sHighImageCache = {};
    sHighImageWH = {};
}

function MakeImageParamKey (imageUrl, isHigh) {
    return imageUrl + isHigh;
}

function UpdateImageCache (albumArray) {
    var cacheService = GetCacheService();
    var album = albumArray;
    cacheService.UpdateCache();
    var len = album.length;
    var isMatch = false;
    for (var i=0,im=album.length;i<im;i++) {
        var key = album[i];
        if(sImageCache[key]){
            sImageCache[key]=null;
            delete sImageCache[key];
        }
    }
}

function GetCacheService () {
    var a = {
        "typeof" : typeof sCacheService,
        "string" : ''+sCacheService,
        "isNull2" : sCacheService===null,
        "isNull3" : sCacheService===null,
        "isUndefined2" : sCacheService==undefined,
        "isUndefined3" : sCacheService===undefined,
        "JSON" : JSON.stringify(sCacheService),
        "Object" : sCacheService instanceof Object,
        "ImageCacheService" : sCacheService instanceof ImageCacheService,

    };
    elog("========GetCacheService========begin========"+JSON.stringify(a));
    if (sCacheService===null) {
    elog("========GetCacheService========1========");
        sCacheService = new ImageCacheService(sImgWidth, sImgHeight);
    }
    elog("========GetCacheService========end========"+typeof sCacheService.GetImageData);
    return sCacheService;
}

extend(AsyncImageLoader, {
    //public static:
    LoadDrawable : LoadDrawable,
    DrawableLoaded : DrawableLoaded,
    NeedLoadHighDrawable : NeedLoadHighDrawable,
    GetOrigionWidthAndHeight : GetOrigionWidthAndHeight,
    ClearAll : ClearAll,
    MakeImageParamKey : MakeImageParamKey,
    UpdateImageCache : UpdateImageCache,
    GetCacheService : GetCacheService,
});

return AsyncImageLoader;

};   //module.exports
