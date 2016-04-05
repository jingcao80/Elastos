
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/BitmapFactory.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

////////////////////////////////////////////////////////////////////////////////
CAR_SINGLETON_IMPL(CBitmapFactory);

CAR_INTERFACE_IMPL(CBitmapFactory, Singleton, IBitmapFactory);

ECode CBitmapFactory::DecodeFile(
    /* [in] */ const String& pathName,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeFile(pathName, opts, bitmap);
}

ECode CBitmapFactory::DecodeFile(
    /* [in] */ const String& pathName,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeFile(pathName, bitmap);
}

ECode CBitmapFactory::DecodeResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ IRect* pad,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeResourceStream(res, value, is, pad, _opts, bitmap);
}

ECode CBitmapFactory::DecodeResource(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeResource(res, id, opts, bitmap);
}

ECode CBitmapFactory::DecodeResource(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeResource(res, id, bitmap);
}

ECode CBitmapFactory::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeByteArray(data, offset, length, _opts, bitmap);
}

ECode CBitmapFactory::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeByteArray(data, offset, length, bitmap);
}

ECode CBitmapFactory::DecodeStream(
    /* [in] */ IInputStream* _is,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeStream(_is, outPadding, _opts, bitmap);
}

ECode CBitmapFactory::DecodeStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeStream(is, bitmap);
}

ECode CBitmapFactory::DecodeFileDescriptor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeFileDescriptor(fd, outPadding, _opts, bitmap);
}

ECode CBitmapFactory::DecodeFileDescriptor(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return BitmapFactory::DecodeFileDescriptor(fd, bitmap);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
