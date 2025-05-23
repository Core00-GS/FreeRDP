
#include <winpr/crt.h>
#include <winpr/print.h>
#include <winpr/crypto.h>
#include <winpr/ssl.h>

static BOOL test_crypto_cipher_aes_128_cbc(BOOL ex)
{
	BOOL result = FALSE;
	BYTE key[16] = "0123456789abcdeF";
	BYTE iv[16] = "1234567887654321";
	BYTE ibuf[1024] = { 0 };
	BYTE obuf[1024] = { 0 };
	size_t ilen = 0;
	size_t olen = 0;
	size_t xlen = 0;
	const char plaintext[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
	                         "eiusmod tempor incididunt ut labore et dolore magna aliqua.";

	/* encrypt */

	WINPR_CIPHER_CTX* ctx = NULL;
	if (ex)
		ctx = winpr_Cipher_NewEx(WINPR_CIPHER_AES_128_CBC, WINPR_ENCRYPT, key, sizeof(key), iv,
		                         sizeof(iv));
#if !defined(WITHOUT_FREERDP_3x_DEPRECATED)
	else
		ctx = winpr_Cipher_New(WINPR_CIPHER_AES_128_CBC, WINPR_ENCRYPT, key, iv);
#endif
	if (!ctx)
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_New (encrypt) failed\n", __func__);
		return FALSE;
	}

	ilen = strnlen(plaintext, sizeof(plaintext)) + 1;
	memcpy(ibuf, plaintext, ilen);

	ilen = ((ilen + 15) / 16) * 16;
	olen = 0;
	xlen = 0;

	if (!winpr_Cipher_Update(ctx, ibuf, ilen, obuf, &olen))
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_New (encrypt) failed\n", __func__);
		goto out;
	}
	xlen += olen;

	if (!winpr_Cipher_Final(ctx, obuf + xlen, &olen))
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_Final (encrypt) failed\n", __func__);
		goto out;
	}
	xlen += olen;

	if (xlen != ilen)
	{
		(void)fprintf(stderr, "%s: error, xlen (%" PRIuz ") != ilen (%" PRIuz ") (encrypt)\n",
		              __func__, xlen, ilen);
		goto out;
	}

	winpr_Cipher_Free(ctx);
	ctx = NULL;

	/* decrypt */

	if (ex)
		ctx = winpr_Cipher_NewEx(WINPR_CIPHER_AES_128_CBC, WINPR_DECRYPT, key, sizeof(key), iv,
		                         sizeof(iv));
#if !defined(WITHOUT_FREERDP_3x_DEPRECATED)
	else
		ctx = winpr_Cipher_New(WINPR_CIPHER_AES_128_CBC, WINPR_DECRYPT, key, iv);

#endif
	if (!ctx)
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_New (decrypt) failed\n", __func__);
		return FALSE;
	}

	memset(ibuf, 0, sizeof(ibuf));
	memcpy(ibuf, obuf, xlen);
	memset(obuf, 0, sizeof(obuf));

	ilen = xlen;
	olen = 0;
	xlen = 0;

	if (!winpr_Cipher_Update(ctx, ibuf, ilen, obuf, &olen))
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_New (decrypt) failed\n", __func__);
		goto out;
	}
	xlen += olen;

	if (!winpr_Cipher_Final(ctx, obuf + xlen, &olen))
	{
		(void)fprintf(stderr, "%s: winpr_Cipher_Final (decrypt) failed\n", __func__);
		goto out;
	}
	xlen += olen;

	if (xlen != ilen)
	{
		(void)fprintf(stderr, "%s: error, xlen (%" PRIuz ") != ilen (%" PRIuz ") (decrypt)\n",
		              __func__, xlen, ilen);
		goto out;
	}

	if (strcmp((const char*)obuf, plaintext) != 0)
	{
		(void)fprintf(stderr, "%s: error, decrypted data does not match plaintext\n", __func__);
		goto out;
	}

	result = TRUE;

out:
	winpr_Cipher_Free(ctx);
	return result;
}

static const char TEST_RC4_KEY[] = "Key";
static const char TEST_RC4_PLAINTEXT[] = "Plaintext";
static const char TEST_RC4_CIPHERTEXT[] = "\xBB\xF3\x16\xE8\xD9\x40\xAF\x0A\xD3";

static BOOL test_crypto_cipher_rc4(void)
{
	BOOL rc = FALSE;
	WINPR_RC4_CTX* ctx = NULL;

	const size_t len = strnlen(TEST_RC4_PLAINTEXT, sizeof(TEST_RC4_PLAINTEXT));
	BYTE* text = (BYTE*)calloc(1, len);
	if (!text)
	{
		(void)fprintf(stderr, "%s: failed to allocate text buffer (len=%" PRIuz ")\n", __func__,
		              len);
		goto out;
	}

	if ((ctx = winpr_RC4_New(TEST_RC4_KEY, strnlen(TEST_RC4_KEY, sizeof(TEST_RC4_KEY)))) == NULL)
	{
		(void)fprintf(stderr, "%s: winpr_RC4_New failed\n", __func__);
		goto out;
	}
	rc = winpr_RC4_Update(ctx, len, (const BYTE*)TEST_RC4_PLAINTEXT, text);
	winpr_RC4_Free(ctx);
	if (!rc)
	{
		(void)fprintf(stderr, "%s: winpr_RC4_Update failed\n", __func__);
		goto out;
	}

	if (memcmp(text, TEST_RC4_CIPHERTEXT, len) != 0)
	{
		char* actual = NULL;
		char* expected = NULL;

		actual = winpr_BinToHexString(text, len, FALSE);
		expected = winpr_BinToHexString(TEST_RC4_CIPHERTEXT, len, FALSE);

		(void)fprintf(stderr, "%s: unexpected RC4 ciphertext: Actual: %s Expected: %s\n", __func__,
		              actual, expected);

		free(actual);
		free(expected);
		goto out;
	}

	rc = TRUE;

out:
	free(text);
	return rc;
}

static const BYTE* TEST_RAND_DATA =
    (BYTE*)"\x1F\xC2\xEE\x4C\xA3\x66\x80\xA2\xCE\xFE\x56\xB4\x9E\x08\x30\x96"
           "\x33\x6A\xA9\x6D\x36\xFD\x3C\xB7\x83\x04\x4E\x5E\xDC\x22\xCD\xF3"
           "\x48\xDF\x3A\x2A\x61\xF1\xA8\xFA\x1F\xC6\xC7\x1B\x81\xB4\xE1\x0E"
           "\xCB\xA2\xEF\xA1\x12\x4A\x83\xE5\x1D\x72\x1D\x2D\x26\xA8\x6B\xC0";

static const BYTE* TEST_CIPHER_KEY =
    (BYTE*)"\x9D\x7C\xC0\xA1\x94\x3B\x07\x67\x2F\xD3\x83\x10\x51\x83\x38\x0E"
           "\x1C\x74\x8C\x4E\x15\x79\xD6\xFF\xE2\xF0\x37\x7F\x8C\xD7\xD2\x13";

static const BYTE* TEST_CIPHER_IV =
    (BYTE*)"\xFE\xE3\x9F\xF0\xD1\x5E\x37\x0C\xAB\xAB\x9B\x04\xF3\xDB\x99\x15";

static BOOL test_crypto_cipher_key(void)
{
	int status = 0;
	BYTE key[32] = { 0 };
	BYTE iv[16] = { 0 };
	BYTE salt[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };

	status = winpr_Cipher_BytesToKey(WINPR_CIPHER_AES_256_CBC, WINPR_MD_SHA1, salt, TEST_RAND_DATA,
	                                 64, 4, key, iv);

	if (status != 32 || memcmp(key, TEST_CIPHER_KEY, 32) != 0 ||
	    memcmp(iv, TEST_CIPHER_IV, 16) != 0)
	{
		char* akstr = NULL;
		char* ekstr = NULL;
		char* aivstr = NULL;
		char* eivstr = NULL;

		akstr = winpr_BinToHexString(key, 32, 0);
		ekstr = winpr_BinToHexString(TEST_CIPHER_KEY, 32, 0);

		aivstr = winpr_BinToHexString(iv, 16, 0);
		eivstr = winpr_BinToHexString(TEST_CIPHER_IV, 16, 0);

		(void)fprintf(stderr, "Unexpected EVP_BytesToKey Key: Actual: %s, Expected: %s\n", akstr,
		              ekstr);
		(void)fprintf(stderr, "Unexpected EVP_BytesToKey IV : Actual: %s, Expected: %s\n", aivstr,
		              eivstr);

		free(akstr);
		free(ekstr);
		free(aivstr);
		free(eivstr);
	}

	return TRUE;
}

int TestCryptoCipher(int argc, char* argv[])
{

	WINPR_UNUSED(argc);
	WINPR_UNUSED(argv);

	winpr_InitializeSSL(WINPR_SSL_INIT_DEFAULT);

	if (!test_crypto_cipher_aes_128_cbc(TRUE))
		return -1;

	if (!test_crypto_cipher_aes_128_cbc(FALSE))
		return -1;

	if (!test_crypto_cipher_rc4())
		return -1;

	if (!test_crypto_cipher_key())
		return -1;

	return 0;
}
