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

Your server must respond to this request with a `404` response

### Respond with body
In this stage, you'll implement the `/echo/{str}` endpoint, which accepts a string and returns it in the response body.
Your `/echo/{str}` endpoint must return a `200` response, with the response body set to given string, and with a Content-Type and Content-Length header.

To test simply run 
`$ ./your_program.sh`
and 
`$ curl -v http://localhost:4221/echo/abcd`

this will return `abcd`
### Read Header
In this stage, you'll implement the `/user-agent` endpoint, which reads the User-Agent request header and returns it in the response body.
The User-Agent header describes the client's user agent.

Here is an example: 
```
// Status line
HTTP/1.1 200 OK               // Status code must be 200
\r\n

// Headers
Content-Type: text/plain\r\n
Content-Length: 12\r\n
\r\n

// Response body
foobar/1.2.3                  // The value of `User-Agent`
```

To test simply run 
`$ ./your_program.sh`
and 
`$ curl -v --header "User-Agent: foobar/1.2.3" http://localhost:4221/user-agent`


### Concurrent Connections 
In this stage, you'll add support for concurrent connections.
To test simply run `$ ./your_program.sh` and 
```
$ (sleep 3 && printf "GET / HTTP/1.1\r\n\r\n") | nc localhost 4221 &
$ (sleep 3 && printf "GET / HTTP/1.1\r\n\r\n") | nc localhost 4221 &
$ (sleep 3 && printf "GET / HTTP/1.1\r\n\r\n") | nc localhost 4221 &
```

### Return a file
In this stage, you'll implement the /files/{filename} endpoint, which returns a requested file to the client.
The tester will execute your program with a `--directory` flag. The `--directory` flag specifies the directory where the files are stored, as an absolute path.

`$ ./your_program.sh --directory /tmp/`

The tester will then send two `GET` requests to the `/files/{filename}` endpoint on your server.

### Read Request body
In this stage, you'll add support for the `POST` method of the `/files/{filename}` endpoint, which accepts text from the client and creates a new file with that text.
