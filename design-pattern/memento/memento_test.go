package memento

import (
	"fmt"
	"testing"
)

func TestMemento(t *testing.T) {
	caretaker := make(map[string]*Memento)
	originator := &Originator{}
	// すごい originator.CrateMemento("State1", 10)って書きたくなる
	originator.Set("State1", 10)
	caretaker["State1"] = originator.CreateMemento()
	originator.Set("State2", 20)
	caretaker["State2"] = originator.CreateMemento()
	originator.RestoreMemento(caretaker["State1"])
	fmt.Println(originator.value)
	// caretaker.AddMemento(originator.CreateMemento())
	// originator.RestoreMemento(caretaker.GetMemento(0))
}
