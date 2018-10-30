package main

import (
	"fmt"
	"log"
	"net"
)

type image_id_t [8]byte

const filePath = "../unix_domain.uds"

func VMCreate(imageID string, createReqMetadata string) error {
	conn, err := net.Dial("unix", filePath)
	if err != nil {
		log.Printf("error: % \n", err)
		return err
	}
	defer conn.Close()

	message := ""
	// var eid uint64
	// eid = 0x1
	// eidStr := string([]byte(strconv.FormatUint(uint64(eid), 16)))

	message += "00000001"
	message += "that is collect !:)"

	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Printf("error: %v\n", err)
		return err
	}
	log.Printf("send: %s\n", message)
	// これをしないとEOFが通知されずにレスポンスの処理まで進んでくれない
	err = conn.(*net.UnixConn).CloseWrite()
	if err != nil {
		log.Printf("error: %v\n", err)
		return err
	}
	return nil
}

func main() {
	imageID := ""
	createReqMetadata := ""

	err := VMCreate(imageID, createReqMetadata)
	if err != nil {
		fmt.Println(err.Error())
	}
}
