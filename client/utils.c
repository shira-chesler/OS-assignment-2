#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

void base64_encode_file(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    //what is a BIO? Base In-Out?
    BIO *bio, *b64;
    //BUF_MEM *buffer_ptr;

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

void base64_encode_string(const char *input, int length, char **output) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    *output = (char *)malloc(bufferPtr->length);
    memcpy(*output, bufferPtr->data, bufferPtr->length - 1);
    (*output)[bufferPtr->length - 1] = 0;

    BIO_free_all(bio);
}

int size_of_encoded_string(int length) {
    return 4 * ((length + 2) / 3);
}

int size_of_decoded_string(int length) {
    return (3 * length) / 4;
}

void base64_decode_string(const char *input, int length, char **output) {
    BIO *bio, *b64;

    *output = (char *)malloc(length);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input, length);
    bio = BIO_push(b64, bio);

    BIO_read(bio, *output, length);

    BIO_free_all(bio);
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