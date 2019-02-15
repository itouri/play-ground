package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"net"

	"github.com/google/uuid"
)

type image_id_t [16]byte

const filePath = "/tmp/compute/golang.uds"

// https://qiita.com/ryskiwt/items/17617d4f3e8dde7c2b8e
// Uint2bytes converts uint64 to []byte
func Uint2bytes(i uint32) []byte {
	bytes := make([]byte, 4)
	binary.LittleEndian.PutUint32(bytes, i)
	return bytes
}

func VMCreate(imageID uuid.UUID, imageMetadata []byte, createReqMetadata []byte) error {
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

	message := hex
	is := uint32(len(imageMetadata))
	message = append(message, Uint2bytes(is)...)

	cs := uint32(len(createReqMetadata))
	fmt.Println(Uint2bytes(cs))
	message = append(message, Uint2bytes(cs)...)

	message = append(message, imageMetadata...)
	message = append(message, createReqMetadata...)

	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Printf("error: %v\n", err)
		return err
	}
	log.Printf("imd_size: %d", len(imageMetadata))
	log.Printf("crm_size: %d", len(createReqMetadata))

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
	imageMetadata := "abcd"
	createReqMetadata := "efghi"

	fmt.Println(imageID.MarshalBinary())

	err := VMCreate(imageID, []byte(imageMetadata), []byte(createReqMetadata))
	if err != nil {
		fmt.Println(err.Error())
	}
}
