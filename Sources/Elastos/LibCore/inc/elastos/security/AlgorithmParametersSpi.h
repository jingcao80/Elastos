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

#ifndef __ELASTOS_SECURITY_ALGORITHMPARAMETERSSPI_H__
#define __ELASTOS_SECURITY_ALGORITHMPARAMETERSSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastos {
namespace Security {

/**
 * {@code AlgorithmParametersSpi} is the Service Provider Interface (SPI)
 * definition for {@code AlgorithmParameters}.
 *
 * @see AlgorithmParameters
 */
class AlgorithmParametersSpi: public Object
{
public:
    virtual ~AlgorithmParametersSpi() {}

// protected:
    /**
     * Initializes this {@code AlgorithmParametersSpi} with the specified
     * {@code AlgorithmParameterSpec}.
     *
     * @param paramSpec
     *            the parameter specification.
     * @throws InvalidParameterSpecException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized or the given {@code paramSpec} is not appropriate
     *             for initializing this {@code AlgorithmParametersSpi}.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IAlgorithmParameterSpec* paramSpec)
            /*throws InvalidParameterSpecException*/ = 0;

    /**
     * Initializes this {@code AlgorithmParametersSpi} with the specified
     * {@code byte[]} using the default decoding format for parameters. The
     * default encoding format is ASN.1.
     *
     * @param params
     *            the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized, or the parameter could not be encoded.
     */
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<Byte>* params) /*throws IOException*/ = 0;

    /**
     * Initializes this {@code AlgorithmParametersSpi} with the specified
     * {@code byte[]} using the specified decoding format.
     *
     * @param params
     *            the encoded parameters.
     * @param format
     *            the name of the decoding format.
     * @throws IOException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized, or the parameter could not be encoded.
     */
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<Byte>* params,
        /* [in] */ const String& format) /*throws IOException*/ = 0;

    /**
     * Returns the {@code AlgorithmParameterSpec} for this {@code
     * AlgorithmParametersSpi}.
     *
     * @param paramSpec
     *            the type of the parameter specification in which this
     *            parameters should be converted.
     * @return the {@code AlgorithmParameterSpec} for this {@code
     *         AlgorithmParametersSpi}.
     * @throws InvalidParameterSpecException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized, or if this parameters could not be converted to
     *             the specified class.
     */
    virtual CARAPI EngineGetParameterSpec(
        /* [in] */ ClassID paramSpec,
        /* [out] */ IInterface** result) /*throws InvalidParameterSpecException*/ = 0;

    /**
     * Returns the parameters in their default encoding format. The default
     * encoding format is ASN.1.
     *
     * @return the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized, or if this parameters could not be encoded.
     */
    virtual CARAPI EngineGetEncoded(
        /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/ = 0;

    /**
     * Returns the parameters in the specified encoding format.
     *
     * @param format
     *            the name of the encoding format.
     * @return the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParametersSpi} has already been
     *             initialized, or if this parameters could not be encoded.
     */
    virtual CARAPI EngineGetEncoded(
        /* [in] */ const String& format,
        /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/ = 0;

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code AlgorithmParametersSpi}.
     *
     * @return a printable representation for this {@code
     *         AlgorithmParametersSpi}.
     */
    virtual CARAPI EngineToString(
        /* [out] */ String* result) = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_ALGORITHMPARAMETERSSPI_H__
