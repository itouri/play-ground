package handlers

import (
	"net/http"

	"../ws"

	"github.com/gorilla/websocket"
	"github.com/labstack/echo"
)

var (
	upgrader = websocket.Upgrader{
		// これを入れないと同一ドメインからの socket 接続要求以外に 403 を出す
		CheckOrigin: func(r *http.Request) bool {
			return true
		},
	}
)

func GetWs(c echo.Context) error {
	err := ws.ServeWs(c.Response(), c.Request())
	if err != nil {
		c.Logger().Error(err)
	}
	return c.NoContent(http.StatusOK)
}
