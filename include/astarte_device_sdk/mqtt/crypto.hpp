// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_CRYPTO_HPP
#define ASTARTE_CRYPTO_HPP

#include <string>
#include <string_view>

#include "mbedtls/error.h"
#include "mbedtls/pk.h"
#include "psa/crypto.h"

namespace AstarteDeviceSdk {

/**
 * @brief A C++ RAII wrapper for a PSA mbedtls_svc_key_id_t.
 */
class PsaKey {
 public:
  /**
   @brief Constructs an empty PsaKey, holding no key.
   */
  PsaKey();
  /**
   * @brief Constructs a PsaKey that takes ownership of an existing key ID.
   * @param key_id The PSA key identifier to manage.
   */
  explicit PsaKey(mbedtls_svc_key_id_t key_id);
  /**
   * @brief Destroys the PsaKey.
   */
  ~PsaKey();
  /**
   * @brief PsaKey is not copy-constructible.
   */
  PsaKey(const PsaKey&) = delete;
  /**
   * @brief PsaKey is not copy-assignable.
   */
  PsaKey& operator=(const PsaKey&) = delete;
  /**
   * @brief Move-constructs a PsaKey.
   * @param other The PsaKey to move from.
   */
  PsaKey(PsaKey&& other) noexcept;
  /**
   * @brief Move-assigns a PsaKey.
   * @param other The PsaKey to move from.
   * @return A reference to this object.
   * @throws CryptoException if destroying the *current* key fails.
   */
  auto operator=(PsaKey&& other) -> PsaKey&;
  /**
   * @brief Gets read-only access to the underlying key ID
   * @return The managed mbedtls_svc_key_id_t. Returns PSA_KEY_ID_NULL if this object is empty.
   */
  auto get() const -> mbedtls_svc_key_id_t;

 private:
  /**
   * @brief The managed PSA key identifier.
   */
  mbedtls_svc_key_id_t key_id;
};

/**
 * @brief Handle Astarte cryptographic operations.
 */
class Crypto {
 public:
  /**
   * @brief Creates a new ECDSA (secp256r1) private key.
   * @return A PsaKey object managing the newly created private key.
   * @throws CryptoException on failure.
   */
  static auto create_key() -> PsaKey;

  /**
   * @brief Creates a Certificate Signing Request (CSR) from a private key.
   *
   * @param priv_key A reference to the PsaKey holding the private key.
   * This function *uses* the key but does not take ownership.
   * @return A string containing the CSR in PEM format.
   * @throws CryptoException on failure.
   */
  static auto create_csr(const PsaKey priv_key) -> std::string;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_CRYPTO_HPP
