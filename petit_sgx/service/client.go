package main

import (
	"crypto/sha256"
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"log"
	"net"
	"os/exec"

	nonce "github.com/LarryBattle/nonce-golang"
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

func createRequestMetadata(clientUUID uuid.UUID) ([]byte, error) {
	var crm []byte

	clientID, err := clientUUID.MarshalBinary()
	if err != nil {
		return nil, err
	}

	nonce := nonce.NewToken()
	hmac := sha256.Sum256(crm)

	crm = append(crm, clientID...)
	crm = append(crm, nonce...)
	crm = append(crm, hmac[:]...)

	fmt.Printf("--- createRequestMetadata ---\n")
	fmt.Printf("clentID : %v\n", clientID)
	fmt.Printf("nonce   : %v\n", nonce)
	fmt.Printf("hmac    : %v\n", hmac[:])

	return crm, nil
}

func createImageMetadata(clientUUID uuid.UUID, mrenclave []byte) ([]byte, error) {
	var imd []byte
	clientID, err := clientUUID.MarshalBinary()
	if err != nil {
		return nil, err
	}
	imd = append(imd, clientID...)
	imd = append(imd, mrenclave...)
	hmac := sha256.Sum256(imd)
	imd = append(imd, hmac[:]...)

	fmt.Printf("--- createImageMetadata ---\n")
	fmt.Printf("clentID   : %v\n", clientID)
	fmt.Printf("mrenclave : %v\n", mrenclave)
	fmt.Printf("hmac      : %v\n", hmac[:])

	return imd, nil
}

func VMCreate(imageUUID uuid.UUID) error {
	conn, err := net.Dial("unix", filePath)
	if err != nil {
		log.Printf("error: % \n", err)
		return err
	}
	defer conn.Close()

	appPath := "./vm_app"
	out, err := exec.Command(appPath).Output()

	/* image_id */
	imageID, err := imageUUID.MarshalBinary()
	if err != nil {
		return err
	}

	// tmp
	clientUUID := uuid.New()

	/* image_metadata */
	// 先にimage_metadata作らないとサイズがわからない
	mrenclave, err := hex.DecodeString(string(out))
	if err != nil {
		fmt.Printf("failed parse mrenclave : %s", err.Error())
		return err
	}

	imd, err := createImageMetadata(clientUUID, mrenclave)
	if err != nil {
		fmt.Printf("failed createImageMetadata: %s", err.Error())
	}

	/* image_metadata_size*/
	imdSz := uint32(len(imd))
	imageMetadataSize := Uint2bytes(imdSz)

	/* create_req_metadata */
	crm, err := createRequestMetadata(clientUUID)
	if err != nil {
		fmt.Printf("failed createRequestMetadata: %s", err.Error())
	}

	/* create_req_metadata_size */
	crmSz := uint32(len(crm))
	createRequestMetadataSize := Uint2bytes(crmSz)

	/* create message */
	var message []byte
	message = append(message, imageID...)
	message = append(message, imageMetadataSize...)
	message = append(message, createRequestMetadataSize...)
	message = append(message, imd...)
	message = append(message, crm...)

	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Printf("error: %v\n", err)
		return err
	}
	log.Printf("imd_size: %d", imdSz)
	log.Printf("crm_size: %d", crmSz)

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
	fmt.Println(imageID.MarshalBinary())

	err := VMCreate(imageID)
	if err != nil {
		fmt.Println(err.Error())
	}
}
