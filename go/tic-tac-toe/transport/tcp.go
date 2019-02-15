package transport

import (
	"fmt"
	"net"

	"../packet"
)

type Tcp struct {
	// // リスニングソケット.
	listener *net.TCPListener
	// // クライアントとの接続用ソケット.
	socket net.Conn

	sendQueue   packet.Queue
	recvQueue   packet.Queue
	isServer    bool
	isConnected bool

	// // イベント通知のデリゲート.
	// public delegate void 	EventHandler(NetEventState state);
	// private EventHandler	m_handler;

	// // スレッド実行フラグ.
	// protected bool			m_threadLoop = false;
	// protected Thread		m_thread = null;
	// private static int 		s_mtu = 1400;
}

func (t *Tcp) NewTcp() {
	t.sendQueue = packet.NewQueue()
	t.recvQueue = packet.NewQueue()
	t.isServer = false
	t.isConnected = false
}

func (t *Tcp) StartServer(port int, connNum int) bool {
	tcpAddr, err := net.ResolveTCPAddr("tcp", ":"+PORT)
	failOnError(err)

	t.listener, err := net.ListenTCP("tcp", tcpAddr)
	failOnError(err)
	defer t.listener.Close()

	t.isServer = true

	// for <-end {
	for {
		t.socket, err := listener.Accept()
		if err != nil {
			continue
		}
		go handleClient(conn)
	}
}

func (t *Tcp) handleClient() {
	// クライアントとの小受信を処理します.
	if (m_socket != null && m_isConnected == true) {
		// 送信処理.
		dispatchSend();
		// 受信処理.
		dispatchReceive();
	}
}


func (t *Tcp) dispatchSend() {
	t.socket.
}

func (t *Tcp) dispatchReceive() {
	
}

func (t *Tcp) IsServer() {
	return t.isServer
}

func (t *Tcp) IsConnected() {
	return t.isConnected
}

func (t *Tcp) acceptClient() {
	// if t.listener != nil && t.listener.Poll(0, SelectMode.SelectRead) {
	if t.listener != nil {
		t.socket = t.listener.Accept()
		t.isConnected = true
	}
}
