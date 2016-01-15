
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CMovieHelper.h"
#include "elastos/droid/graphics/Movie.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CMovieHelper);
CAR_INTERFACE_IMPL(CMovieHelper, Singleton, IMovieHelper);
ECode CMovieHelper::DecodeFile(
    /* [in] */ const String& pathName,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    return Movie::DecodeFile(pathName, movie);
}

ECode CMovieHelper::DecodeStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    return Movie::DecodeStream(is, movie);
}

ECode CMovieHelper::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    return Movie::DecodeByteArray(data, offset, length, movie);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
