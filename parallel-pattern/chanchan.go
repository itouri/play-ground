package main

// chan chan により先にchanに入ってきた順に実行する機能を実現

import (
	"fmt"
	"time"
)

func sleep(numsChan chan int, n int, wait time.Duration) {
	time.Sleep(wait * time.Second)
	fmt.Printf("end sleep: %d\n", n)
	numsChan <- n
}

func do(numChans chan chan int, ch chan int, n int, wait time.Duration) {
	numChans <- ch
	sleep(ch, n, wait)
}

func main() {
	// バッファ付きにするの忘れてしばらく詰まった
	numChans := make(chan chan int, 3)
	end := make(chan bool)

	ch1 := make(chan int)
	ch2 := make(chan int)
	ch3 := make(chan int)

	go do(numChans, ch1, 1, 3)
	go do(numChans, ch2, 2, 2)
	go do(numChans, ch3, 3, 1)

	go func(end chan bool) {
		for numChan := range numChans {
			num := <-numChan
			fmt.Printf("main: %d\n", num)
			if num == 3 {
				end <- true
			}
		}
	}(end)

	<-end
}
