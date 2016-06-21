module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

function extend (a,b) {
    for (var p in b) a[p] = b[p];
}

//require  ImageCacheService

//main class
function AsyncImageLoader () {};

//private static property:
var TAG = "AsyncImageLoader";
var sImgWidth = 320;
var sImgWidth = 320;
var sImgHeight = null;

var sCacheService = null;

var sImageCache = [];
var sImageSampleStatus = [];
var sHighImageCache = [];
var sHighImageWH = [];
var sImageLoaderParamCache = [];

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
    function _ImageLoaderThread (abIsHight, asImageUrl) {
        this.mIsHight = abIsHight;
        this.mImageUrl = asImageUrl;
    }

    //private method
    function LoadImageFromUrl (path) {
        // var cacheService = GetCacheService();
        // var last = 0;
        // var file = Core_New("CFile", path);
        // var last = file.LastModified();
        // var hashkey = path.GetHashCode();
        // var len = file.GetLength();
        // var bitmap = cacheService.GetImageData(hashkey, mIsHigh, len, last);
        // if (bitmap) {
        //     //Mutex::Autolock lock(sStatusLock);
        //     sImageSampleStatus[path] = true;
        //     var bitmapDrawable = Droid_New(bitmap);
        //     return bitmapDrawable;
        // }

        // var opt;
        // var inSampleSize = 0;
        // var opt = Droid_New("CBitmapFactoryOptions");
        // opt.SetInJustDecodeBounds(true);
        // var factory = Droid_New("CBitmapFactory");
        // var bitmap = factory.DecodeFile(path, opt);

        // if (mIsHigh) {
        //     inSampleSize = ComputeSampleSize(opt, -1, 1280 * 672 * 4);
        // }
        // else {
        //     inSampleSize = ComputeSampleSize(opt, -1, sImgWidth * sImgHeight);
        // }

        // opt.SetInSampleSize(inSampleSize);
        // opt.SetInJustDecodeBounds(false);

        // //Mutex::Autolock lock(sStatusLock);
        // sImageSampleStatus[path] = (inSampleSize > 1 || mIsHigh);

        // bitmap = null;
        // bitmap = factory.DecodeFile(path, opt);
        // bitmapDrawable = Droid_New(bitmap);
        // if (inSampleSize > 1 && bitmap != null) {
        //     cacheService.PutImageData(hashkey, mIsHigh, len, last, bitmap);
        // }

        // return bitmapDrawable;
    }

    function ComputeSampleSize (options, minSideLength, maxNumOfPixels) {
    //     var initialSize = ComputeInitialSampleSize(options, minSideLength, maxNumOfPixels);
    //     var roundedSize = 0;

    //     if (initialSize <= 8 ) {
    //         roundedSize = 1;
    //         while (roundedSize < initialSize) {
    //             roundedSize <<= 1;
    //         }
    //     }
    //     else {
    //         roundedSize = (initialSize + 7) / 8 * 8;
    //     }

    //     return roundedSize;
    }

    function ComputeInitialSampleSize (options, minSideLength, maxNumOfPixels) {
        // var w = options.GetOutWidth();
        // var h = options.GetOutHeight();

        // if (mIsHigh) {
        //     //Mutex::Autolock lock(sImageWHLock);
        //     sHighImageWH[mImageUrl] = [w, h];
        // }

        // var lowerBound = (maxNumOfPixels == -1/*UNCONSTRAINED*/) ? 1 :
        //     Math.Ceil(Math.Sqrt((Double)w * h / maxNumOfPixels));
        // var upperBound = (minSideLength == -1/*UNCONSTRAINED*/) ? 128 :
        //     Math.Min(Math.Floor((Double)w / minSideLength),
        //         Math.Floor(h / minSideLength));

        // if (upperBound < lowerBound) {
        //     // return the larger one when there is no overlapping zone.
        //     return lowerBound;
        // }

        // if ((maxNumOfPixels == -1/*UNCONSTRAINED*/) && (minSideLength == -1/*UNCONSTRAINED*/)) {
        //     return 1;
        // }
        // else if (minSideLength == -1/*UNCONSTRAINED*/) {
        //     return lowerBound;
        // }
        // else {
        //     return upperBound;
        // }

    }

    //public method:
    extend(_ImageLoaderThread.prototype, {
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
    elog('====AsyncImageLoader.LoadDrawable.begin========3========');
    var list = this.sImageLoaderParamCache[key];
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
        if (needAdd) {
            list.push({
                mImageView : imageView,
                mCallback : imageCallback,
            });
        }
    }
    else {
        elog('====AsyncImageLoader.LoadDrawable.begin========6========');
        this.sImageLoaderParamCache[key] = [
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

function DrawableLoaded () {
    if (sCacheService == NULL) {
        sCacheService = new ImageCacheService(sImgWidth, sImgHeight);
    }
    return sCacheService;
}

function NeedLoadHighDrawable () {}
function GetOrigionWidthAndHeight () {}
function ClearAll () {}
function MakeImageParamKey () {}
function UpdateImageCache () {}
function GetCacheService () {}

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
