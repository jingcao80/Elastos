#include "CCipherHelper.h"
#include "CCipher.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CCipherHelper, Singleton, ICipherHelper)

CAR_SINGLETON_IMPL(CCipherHelper)

/**
 * Creates a new Cipher for the specified transformation. The installed
 * providers are searched in order for an implementation of the specified
 * transformation. The first found provider providing the transformation is
 * used to create the cipher. If no provider is found an exception is
 * thrown.
 *
 * @param transformation
 *            the name of the transformation to create a cipher for.
 * @return a cipher for the requested transformation.
 * @throws NoSuchAlgorithmException
 *             if no installed provider can provide the
 *             <i>transformation</i>, or it is {@code null}, empty or in an
 *             invalid format.
 * @throws NoSuchPaddingException
 *             if no installed provider can provide the padding scheme in
 *             the <i>transformation</i>.
 */
ECode CCipherHelper::GetInstance(
    /* [in] */ const String& transformation,
    /* [out] */ ICipher ** cipher)
{
	VALIDATE_NOT_NULL(cipher);
	return CCipher::GetInstance(transformation, cipher);
}

/**
 * Creates a new cipher for the specified transformation provided by the
 * specified provider.
 *
 * @param transformation
 *            the name of the transformation to create a cipher for.
 * @param provider
 *            the name of the provider to ask for the transformation.
 * @return a cipher for the requested transformation.
 * @throws NoSuchAlgorithmException
 *             if the specified provider can not provide the
 *             <i>transformation</i>, or it is {@code null}, empty or in an
 *             invalid format.
 * @throws NoSuchProviderException
 *             if no provider with the specified name can be found.
 * @throws NoSuchPaddingException
 *             if the requested padding scheme in the <i>transformation</i>
 *             is not available.
 * @throws IllegalArgumentException
 *             if the specified provider is {@code null}.
 */
ECode CCipherHelper::GetInstance(
    /* [in] */ const String& transformation,
    /* [in] */ const String& provider,
    /* [out] */ ICipher ** cipher)
{
	VALIDATE_NOT_NULL(cipher);
	return CCipher::GetInstance(transformation, provider, cipher);
}

/**
 * Creates a new cipher for the specified transformation. The
 * {@code provider} supplied does not have to be registered.
 *
 * @param transformation
 *            the name of the transformation to create a cipher for.
 * @param provider
 *            the provider to ask for the transformation.
 * @return a cipher for the requested transformation.
 * @throws NoSuchAlgorithmException
 *             if the specified provider can not provide the
 *             <i>transformation</i>, or it is {@code null}, empty or in an
 *             invalid format.
 * @throws NoSuchPaddingException
 *             if the requested padding scheme in the <i>transformation</i>
 *             is not available.
 * @throws IllegalArgumentException
 *             if the provider is {@code null}.
 */
ECode CCipherHelper::GetInstance(
    /* [in] */ const String& transformation,
    /* [in] */ IProvider * provider,
    /* [out] */ ICipher ** cipher)
{
	VALIDATE_NOT_NULL(cipher);
	return CCipher::GetInstance(transformation, provider, cipher);
}

/**
 * Returns the maximum key length for the specified transformation.
 *
 * @param transformation
 *            the transformation name.
 * @return the maximum key length, currently {@code Integer.MAX_VALUE}.
 * @throws NoSuchAlgorithmException
 *             if no provider for the specified {@code transformation} can
 *             be found.
 * @throws NullPointerException
 *             if {@code transformation} is {@code null}.
 */
ECode CCipherHelper::GetMaxAllowedKeyLength(
    /* [in] */ const String& transformation,
    /* [out] */ Int32 * value)
{
	VALIDATE_NOT_NULL(value);
	return CCipher::GetMaxAllowedKeyLength(transformation, value);
}

/**
 * Returns the maximum cipher parameter value for the specified
 * transformation. If there is no maximum limit, {@code null} is returned.
 *
 * @param transformation
 *            the transformation name.
 * @return a parameter spec holding the maximum value or {@code null}.
 *         Currently {@code null}.
 * @throws NoSuchAlgorithmException
 *             if no provider for the specified {@code transformation} can
 *             be found.
 * @throws NullPointerException
 *             if {@code transformation} is {@code null}.
*/
ECode CCipherHelper::GetMaxAllowedParameterSpec(
    /* [in] */ const String& transformation,
    /* [out] */ IAlgorithmParameterSpec ** value)
{
	VALIDATE_NOT_NULL(value);
	return CCipher::GetMaxAllowedParameterSpec(transformation, value);
}

} // Crypto
} // Elastosx