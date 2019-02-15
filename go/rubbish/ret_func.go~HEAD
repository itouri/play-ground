package main

import (
	"fmt"
)

func NewSumFunc(max int) func(int) error {
	return func(min int) error {
		if min < 0 {
			return fmt.Errorf("Cannot calc under 0 num")
		}
		sum := 0
		for i := min; i < max+1; i++ {
			sum += i
		}
		fmt.Printf("sum of %d -> %d = %d", min, max, sum)
		return nil
	}
}

func main() {
	NewSumFunc(100)(1)
}
