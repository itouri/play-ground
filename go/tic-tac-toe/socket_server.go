package main

import (
	"fmt"
	"net"
	"os"
)

const (
	IPADDR = "127.0.0.1"
	PORT   = "55555"
)

func failOnError(err error) {
	if err != nil {
		fmt.Printf("FATAL: %s\n", err.Error())
		os.Exit(1)
	}
}

func main() {
	tcpAddr, err := net.ResolveTCPAddr("tcp", ":"+PORT)
	failOnError(err)

	listener, err := net.ListenTCP("tcp", tcpAddr)
	failOnError(err)
	defer listener.Close()

	fmt.Println("Wait for connection")
	for {
		conn, err := listener.Accept()
		if err != nil {
			continue
		}
		go handleClient(conn)
	}
}

func handleClient(conn net.Conn) {
	defer conn.Close()
	buf := make([]byte, 1024)

	fmt.Print("message:")
	for {
		n, err := conn.Read(buf)
		if n == 0 {
			break
		}
		failOnError(err)
		fmt.Printf("%s\n", string(buf[:n]))
	}
}
