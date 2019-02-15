package main

import (
	"./handlers"
	"./ws"

	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
)

func main() {
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	e.Use(middleware.CORS())

	ws.RunHab()
	e.GET("/ws", handlers.GetWs)

	// Start server
	//e.Run(standard.New(":1323"))
	e.Start(":23456")
}
