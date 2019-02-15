package observer

import (
	"testing"
)

// TODO test
func TestOnserver(t *testing.T) {
	// assert := assert.New(t)
	cnOb1 := &ConcreteObserver{name: "cnOb1"} // 暗黙の型変換は?
	cnOb2 := &ConcreteObserver{name: "cnOb2"} // &を外すとなんでエラーになる?

	cnSb1 := &ConcreteSubject{name: "cnSb1"}
	cnSb1.AddObserver(cnOb1)
	cnSb1.NotifyObservers("Hello!")

	cnSb2 := &ConcreteSubject{name: "cnSb2"}
	cnSb2.AddObserver(cnOb1)
	cnSb2.AddObserver(cnOb2)
	cnSb2.NotifyObservers("Ola!")

	// assert.Equal(Print("cnSb1", "cnOb1", "Hello!"), cnSb1.NotifyObservers("Hello!"))
}
