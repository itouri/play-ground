package main

import (

	//"fmt"

	"net/http"

	"./handlers"

	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
)

func retAMAZON(c echo.Context) error {
	return c.String(http.StatusOK, "AMAZON")
}

func main() {
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	// これを入れないと OPTION のメソッドがさばけずエラーになる
	// TODO これがない時のエラー原因を理解する
	e.Use(middleware.CORS())

	e.GET("/api/spot/:id", handlers.GetSpot)
	e.POST("/api/spot", handlers.PostSpot)

	e.GET("/calc", handlers.GetCalc)

	e.GET("/stocker", handlers.GetStocker)

	// basic auth
	e.GET("/secret/", handlers.GetSecret, interceptor.BasicAuth())

	e.GET("/", retAMAZON)

	// Start server
	//e.Run(standard.New(":1323"))
	e.Start(":8080")
}
