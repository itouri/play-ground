package b

import "../a"

type B struct {
	*a.A
	ID   string
	Name string
}

func NewB() *B {
	return &B{
		A:    a.NewA(),
		ID:   "bID",
		Name: "bName",
	}
}
