package main

import (
	"fmt"
	"log"
	"net"

	"github.com/google/uuid"
)

type image_id_t [16]byte

const filePath = "../unix_domain.uds"

func VMCreate(imageID uuid.UUID, createReqMetadata []byte) error {
	conn, err := net.Dial("unix", filePath)
	if err != nil {
		log.Printf("error: % \n", err)
		return err
	}
	defer conn.Close()

	hex, err := imageID.MarshalBinary()

	//message := ""
	// var eid uint64
	// eid = 0x1
	// eidStr := string([]byte(strconv.FormatUint(uint64(eid), 16)))

	message := string(hex)
	message += string(createReqMetadata)

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
	imageID := uuid.New()
	createReqMetadata := "abcdefghijklmnopqrstuvxyz"

	fmt.Println(imageID.MarshalBinary())

	err := VMCreate(imageID, []byte(createReqMetadata))
	if err != nil {
		fmt.Println(err.Error())
	}
}
