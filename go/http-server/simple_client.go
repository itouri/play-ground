package main

import (
	"bufio"
	"fmt"
	"net"
	"net/http"
	"net/http/httputil"
)

func main() {
	conn, err := net.Dial("tcp", NetAddress)
	panicOnError(err)

	request, err := http.NewRequest("GET", "http://"+NetAddress, nil)
	panicOnError(err)

	request.Write(conn)
	// ReadResponceって何やってる?
	responce, err := http.ReadResponse(bufio.NewReader(conn), request)
	panicOnError(err)

	dump, err := httputil.DumpResponse(responce, true)
	panicOnError(err)

	fmt.Println(string(dump))
}
