package main

/*
worker pool を使って決められた goroutine の数で大きなタスクを処理する
*/

import (
	"fmt"
	"math/rand"
	"runtime"
	"sync"
	"time"
)

const (
	ArrayNum    = 100000000
	EachCalcNum = 10000
)

func worker(wg *sync.WaitGroup, taskChan chan int, sumChan chan int, calcArray *[ArrayNum]int) {
	for task := range taskChan {
		sum := 0
		for i := task; i < task+EachCalcNum; i++ {
			sum += calcArray[i]
		}
		sumChan <- sum
		wg.Done()
	}
}

func main() {
	calcArray := &[ArrayNum]int{}
	taskNum := ArrayNum / EachCalcNum
	ans := 0
	rand.Seed(time.Now().UnixNano())
	for i, _ := range calcArray {
		num := rand.Intn(20)
		calcArray[i] = num
		ans += num
	}

	taskChan := make(chan int, taskNum)
	sumChan := make(chan int)

	var wg sync.WaitGroup
	wg.Add(taskNum)
	sum := 0

	// ArrayNum を taskNum にして下のエラーがでてずーと詰まってた
	// fatal error: all goroutines are asleep - deadlock!
	for i := 0; i < ArrayNum; i += EachCalcNum {
		taskChan <- i
	}

	for i := 0; i < runtime.NumCPU(); i++ {
		go worker(&wg, taskChan, sumChan, calcArray)
	}

	go func(sum *int, sumChan chan int) {
		for num := range sumChan {
			*sum += num
		}
	}(&sum, sumChan)

	wg.Wait()
	fmt.Printf("ans = %d\n", ans)
	fmt.Printf("sum = %d\n", sum)
}
