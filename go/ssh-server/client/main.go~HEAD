package main

import (
	"fmt"
	"io/ioutil"
	"log"

	"golang.org/x/crypto/ssh"
)

func clientConn() {
	// var hostKey ssh.PublicKey
	// A public key may be used to authenticate against the remote
	// server by using an unencrypted PEM-encoded private key file.

	// If you have an encrypted private key, the crypto/x509 package
	// can be used to decrypt it.
	key, err := ioutil.ReadFile("./id_rsa2")
	if err != nil {
		log.Fatalf("unable to read private key: %v", err)
	}

	// Create the Signer for this private key.
	signer, err := ssh.ParsePrivateKey(key)
	if err != nil {
		log.Fatalf("unable to parse private key: %v", err)
	}

	// signer, err = ssh.ParsePrivateKey([]byte(dummySignerStr))
	// if err != nil {
	// 	log.Fatalf("unable to parse private key: %v", err)
	// }

	config := &ssh.ClientConfig{
		User: "user",
		Auth: []ssh.AuthMethod{
			ssh.Password(""),
			ssh.PublicKeys(signer),
			// ssh.KeyboardInteractive(Challenge),
		},
		HostKeyCallback: ssh.InsecureIgnoreHostKey(),
	}

	// Connect to the remote server and perform the SSH handshake.
	client, err := ssh.Dial("tcp", "127.0.0.1:2022", config)
	if err != nil {
		log.Fatalf("unable to connect: %v", err)
	}

	session, err := client.NewSession()
	if err != nil {
		panic("Failed to create session: " + err.Error())
	}
	fmt.Println("OK!")
	defer session.Close()
}

func main() {
	clientConn()
}
