package ws

import (
	"encoding/json"
	"log"
	"net/http"
)

// TODO いい名前が思いつかない

func trapReadMsg(s subscription, msg []byte) {
	var data jsonReversi
	if err := json.Unmarshal(msg, &data); err != nil {
		log.Print(err)
	}

	// 中身が rematch なら stoneColor を各クライアントに送信
	if data.FuncName == "rematch" {
		i := 0
		colors := []string{"-1", "1"}
		connections := h.rooms[s.room]
		for c := range connections {
			sendMsg := &jsonReversi{
				FuncName: "stoneColor",
				Body:     colors[i],
			}
			sendJSON, err := json.Marshal(sendMsg)
			if err != nil {
				log.Print(err)
			}
			m := uniMessage{s.room, sendJSON, c}
			h.unicast <- m
			i++
		}
	}
}

// func sendExit(roomID string, playerID string, connections map[*connection]bool) {
func sendExit(roomID string, playerID string, conn *connection) {
	url := "http://localhost:12345/api/v1/rooms/" + roomID + "/" + playerID
	req, err := http.NewRequest("DELETE", url, nil)
	if err != nil {
		log.Println(err)
	}
	client := new(http.Client)
	_, err = client.Do(req)
	if err != nil {
		log.Println(err)
	}

	// クライアントに exit を送信
	sendMsg := &jsonReversi{
		FuncName: "exit",
		Body:     playerID,
	}
	sendJSON, err := json.Marshal(sendMsg)
	if err != nil {
		log.Print(err)
	}
	h.broadcast <- message{roomID, sendJSON, conn}
}
