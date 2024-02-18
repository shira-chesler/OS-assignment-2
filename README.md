# OS-assignment-2

## Instructions for Using the Server and Client Programs

### Prerequisites

- Ensure that each file to be downloaded has a file extension (e.g., `.txt`, `.list`, etc.).

### Server Setup

- To activate the server program, provide the full path where the server will run. For example:

  `./server /path/to/server/directory`


### Client Usage

- The client program accepts the following arguments:
- `<method>`: The HTTP method to use (`POST` or `GET`).
- `<REMOTEPATH>`: The path relative to the server's directory.
- `<PATHTOCONTENTS>`: (Only required for `POST` method) The full path to the file containing the contents to be posted.

- To download a file from the server (GET request), run the client with the following command:

  `./client GET /path/relative/to/server`

  The file will be saved in the directory where the client is being run from, with the same name as it had on the server.

- To upload a file to the server (POST request), run the client with the following command:

  `./client POST /path/relative/to/server /full/path/to/local/file`

  If a file with the same name already exists on the server, the request will result in a 500 Internal Server Error.

### Additional Notes

- The line ending used in the communication between the client and server is `CRLF` (`\r\n`).



