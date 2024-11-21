# General

codecrafters build your own HTTP server in C


HTTP is the protocol that powers the web. In this challenge, you'll build a HTTP server that's capable of handling simple GET/POST requests, serving files and handling multiple concurrent connections.

Along the way, we'll learn about TCP connections, HTTP headers, HTTP verbs, handling multiple connections and more.


## Tasks 
### Binding to Port 
`$ ./your_program.sh`

### Respond with 200
In HTTP response is made up of three parts, each separated by a CRLF (\r\n):

    Status line.
    Zero or more headers, each ending with a CRLF.
    Optional response body.

In this stage, your server's response will only contain a status line. Here's the response your server must send:
`HTTP/1.1 200 OK\r\n\r\n`

To test simply run 
`$ ./your_program.sh`
and 
`$ curl -v http://localhost:4221`

### Extract URL path 
An HTTP request is made up of three parts, each separated by a CRLF (\r\n):

    Request line.
    Zero or more headers, each ending with a CRLF.
    Optional request body.

Here's an example of an HTTP request:
`GET /index.html HTTP/1.1\r\nHost: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n`
The "request target" specifies the URL path for this request. In this example, the URL path is `/index.html`


To test simply run 
`$ ./your_program.sh`
and 
`$ curl -v http://localhost:4221/abcd`

Your server must respond to this request with a `404` response:

### Read Header

### Concurrent Connections 

### Return a file

### Read Request body
