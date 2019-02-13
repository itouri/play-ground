package observer

import (
	"fmt"
)

// golangでobserverやろうと思ったらチャネル使ったほうがいい気がする

type Observer interface {
	Notify(string, string)
}

type Subject interface {
	NotifyObservers(msg string)
	AddObserver()
}

type ConcreteObserver struct {
	name string
}

func (c *ConcreteObserver) Notify(subjectName string, msg string) {
	fmt.Printf("%s -> %s: %s\n", subjectName, c.name, msg)
}

type ConcreteSubject struct {
	observers []Observer
	name      string
}

func (c *ConcreteSubject) AddObserver(observer Observer) {
	c.observers = append(c.observers, observer)
}

func (c *ConcreteSubject) NotifyObservers(msg string) {
	for _, observer := range c.observers {
		observer.Notify(c.name, msg)
	}
}
