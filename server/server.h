void openTcpServer();

void* handleClient(void* clientFD);

char* checkPath(int argc, char *const *argv);