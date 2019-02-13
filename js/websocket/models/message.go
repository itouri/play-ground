package models

type Game struct {
	Field [64]int `validate:"required"`
	State int     `validate:"required"`
}

func (g *Game) String() string {
	return "game"
}
