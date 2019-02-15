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

		go processSession(conn)
	}
}

// セッション一つを処理
func processSession(conn net.Conn) {
	fmt.Printf("Accept %v\n", conn.RemoteAddr())

	// セッション内のリクエストを順に処理するためのチャンネル
	sessionResponses := make(chan chan *http.Response, 50)
	defer close(sessionResponses)

	// レスポンスを直列化...?
	go writeToConn(sessionResponses, conn)
	reader := bufio.NewReader(conn)

	for {
		// config the time out
		conn.SetReadDeadline(time.Now().Add(5 * time.Second)) // now + 5sec

		// read request
		request, err := http.ReadRequest(reader)
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

		sessionResponse := make(chan *http.Response)
		sessionResponses <- sessionResponse

		// 非同期でレスポンスを実行
		go handleRequest(request, sessionResponse)
	}
}

// セッション内のリクエストを処理
func handleRequest(request *http.Request, resultReceiver chan *http.Response) {
	dump, err := httputil.DumpRequest(request, true)
	panicOnError(err)
	fmt.Println(string(dump))

	content := "Hello world\n" // keep-alive使用時は必要
	// write responce
	// セッション維持のためにkeep-alive必須
	responce := &http.Response{
		StatusCode:    http.StatusOK,
		ProtoMajor:    1,
		ProtoMinor:    1,                   // keep-alive使用時は 1
		ContentLength: int64(len(content)), // keep-alive使用時は必要
		Body:          ioutil.NopCloser(strings.NewReader("Hello World\n")),
	}
	// 処理が終わったらチャネルに書く
	// ブロックされていたwriteToConnの処理を再起動
	resultReceiver <- responce
}

// 順番に従ってconnに書き出しをする(goroutineで実行される)
func writeToConn(sessionResponses chan chan *http.Response, conn net.Conn) {
	defer conn.Close()
	// 順番に取り出す
	for sessionResponse := range sessionResponses {
		// 選択された仕事が終わるまで待つ
		response := <-sessionResponse
		response.Write(conn)
		close(sessionResponse)
	}
}
