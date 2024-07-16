# COBOL Web Server

This project implements a simple web server using COBOL and C. It can serve static HTML files from a specified directory.

## Prerequisites

- GnuCOBOL (>= 2.2)
- GCC
- Make (optional, but recommended)

On Arch Linux, you can install these with:

sudo pacman -S gnucobol gcc make

## Project Structure

- `init_server.c`: C code for low-level socket operations
- `server.cob`: COBOL code for the main web server logic
- `static/`: Directory to store your static HTML files

## Compilation

1. Compile the C code into a shared library:

gcc -shared -fPIC -o libserver.so init_server.c

2. Compile the COBOL program:

cobc -x -free -Wall -O -o server server.cob -ldl

## Usage

1. Create a `static` directory in the same location as your server executable.

2. Place your HTML files in the `static` directory. Make sure you have an `index.html` file.

3. Run the server:

./server

4. The server will start and listen on port 8080 by default.

5. Access your web pages by opening a web browser and navigating to:

http://localhost:8080

or

http://localhost:8080/your-file.html

## Customization

- To change the port number, modify the `WS-PORT` value in `server.cob`.
- To serve different file types, add appropriate MIME types in the `SEND-HTTP-RESPONSE` paragraph of `server.cob`.

## Troubleshooting

- If you get a "module not found" error, ensure that `libserver.so` is in the same directory as the `server` executable or in a directory specified in your `LD_LIBRARY_PATH`.
- If pages aren't loading, check the console output for file read errors and ensure your HTML files are in the `static` directory.

## Limitations

- This server is for educational purposes and is not suitable for production use.
- It doesn't handle concurrent connections or implement advanced HTTP features.
- Error handling is basic and should be improved for real-world usage.

## Contributing

Feel free to fork this project and submit pull requests for improvements or additional features.

## License

This project is open-source and available under the MIT License.