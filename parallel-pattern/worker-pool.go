package main

/*
worker pool を使って決められた goroutine の数で大きなタスクを処理する
ArrayNumの要素数を持つ配列を足し算する
*/

import (
	"fmt"
	"math/rand"
	"runtime"
	"sync"
	"time"
)

const (
	// 足し算する配列の要素数，中身はランダムで初期化
	ArrayNum = 100000000
	// 各goroutineに一度に計算させたい要素数
	EachCalcNum = 10000
)

func worker(wg *sync.WaitGroup, taskChan chan int, sumChan chan int, calcArray *[ArrayNum]int) {
	// main()で溜めておいた task をひたすら実行する
	// taskがなくなったらmain()のwg.wait()がmain()を終わらせてくれる
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
		// taskChanに計算してほしい先頭の添字を入れまくる
		// この段階ではひたすら chan の buffer にこの添字が溜まっていく
		taskChan <- i
	}

	// 使えるCPUだけgoroutineを起動する
	for i := 0; i < runtime.NumCPU(); i++ {
		go worker(&wg, taskChan, sumChan, calcArray)
	}

	// workerがsumChanに各goroutineの計算結果を投げてくるので計算する
	go func(sum *int, sumChan chan int) {
		for num := range sumChan {
			*sum += num
		}
	}(&sum, sumChan)

	wg.Wait()
	fmt.Printf("ans = %d\n", ans)
	fmt.Printf("sum = %d\n", sum)
}
