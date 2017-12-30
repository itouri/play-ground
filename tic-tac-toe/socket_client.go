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

	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	failOnError(err)
	defer conn.Close()

	sendMsg := os.Args[1]
	conn.Write([]byte(sendMsg))
}
