package ws

import (
	"net/http"
	"time"
)

const (
	// Time allowed to write a message to the peer.
	writeWait = 10 * time.Second

	// Time allowed to read the next pong message from the peer.
	pongWait = 60 * time.Second

	// Send pings to peer with this period. Must be less than pongWait.
	pingPeriod = (pongWait * 9) / 10

	// Maximum message size allowed from peer.
	maxMessageSize = 512
)

type jsonReversi struct {
	FuncName   string `json:"funcName"`
	Body       string `json:"body"`
	SystemFlag bool   `json:"systemFlag"`
}

// TODO room,dataの順? data,roomの順?
type message struct {
	room string
	data []byte
	conn *connection // 送らない相手(自分のこと)
}

type uniMessage struct {
	room string
	data []byte
	conn *connection // 送る相手
}

// TODO この関数を gollira.muxを使ってなくす
func parse(key string, vars map[string][]string) string {
	vals, ok := vars[key]
	var val string
	if ok && len(vals) >= 1 {
		val = vals[0]
	}
	return val
}

// serveWs handles websocket requests from the peer.
func ServeWs(w http.ResponseWriter, r *http.Request) error {
	ws, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		return err
	}
	//vars := r.URL.Query()

	c := &connection{send: make(chan []byte, 256), ws: ws}
	s := subscription{c, "tmp"}
	h.register <- s
	go s.writePump()
	// XXX なぜもとのやつに go がない?
	go s.readPump()
	return nil
}
