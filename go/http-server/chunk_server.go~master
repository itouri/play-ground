package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"net/http"
	"net/http/httputil"
	"strings"
	"time"
)

// 青空文庫: ごんぎつねより
// http://www.aozora.gr.jp/cards/000121/card628.html
var contents = []string{
	"これは,私わたしが小さいときに,村の茂平もへいというおじいさんからきいたお話です。",
	"むかしは、私たちの村のちかくの、中山なかやまというところに小さなお城があって、",
	"中山さまというおとのさまが、おられたそうです。",
	"その中山から、少しはなれた山の中に、「ごん狐ぎつね」という狐がいました。",
	"ごんは、一人ひとりぼっちの小狐で、しだの一ぱいしげった森の中に穴をほって住んでいました。",
	"そして、夜でも昼でも、あたりの村へ出てきて、いたずらばかりしました。",
}

// var contents = `これは,私わたしが小さいときに,村の茂平もへいというおじいさんからきいたお話です。
// 	むかしは、私たちの村のちかくの、中山なかやまというところに小さなお城があって、
// 	中山さまというおとのさまが、おられたそうです。
// 	その中山から、少しはなれた山の中に、「ごん狐ぎつね」という狐がいました。
// 	ごんは、一人ひとりぼっちの小狐で、しだの一ぱいしげった森の中に穴をほって住んでいました。
// 	そして、夜でも昼でも、あたりの村へ出てきて、いたずらばかりしました。`

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

		resStr := strings.Join([]string{
			"HTTP/1.1 200 OK",
			"Content-Type: text/plain",
			"Transfer-Encoding: chunked",
			"", "",
		}, "\r\n")

		// レスポンスを書く
		// http.Responceはファイルサイズを指定しないといけないので今回はconnに直接書く
		fmt.Fprint(conn, resStr)

		for _, content := range contents {
			bytes := []byte(content)
			fmt.Fprintf(conn, "%x\r\n%s\r\n", len(bytes), content)
		}
		fmt.Fprint(conn, "0\r\n\r\n")
	}
}
