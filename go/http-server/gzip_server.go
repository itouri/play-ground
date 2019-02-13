package main

import (
	"bufio"
	"bytes"
	"compress/gzip"
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
	defer conn.Close()
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

		responce := makeResponce(request)
		responce.Write(conn)
	}
}

func makeResponce(request *http.Request) *http.Response {
	responce := &http.Response{
		StatusCode: http.StatusOK,
		ProtoMajor: 1,
		ProtoMinor: 1,                 // keep-alive使用時は 1
		Header:     make(http.Header), // gzipに登場
	}
	// gzip圧縮の処理
	if isGZipAcceptable(request) {
		content := "Hello world(gzipped)\n" // keep-alive使用時は必要

		// コンテンツをgzip化して転送
		var buf bytes.Buffer
		writer := gzip.NewWriter(&buf)
		io.WriteString(writer, content)
		writer.Close()

		responce.Body = ioutil.NopCloser(&buf)
		responce.ContentLength = int64(buf.Len())
		responce.Header.Set("Content-Encoding", "gzip")
	} else {
		content := "Hello world(gzipped)\n" // keep-alive使用時は必要
		responce.Body = ioutil.NopCloser(strings.NewReader(content))
		responce.ContentLength = int64(len(content))
	}
	return responce
}

func isGZipAcceptable(request *http.Request) bool {
	str := strings.Join(request.Header["Accept-Encoding"], ",")
	return strings.Index(str, "gzip") != -1
}
