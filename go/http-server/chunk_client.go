package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"net/http"
	"net/http/httputil"
	"strconv"
)

func main() {

	conn, err := net.Dial("tcp", NetAddress)
	panicOnError(err)

	// GET
	request, err := http.NewRequest("GET", "http://"+NetAddress, nil)
	panicOnError(err)

	err = request.Write(conn)
	panicOnError(err)

	reader := bufio.NewReader(conn)
	responce, err := http.ReadResponse(reader, request)
	panicOnError(err)

	dump, err := httputil.DumpResponse(responce, false) // true -> false
	panicOnError(err)
	fmt.Println(string(dump))

	if len(responce.TransferEncoding) < 1 || responce.TransferEncoding[0] != "chunked" {
		panic("wrong transfer encoding")
	}

	for {
		// サイズを取得
		sizeStr, err := reader.ReadBytes('\n') // \nまで読み込み
		if err == io.EOF {
			break
		}

		// 16進数のサイズをパース,サイズが0ならクローズ
		size, err := strconv.ParseInt(string(sizeStr[:len(sizeStr)-2]), 16, 64)
		if size == 0 {
			break
		}
		panicOnError(err)

		// サイズ数分バッファを確保して読み込み
		line := make([]byte, int(size))
		reader.Read(line)
		reader.Discard(2) // 2バイトをスキップ \r\n?
		fmt.Printf(" %d bytes: %s\n", size, string(line))
	}
}
