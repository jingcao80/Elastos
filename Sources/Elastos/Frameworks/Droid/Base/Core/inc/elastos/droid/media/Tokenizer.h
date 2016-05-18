#ifndef __ELASTOS_DROID_MEDIA_TOKENIZER_H__
#define __ELASTOS_DROID_MEDIA_TOKENIZER_H__

#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

class Tokenizer
    : public Object
    , public ITokenizer
{
public:
    class DataTokenizer
        : public Object
        , public ITokenizerDataTokenizer
        , public ITokenizerTokenizerPhase
    {
    public:
        DataTokenizer();

        ~DataTokenizer();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ITokenizer* host);

        CARAPI Start(
            /* [out] */ ITokenizerTokenizerPhase** result);

        CARAPI Tokenize();

    private:
        CARAPI_(Boolean) ReplaceEscape(
            /* [in] */ const String& escape,
            /* [in] */ const String& replacement,
            /* [in] */ Int32 pos);

    private:
        StringBuilder mData;
        Tokenizer* mHost;
    };

    class TagTokenizer
        : public Object
        , public ITokenizerTagTokenizer
        , public ITokenizerTokenizerPhase
    {
    public:
        TagTokenizer();

        ~TagTokenizer();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ITokenizer* host);

        CARAPI Start(
            /* [out] */ ITokenizerTokenizerPhase** result);

        CARAPI Tokenize();

    private:
        CARAPI_(void) Yield_tag();

    private:
        Boolean mAtAnnotation;
        String mName;
        String mAnnotation;
        Tokenizer* mHost;
    };

public:
    Tokenizer();

    virtual ~Tokenizer();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ITokenizerOnTokenListener* listener);

    CARAPI Reset();

    CARAPI Tokenize(
        /* [in] */ const String& s);

private:
    static const String TAG;
    AutoPtr<ITokenizerTokenizerPhase> mPhase;
    AutoPtr<ITokenizerTokenizerPhase> mDataTokenizer;
    AutoPtr<ITokenizerTokenizerPhase> mTagTokenizer;
    AutoPtr<ITokenizerOnTokenListener> mListener;
    String mLine;
    Int32 mHandledLen;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_TOKENIZER_H__