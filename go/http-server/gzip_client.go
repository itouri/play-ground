package main

import (
	"bufio"
	"compress/gzip"
	"fmt"
	"io"
	"net"
	"net/http"
	"net/http/httputil"
	"os"
	"strings"
)

func main() {
	sendMessages := []string{
		"Apple",
		"Banana",
		"Cabocha",
	}
	current := 0
	var conn net.Conn

	// リトライのためにfor
	for {
		var err error
		if conn == nil {
			conn, err = net.Dial("tcp", NetAddress)
			panicOnError(err)
			fmt.Printf("Acecess: %d\n", current)
		}

		// POST
		request, err := http.NewRequest(
			"POST",
			"http://"+NetAddress,
			strings.NewReader(sendMessages[current]))
		// クライアントがgzipに対応していることを明記
		request.Header.Set("Accept-Encoding", "gzip")
		panicOnError(err)

		request.Write(conn)
		// ReadResponceって何やってる?
		responce, err := http.ReadResponse(bufio.NewReader(conn), request)
		// タイムアウトはここでエラーになるのでリトライ
		if err != nil {
			fmt.Println("Retry")
			conn = nil
			continue
		}

		dump, err := httputil.DumpResponse(responce, false) // true -> false
		panicOnError(err)
		fmt.Println(string(dump))

		defer responce.Body.Close()
		if responce.Header.Get("Content-Encoding") == "gzip" {
			reader, err := gzip.NewReader(responce.Body)
			panicOnError(err)

			io.Copy(os.Stdout, reader)
			reader.Close()
		} else {
			io.Copy(os.Stdout, responce.Body)
		}

		// 全部送信完了してれば終了
		current++
		if current == len(sendMessages) {
			break
		}
	}
}
