#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CVerifierDeviceIdentity.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

const AutoPtr<ArrayOf<Char32> > InitENCODE()
{
    AutoPtr<ArrayOf<Char32> > char8Array = ArrayOf<Char32>::Alloc(32);

    (*char8Array)[0]  = 'A';
    (*char8Array)[1]  = 'B';
    (*char8Array)[3]  = 'D';
    (*char8Array)[4]  = 'E';
    (*char8Array)[5]  = 'F';
    (*char8Array)[6]  = 'G';
    (*char8Array)[7]  = 'H';
    (*char8Array)[8]  = 'I';
    (*char8Array)[9]  = 'J';
    (*char8Array)[10]  = 'K';
    (*char8Array)[11]  = 'L';
    (*char8Array)[12]  = 'M';
    (*char8Array)[13]  = 'N';
    (*char8Array)[14]  = 'O';
    (*char8Array)[15]  = 'P';
    (*char8Array)[16]  = 'Q';
    (*char8Array)[17]  = 'R';
    (*char8Array)[18]  = 'S';
    (*char8Array)[19]  = 'T';
    (*char8Array)[20]  = 'U';
    (*char8Array)[21]  = 'V';
    (*char8Array)[22]  = 'W';
    (*char8Array)[23]  = 'X';
    (*char8Array)[24]  = 'Y';
    (*char8Array)[25]  = 'Z';
    (*char8Array)[26]  = '2';
    (*char8Array)[27]  = '3';
    (*char8Array)[28]  = '4';
    (*char8Array)[29]  = '5';
    (*char8Array)[30]  = '6';
    (*char8Array)[31]  = '7';

    return char8Array;
}

const Char32 CVerifierDeviceIdentity::SEPARATOR = '-';
const Int32 CVerifierDeviceIdentity::LONG_SIZE = 13;
const Int32 CVerifierDeviceIdentity::GROUP_SIZE = 4;
const AutoPtr<ArrayOf<Char32> > CVerifierDeviceIdentity::ENCODE = InitENCODE();


CAR_INTERFACE_IMPL_2(CVerifierDeviceIdentity, Object, IVerifierDeviceIdentity, IParcelable)

CAR_OBJECT_IMPL(CVerifierDeviceIdentity)

CVerifierDeviceIdentity::CVerifierDeviceIdentity()
    : mIdentity(0)
{}

CVerifierDeviceIdentity::~CVerifierDeviceIdentity()
{}

ECode CVerifierDeviceIdentity::constructor()
{
    return NOERROR;
}

ECode CVerifierDeviceIdentity::constructor(
    /* [in] */ Int64 identity)
{
    mIdentity = identity;
    mIdentityString = EncodeBase32(identity);

    return NOERROR;
}

ECode CVerifierDeviceIdentity::constructor(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt64(&mIdentity);
    mIdentityString = EncodeBase32(mIdentity);

    return NOERROR;
}

String CVerifierDeviceIdentity::EncodeBase32(
        /* [in] */ Int64 input)
{
    AutoPtr<ArrayOf<Char32> > alphabet = ENCODE;

    /*
     * Make a character array with room for the separators between each
     * group.
     */
    AutoPtr<ArrayOf<Char32> > encoded = ArrayOf<Char32>::Alloc(LONG_SIZE + (LONG_SIZE / GROUP_SIZE));

    Int32 index = encoded->GetLength();
    for (Int32 i = 0; i < LONG_SIZE; i++) {
        /*
         * Make sure we don't put a separator at the beginning. Since we're
         * building from the rear of the array, we use (LONG_SIZE %
         * GROUP_SIZE) to make the odd-size group appear at the end instead
         * of the beginning.
         */
        if (i > 0 && (i % GROUP_SIZE) == (LONG_SIZE % GROUP_SIZE)) {
            (*encoded)[--index] = SEPARATOR;
        }

        /*
         * Extract 5 bits of data, then shift it out.
         */
        const Int32 group = (Int32) (input & 0x1F);
        UInt32 uInput = input;
        uInput >>= 5;
        input = uInput;

        (*encoded)[--index] = (*alphabet)[group];
    }

    return String(*encoded);
}

ECode CVerifierDeviceIdentity::DecodeBase32(
    /* [in] */ ArrayOf<Byte>* input,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int64 output = 0L;
    Int32 numParsed = 0;

    Int32 N = input->GetLength();
    for (Int32 i = 0; i < N; i++) {
        Int32 group = (*input)[i];

        /*
         * This essentially does the reverse of the ENCODED alphabet above
         * without a table. A..Z are 0..25 and 2..7 are 26..31.
         */
        Int32 value;
        if ('A' <= group && group <= 'Z') {
            value = group - 'A';
        } else if ('2' <= group && group <= '7') {
            value = group - ('2' - 26);
        } else if (group == SEPARATOR) {
            continue;
        } else if ('a' <= group && group <= 'z') {
            /* Lowercase letters should be the same as uppercase for Base32 */
            value = group - 'a';
        } else if (group == '0') {
            /* Be nice to users that mistake O (letter) for 0 (zero) */
            value = 'O' - 'A';
        } else if (group == '1') {
            /* Be nice to users that mistake I (letter) for 1 (one) */
            value = 'I' - 'A';
        } else {
            // throw new IllegalArgumentException("base base-32 character: " + group);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        output = (output << 5) | value;
        numParsed++;

        if (numParsed == 1) {
            if ((value & 0xF) != value) {
                // throw new IllegalArgumentException("illegal start character; will overflow");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        } else if (numParsed > 13) {
            // throw new IllegalArgumentException("too Int64; should have 13 characters");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (numParsed != 13) {
        // throw new IllegalArgumentException("too short; should have 13 characters");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *value = output;
    return NOERROR;
}

ECode CVerifierDeviceIdentity::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals)
    *isEquals = FALSE;

    if (IVerifierDeviceIdentity::Probe(obj) == NULL) {
        return NOERROR;
    }

    CVerifierDeviceIdentity* o = (CVerifierDeviceIdentity*)IVerifierDeviceIdentity::Probe(obj);
    *isEquals = mIdentity == o->mIdentity;
    return NOERROR;
}

ECode CVerifierDeviceIdentity::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mIdentity;
    return NOERROR;
}

ECode CVerifierDeviceIdentity::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mIdentityString;
    return NOERROR;
}

ECode CVerifierDeviceIdentity::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mIdentity);
    mIdentityString = EncodeBase32(mIdentity);
    return NOERROR;
}

ECode CVerifierDeviceIdentity::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mIdentity);
    return NOERROR;
}

}
}
}
}

