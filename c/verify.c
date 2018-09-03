#include <stdlib.h>
#include <secp256k1.h>

#include "sha256.h"

int char_to_int(char ch)
{
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  return -1;
}

int hex_to_bin(char* buf, size_t buf_len, const char* hex)
{
  int i = 0;

  for (; i < buf_len && hex[i * 2] != '\0' && hex[i * 2 + 1] != '\0'; i++) {
    int a = char_to_int(hex[i * 2]);
    int b = char_to_int(hex[i * 2 + 1]);

    if (a < 0 || b < 0) {
      return -1;
    }

    buf[i] = ((a & 0xF) << 4) | (b & 0xF);
  }

  if (i == buf_len && hex[i * 2] != '\0') {
    return -1;
  }
  return i;
}

void callback(const char* message, void* data) {
  exit(10);
}

#define CHECK_LEN(x) if ((x) <= 0) { return x; }

/*
 * Arguments are listed in the following order:
 * 0. Program name, ignored here, only preserved for compatibility reason
 * 1. Pubkey in hex format, a maximum of 130 bytes will be processed
 * 2. Signature in hex format, a maximum of 512 bytes will be processed
 * 3. Current script hash in hex format, which is 128 bytes. While this program
 * cannot verify the hash directly, this ensures the script is include in
 * signature calculation
 * 4. Other additional parameters that might be included. Notice only ASCII
 * characters are included, so binary should be passed as binary format.
 *
 * This program will run double sha256 on all arguments excluding pubkey and
 * signature(also for simplicity, we are running sha256 on ASCII chars directly,
 * not deserialized raw bytes), then it will use sha256 result calculated as the
 * message to verify the signature. It returns 0 if the signature works, and
 * a non-zero value otherwise.
 *
 * Note all hex values passed in as arguments must have lower case letters for
 * deterministic behavior.
 */
int main(int argc, char* argv[])
{
  char buf[256];
  int len;

  if (argc < 4) {
    return -1;
  }

  secp256k1_context *context = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
  secp256k1_context_set_illegal_callback(context, callback, NULL);
  secp256k1_context_set_error_callback(context, callback, NULL);

  len = hex_to_bin(buf, 65, argv[1]);
  CHECK_LEN(len);
  secp256k1_pubkey pubkey;

  int ret = secp256k1_ec_pubkey_parse(context, &pubkey, buf, len);
  if (ret == 0) {
    secp256k1_context_destroy(context);
    return 1;
  }

  len = hex_to_bin(buf, 256, argv[2]);
  CHECK_LEN(len);
  secp256k1_ecdsa_signature signature;
  secp256k1_ecdsa_signature_parse_der(context, &signature, buf, len);

  SHA256_CTX sha256_ctx;
  unsigned char hash[SHA256_BLOCK_SIZE];
  sha256_init(&sha256_ctx);
  for (int i = 3; i < argc; i++) {
    sha256_update(&sha256_ctx, argv[i], strlen(argv[i]));
  }
  sha256_final(&sha256_ctx, hash);

  sha256_init(&sha256_ctx);
  sha256_update(&sha256_ctx, hash, SHA256_BLOCK_SIZE);
  sha256_final(&sha256_ctx, hash);

  ret = secp256k1_ecdsa_verify(context, &signature, hash, &pubkey);
  if (ret == 1) {
    ret = 0;
  } else {
    ret = 2;
  }

  secp256k1_context_destroy(context);
  return ret;
}
