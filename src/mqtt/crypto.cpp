// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/crypto.hpp"

#include <spdlog/spdlog.h>

#include <cstring>
#include <format>
#include <stdexcept>
#include <vector>

#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "mbedtls/error.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"

namespace AstarteDeviceSdk {

// Wrappers for Mbed TLS Resources and exploit RAII principle by ensuring that
// mbedtls_*_free is always called automatically.
class MbedPk {
 public:
  mbedtls_pk_context ctx;
  MbedPk() { mbedtls_pk_init(&ctx); }
  ~MbedPk() { mbedtls_pk_free(&ctx); }
};

class MbedX509WriteCsr {
 public:
  mbedtls_x509write_csr ctx;
  MbedX509WriteCsr() { mbedtls_x509write_csr_init(&ctx); }
  ~MbedX509WriteCsr() { mbedtls_x509write_csr_free(&ctx); }
};

class MbedX509Crt {
 public:
  mbedtls_x509_crt ctx;
  MbedX509Crt() { mbedtls_x509_crt_init(&ctx); }
  ~MbedX509Crt() { mbedtls_x509_crt_free(&ctx); }
};

// Helper to convert Mbed TLS errors into C++ exceptions
void mbedtls_check(int ret, const std::string& function_name) {
  if (ret != 0) {
    char error_buf[100];
    mbedtls_strerror(ret, error_buf, sizeof(error_buf));
    throw CryptoException(std::format("{} failed: {}", function_name, error_buf));
  }
}

auto Crypto::create_key() -> std::string {
  mbedtls_check(psa_crypto_init(), "psa_crypto_init");.

  // generate the PSA EC key
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_algorithm(&key_attributes, PSA_ECC_FAMILY_SECP_R1);
  psa_key_usage_t usage_flags = PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT;
  psa_set_key_usage_flags(&key_attributes,
                          usage_flags);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&key_attributes, 256);

  mbedtls_svc_key_id_t psa_key;
  mbedtls_check(psa_generate_key(&key_attributes, &psa_key), "psa_generate_key");

  psa_reset_key_attributes(&key_attributes);

  // exposing a PSA key via PK
  MbedPk pk_key;
  mbedtls_check(mbedtls_pk_copy_from_psa(psa_key, &pk_key.ctx), "mbedtls_pk_copy_from_psa");

  // write the key to a PEM string
  std::vector<unsigned char> buf(1024, 0);
  mbedtls_check(mbedtls_pk_write_key_pem(&pk_key.ctx, buf.data(), buf.size()),
                "mbedtls_pk_write_key_pem");

  return std::string(reinterpret_cast<const char*>(buf.data()));
}

auto Crypto::create_csr(std::string_view privkey_pem) -> std::string {
  MbedX509WriteCsr req;
  MbedPk key;

  // parse the private key (+1 for null terminator)
  mbedtls_check(
      mbedtls_pk_parse_key(&key.ctx, reinterpret_cast<const unsigned char*>(privkey_pem.data()),
                           privkey_pem.length() + 1, nullptr, 0),
      "mbedtls_pk_parse_key");

  // configure the CSR
  mbedtls_x509write_csr_set_key(&req.ctx, &key.ctx);
  mbedtls_x509write_csr_set_md_alg(&req.ctx, MBEDTLS_MD_SHA256);
  mbedtls_check(mbedtls_x509write_csr_set_subject_name(&req.ctx, "CN=temporary"),
                "mbedtls_x509write_csr_set_subject_name");

  // write the CSR to a PEM string
  std::vector<unsigned char> buf(2048, 0);
  mbedtls_check(mbedtls_x509write_csr_pem(&req.ctx, buf.data(), buf.size()),
                "mbedtls_x509write_csr_pem");

  return std::string(reinterpret_cast<const char*>(buf.data()));
}

}  // namespace AstarteDeviceSdk
