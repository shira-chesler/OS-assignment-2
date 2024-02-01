#include <stdio.h>
#include <stdlib.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

void base64_encode_file(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(output_file, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);

    int length;
    unsigned char buffer[1024];
    while ((length = fread(buffer, 1, 1024, input_file)) > 0) {
        BIO_write(bio, buffer, length);
    }

    BIO_flush(bio);
    BIO_free_all(bio);

    fclose(input_file);
    fclose(output_file);
}



void base64_decode_file(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    BIO *bio, *b64;
    int length;
    unsigned char buffer[1024];

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(input_file, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines - they're not part of the base64 encoding

    while ((length = BIO_read(bio, buffer, 1024)) > 0) {
        fwrite(buffer, 1, length, output_file);
    }

    BIO_free_all(bio);
    fclose(input_file);
    fclose(output_file);
}