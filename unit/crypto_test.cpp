// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/mqtt/crypto.hpp"

using AstarteDeviceSdk::PsaKey;

TEST(AstarteTestCrypto, ParseNullPsaKey) {
  PsaKey key = PsaKey();
  EXPECT_EQ(key.get(), PSA_KEY_ID_NULL);
}

TEST(AstarteTestCrypto, ParsePsaKey) {
  PsaKey key = PsaKey();
  key.generate();
  psa_key_attributes_t attr = psa_key_attributes_init();

  auto ret = psa_get_key_attributes(key.get(), &attr);
  EXPECT_EQ(ret, 0);

  auto algorithm = psa_get_key_algorithm(&attr);
  EXPECT_EQ(algorithm, PSA_ECC_FAMILY_SECP_R1);
  auto flags = psa_get_key_usage_flags(&attr);
  EXPECT_EQ(flags, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  auto key_type = psa_get_key_type(&attr);
  EXPECT_EQ(key_type, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  auto key_bits = psa_get_key_bits(&attr);
  EXPECT_EQ(key_bits, 256);

  psa_reset_key_attributes(&attr);
}
#endif
