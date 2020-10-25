# web-server

#### To run:
- gcc echo_server.c -o echo_server -lpthread
- ./echo_server ip-address port number
- Our program works on Firefox, and the 404 error works on Chrome
  

#### Known Errors
When we did error checking, we found that messages that came through telnet had different lengths than messages that came through a browser. <br />
Our msg array from the browser includes more information than "GET /index.html HTTP/1.1" <br />
We could not change how the browser sent the message, so we elected to continue the program as we did <br />
We wanted to error check GET commands that were typed into telnet to see if they were formatted correctly. <br />
We did this format checking successfully, but a warning about the incorrect formatting for telnet will always print when using the browser. <br />
If you want to run on a browser, comment out lines 90-94 <br />
