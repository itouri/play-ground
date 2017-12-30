package transport

import (
	"../packet"
)

type Tcp struct {
	// // リスニングソケット.
	// private Socket			m_listener = null;
	// // クライアントとの接続用ソケット.
	// private Socket			m_socket = null;

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

func (t *Tcp) IsServer() {
	return t.isServer
}

func (t *Tcp) IsConnected() {
	return t.isConnected
}
