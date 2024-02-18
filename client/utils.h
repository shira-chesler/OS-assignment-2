void base64_encode_file(const char *input_filename, const char *output_filename);
void base64_decode_file(const char *input_filename, const char *output_filename);

void base64_encode_string(const char *input, int length, char **output);
void base64_decode_string(const char *input, int length, char **output);

int size_of_encoded_string(int length);
int size_of_decoded_string(int length);