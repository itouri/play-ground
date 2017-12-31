package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"strings"

	"golang.org/x/crypto/ssh"
	"golang.org/x/crypto/ssh/terminal"
)

func fatalOnError(err error, str string) {
	if err != nil {
		log.Fatalf("%s: %s", str, err.Error())
	}
}

// goroutineで分けたほうがいいな
func NewServerConn() {
	config := &ssh.ServerConfig{
		NoClientAuth: false,
		PasswordCallback: func(c ssh.ConnMetadata, pass []byte) (*ssh.Permissions, error) {
			if string(pass) != "" {
				return nil, nil
			}
			return nil, fmt.Errorf("password rejected for %q", c.User())
		},

		PublicKeyCallback: func(conn ssh.ConnMetadata, key ssh.PublicKey) (*ssh.Permissions, error) {
			// if key == nil {
			// 	fmt.Printf("key is nil\n")
			// 	return nil, nil
			// }

			// クライアントから公開鍵をパース
			pubKey, err := ioutil.ReadFile("./id_rsa.pub")
			fatalOnError(err, "Failed to parse private key")

			zkPubKey := strings.TrimSpace(string(pubKey))
			pubkey := strings.TrimSpace(string(ssh.MarshalAuthorizedKey(key)))
			if zkPubKey == pubkey {
				fmt.Printf("OK! PUB::%#v\n\n", pubkey)
				return nil, nil
			} else {
				fmt.Printf("CANT! PUB::%#v\n\n", pubkey)
				// fmt.Printf("Marshal PUB::%#v\n", key.Marshal())
				// fmt.Printf("MarshalAuth + TrimSpace PUB::%#v\n", pkey)
				// // return &ssh.Permissions{Extensions: map[string]string{"key-id": com.ToStr(pkey.ID)}}, nil
				return nil, fmt.Errorf("Private key mismatch with database public key")
			}

			pkey := strings.TrimSpace(string(ssh.MarshalAuthorizedKey(key)))
			fmt.Printf("PUB::%#v\n\n", pkey)
			// fmt.Printf("Marshal PUB::%#v\n", key.Marshal())
			// fmt.Printf("MarshalAuth + TrimSpace PUB::%#v\n", pkey)
			// // return &ssh.Permissions{Extensions: map[string]string{"key-id": com.ToStr(pkey.ID)}}, nil
			return nil, nil
		},
	}

	// 秘密鍵登録
	privateBytes, err := ioutil.ReadFile("./hostkey")
	fatalOnError(err, "Failed to parse private key: ")

	private, err := ssh.ParsePrivateKey(privateBytes)
	fatalOnError(err, "Failed to parse private key")

	config.AddHostKey(private)

	// Listen 普通のソケット通信のlisten()と同じ
	listener, err := net.Listen("tcp", "127.0.0.1:2022")
	fatalOnError(err, "failed to listen for connection")

	// ここも普通のソケット通信と同じ
	tcpConn, err := listener.Accept()
	fatalOnError(err, "failed to accept incoming connection")

	// ハンドシェイクってなんのこと言ってる？
	_, chans, reqs, err := ssh.NewServerConn(tcpConn, config)
	fatalOnError(err, "failed to handshake")

	// 何のためにあるんだこの関数は....関数名通りの処理してる
	go ssh.DiscardRequests(reqs)

	for newChannel := range chans {
		// shellに接続したい場合 type は session のはず
		if newChannel.ChannelType() != "session" {
			newChannel.Reject(ssh.UnknownChannelType, "unknown channel type")
			continue
		}
		// channelに対する accept ってどういうことだろう
		channel, requests, err := newChannel.Accept()
		fatalOnError(err, fmt.Sprintf("Could not accept channel: %v", err))

		// ??????
		go func(in <-chan *ssh.Request) {
			for req := range in {
				req.Reply(req.Type == "shell", nil)
			}
		}(requests)

		term := terminal.NewTerminal(channel, "> ")

		// ここに実際にやりたいことを書く
		go func() {
			defer channel.Close()
			for {
				line, err := term.ReadLine()
				if err != nil {
					break
				}
				if line == "dog\n" {
					fmt.Println("bow wow!")
				} else {
					fmt.Println(line)
				}
			}
		}()
	}
}

func main() {
	NewServerConn()
}
