package main

import (
	"bufio"
	"fmt"
	"net"
	"net/http"
	"net/http/httputil"
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

		dump, err := httputil.DumpResponse(responce, true)
		panicOnError(err)
		fmt.Println(string(dump))

		// 全部送信完了してれば終了
		current++
		if current == len(sendMessages) {
			break
		}
	}
}
