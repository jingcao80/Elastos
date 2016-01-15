
#ifndef __ELASTOS_DROID_UTILITY_CBASE64_H__
#define __ELASTOS_DROID_UTILITY_CBASE64_H__

#include "_Elastos_Droid_Utility_CBase64.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CBase64)
    , public Singleton
    , public IBase64
{
public:
    /* package */
    class Coder
        : public Object
    {
    public:

        AutoPtr<ArrayOf<Byte> > mOutput;
        Int32 mOp;

        Coder()
            : mOp(0)
        {
        }

        virtual ~Coder()
        {
        }

        /**
         * Encode/decode another block of input data.  this->output is
         * provided by the caller, and must be big enough to hold all
         * the coded data.  On exit, this->opwill be set to the length
         * of the coded data.
         *
         * @param finish true if this is the final call to process for
         *        this object.  Will finalize the coder state and
         *        include any final bytes in the output.
         *
         * @return true if the input so far is good; false if some
         *         error has been detected in the input stream..
         */
        virtual Boolean Process(
            ArrayOf<Byte>* input, Int32 offset, Int32 len, Boolean finish) = 0;

        /**
         * @return the maximum number of bytes a call to process()
         * could produce for the given number of input bytes.  This may
         * be an overestimate.
         */
        virtual Int32 MaxOutputSize(Int32 len) = 0;
    };

    /* package */
    class Decoder
        : public Coder
    {
    private:

        /**
         * States 0-3 are reading through the next input tuple.
         * State 4 is having read one '=' and expecting exactly
         * one more.
         * State 5 is expecting no more data or padding characters
         * in the input.
         * State 6 is the error state; an error has been detected
         * in the input and no future input can "fix" it.
         */
        Int32 state;   // state number (0 to 6)
        Int32 value;

        const Int32* alphabet;

    public:

        Decoder(Int32 flags, ArrayOf<Byte>* output);

        /**
         * @return an overestimate for the number of bytes {@code
         * len} bytes could decode to.
         */
        virtual Int32 MaxOutputSize(Int32 len);
        /**
         * Decode another block of input data.
         *
         * @return true if the state machine is still healthy.  false if
         *         bad base-64 data has been detected in the input stream.
         */
        virtual Boolean Process(
            ArrayOf<Byte>* input, Int32 offset, Int32 len, Boolean finish);
    };

    /* package */
    class Encoder
        : public Coder
    {
    private:

        Byte tail[2];
        /* package */ Int32 tailLen;
        Int32 count;
        const Byte* alphabet;

    public:

        Boolean do_padding;
        Boolean do_newline;
        Boolean do_cr;

        Encoder(Int32 flags, ArrayOf<Byte>* output);

        /**
         * @return an overestimate for the number of bytes {@code
         * len} bytes could encode to.
         */
        virtual Int32 MaxOutputSize(Int32 len);

        virtual Boolean Process(
            ArrayOf<Byte>* input, Int32 offset, Int32 len, Boolean finish);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Decode(
        /* [in] */ const String& str,
        /* [in] */ Int32 flags,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 flags,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [in] */ Int32 flags,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI EncodeToString(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI EncodeToString(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 flags,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [in] */ Int32 flags,
        /* [out, callee] */ ArrayOf<Byte>** result);

private:
};

}
}
}
#endif // __ELASTOS_DROID_UTILITY_CBASE64_H__
