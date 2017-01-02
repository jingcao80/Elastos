//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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