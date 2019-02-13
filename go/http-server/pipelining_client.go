package main

import (
	"bufio"
	"fmt"
	"net"
	"net/http"
	"net/http/httputil"
)

func main() {
	sendMessages := []string{
		"Apple",
		"Banana",
		"Cabocha",
	}
	current := 0
	requests := make(chan *http.Request, len(sendMessages))
	conn, err := net.Dial("tcp", NetAddress)
	panicOnError(err)
	fmt.Printf("Acecess: %d\n", current)
	defer conn.Close()

	// リトライのためにfor
	for i, message := range sendMessages {
		request, err := http.NewRequest("GET", "http://"+NetAddress, nil)
		if i == len(sendMessages)-1 {
			request.Header.Add("Connection", "close")
		} else {
			request.Header.Add("Connection", "keep-alive")
		}
		panicOnError(err)

		err = request.Write(conn)
		panicOnError(err)

		fmt.Println("send: ", message)
		requests <- request
	}
	close(requests)

	// レスポンスをまとめて受信
	for request := range requests {
		// ReadResponceって何やってる?
		responce, err := http.ReadResponse(bufio.NewReader(conn), request)
		panicOnError(err)

		dump, err := httputil.DumpResponse(responce, true)
		panicOnError(err)
		fmt.Println(string(dump))

		if current == len(sendMessages) {
			break
		}
	}
}
