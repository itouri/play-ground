package main

// chan chan により先にchanに入ってきた順に実行する機能を実現

import (
	"fmt"
	"sync"
	"time"
)

func do(wg *sync.WaitGroup, numChans chan chan int, ch chan int, n int, wait time.Duration) {
	numChans <- ch
	time.Sleep(wait * time.Second)
	fmt.Printf("end sleep: %d\n", n)
	ch <- n
	wg.Done()
}

func main() {
	// バッファ付きにするの忘れてしばらく詰まった
	numChans := make(chan chan int, 3)
	var wg sync.WaitGroup
	wg.Add(3)

	ch1 := make(chan int)
	ch2 := make(chan int)
	ch3 := make(chan int)

	go do(&wg, numChans, ch1, 1, 3)
	go do(&wg, numChans, ch2, 2, 2)
	go do(&wg, numChans, ch3, 3, 1)

	go func() {
		for numChan := range numChans {
			num := <-numChan
			fmt.Printf("main: %d\n", num)
		}
	}()
	wg.Wait()
}
