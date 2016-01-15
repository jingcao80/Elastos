#ifndef __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__
#define __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

class UnstyledTextExtractor
    : public Object
    , public IUnstyledTextExtractor
    , public ITokenizerOnTokenListener
{
public:
    UnstyledTextExtractor();

    virtual ~UnstyledTextExtractor();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI OnData(
        /* [in] */ const String& s);

    CARAPI OnStart(
        /* [in] */ const String& tag,
        /* [in] */ ArrayOf<String>* classes,
        /* [in] */ const String& annotation);

    CARAPI OnEnd(
        /* [in] */ const String& tag);

    CARAPI OnTimeStamp(
        /* [in] */ Int64 timestampMs);

    CARAPI OnLineEnd();

    CARAPI GetText(
        /* [out, callee] */ ArrayOf<IArrayOf*>** result);

private:
    CARAPI_(void) Init();

public:
    AutoPtr<IStringBuilder> mLine;
    AutoPtr<IVector> mLines;
    AutoPtr<IVector> mCurrentLine;
    Int64 mLastTimestamp;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_UNSTYLEDTEXTEXTRACTOR_H__
