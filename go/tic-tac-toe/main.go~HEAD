package main

import (
	"fmt"
)

type EnumMark int

// Request methods
const (
	Black EnumMark = iota
	White
	None
)

func printField(field [3][3]EnumMark) {
	for _, f := range field {
		for _, m := range f {
			switch EnumMark(m) {
			case Black:
				fmt.Printf("o")
			case White:
				fmt.Printf("x")
			case None:
				fmt.Printf("-")
			}
		}
		fmt.Printf("\n")
	}
}

func initField() [3][3]EnumMark {
	var field [3][3]EnumMark
	for x, f := range field {
		for y := range f {
			field[x][y] = None
		}
	}
	return field
}

func main() {
	fmt.Printf("Tic Tac Toe\n")
	var field [3][3]EnumMark
	field = initField()
	printField(field)
}
