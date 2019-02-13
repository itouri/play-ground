package memento

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestMemento(t *testing.T) {
	assert := assert.New(t)
	caretaker := make(map[string]*Memento)
	originator := &Originator{}
	// すごい originator.CrateMemento("State1", 10)って書きたくなる
	originator.Set("State1", 10)
	caretaker["State1"] = originator.CreateMemento()
	originator.Set("State2", 20)
	caretaker["State2"] = originator.CreateMemento()
	originator.RestoreMemento(caretaker["State1"])

	assert.Equal(10, originator.value)
}
