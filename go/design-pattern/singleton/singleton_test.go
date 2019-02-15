package singleton

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestSingleton(t *testing.T) {
	assert := assert.New(t)
	inst1 := GetInstance()
	inst2 := GetInstance()

	assert.Equal(inst1, inst2)
}
