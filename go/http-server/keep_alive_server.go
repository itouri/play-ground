package main

import (
	"bufio"
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"net/http"
	"net/http/httputil"
	"strings"
	"time"
)

func main() {
	listener, err := net.Listen("tcp", NetAddress)
	panicOnError(err)

	fmt.Println("Server is running at localhost:8888")

	for {
		conn, err := listener.Accept()
		panicOnError(err)

		go handleClient(conn)
	}
}

func handleClient(conn net.Conn) {
	fmt.Printf("Accept %v\n", conn.RemoteAddr())
	for {
		// config the time out
		conn.SetReadDeadline(time.Now().Add(5 * time.Second)) // now + 5sec

		// read request
		request, err := http.ReadRequest(bufio.NewReader(conn))
		if err != nil {
			// タイムアウトしたら終了
			neterr, ok := err.(net.Error)
			if ok && neterr.Timeout() {
				fmt.Println("Timeout")
				break
			} else if err == io.EOF {
				// ソケットがクローズしたときも終了
				break
			}
			panic(err)
		}

		dump, err := httputil.DumpRequest(request, true)
		panicOnError(err)

		fmt.Println(string(dump))
		content := "Hello world\n" // keep-alive使用時は必要
		// write responce
		responce := http.Response{
			StatusCode:    http.StatusOK,
			ProtoMajor:    1,
			ProtoMinor:    1,                   // keep-alive使用時は 1
			ContentLength: int64(len(content)), // keep-alive使用時は必要
			Body:          ioutil.NopCloser(strings.NewReader("Hello World\n")),
		}
		responce.Write(conn)
	}
	conn.Close()
}
