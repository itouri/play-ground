package main

import (
	"fmt"
	"reflect"
)

func main() {
	var a interface{} = 1
	a = nil
	if a == nil {
		aa := reflect.ValueOf(a)
		fmt.Println(aa.Type())
	}
}
