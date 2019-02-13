package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"net"
	"net/http"
	"net/http/httputil"
	"strings"
)

func main() {
	listener, err := net.Listen("tcp", NetAddress)
	panicOnError(err)

	fmt.Println("Server is running at localhost:8888")

	for {
		conn, err := listener.Accept()
		panicOnError(err)

		go func() {
			fmt.Printf("Accept %v\n", conn.RemoteAddr())
			// read request
			request, err := http.ReadRequest(bufio.NewReader(conn))
			panicOnError(err)

			dump, err := httputil.DumpRequest(request, true)
			panicOnError(err)

			fmt.Println(string(dump))
			// write responce
			responce := http.Response{
				StatusCode: http.StatusOK,
				ProtoMajor: 1,
				ProtoMinor: 0,
				Body:       ioutil.NopCloser(strings.NewReader("Hello World\n")),
			}
			responce.Write(conn)
			conn.Close()
		}()
	}
}
