package singleton

import "testing"

func TestSingleton(t *testing.T) {
	inst1 := GetInstance()
	inst2 := GetInstance()
	if inst1 != inst2 {
		t.Errorf("inst1 != inst2\n")
	}
}
