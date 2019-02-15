package main

/*
future-promiseでパイプラインの処理を実現
*/

import (
	"fmt"
	"math/rand"
)

const (
	ArrayNum = 1000
)

func makeArray() chan *[ArrayNum]int {
	promise := make(chan *[ArrayNum]int)
	go func() {
		calcArray := &[ArrayNum]int{}
		ans := 0
		for i := range calcArray {
			num := rand.Intn(20)
			calcArray[i] = num
			ans += num
		}
		fmt.Printf("ans = %d\n", ans)
		promise <- calcArray
	}()
	return promise
}

func calcSum(futureArray chan *[ArrayNum]int) chan int {
	promise := make(chan int)
	go func() {
		for ary := range futureArray {
			sum := 0
			for i := range ary {
				sum += ary[i]
			}
			promise <- sum
		}
	}()
	return promise
}

func main() {
	// makeArrayで計算に使う配列を作る
	futureArray := makeArray()
	futureSum := calcSum(futureArray)
	fmt.Printf("sum = %d", <-futureSum)
}
