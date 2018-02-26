package main

import (
	"github.com/reactivex/rxgo/observable"
	"github.com/reactivex/rxgo/observer"
)

func main() {
	// observableを作成
	emitter := make(chan interface{})
	source := observable.Observable(emitter)

	watcher := observer.Observer{
		
	}
}
