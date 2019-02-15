package main

import (
	"bytes"
	"fmt"
	"os"
)

func main() {
	buf := bytes.NewBuffer([]byte{1, 2, 3})
	buf.Write([]byte{4, 5, 6})

	b := make([]byte, 4)
	buf.Read(b)

	str := fmt.Sprintf("buf = %v, b = %v\n", buf.Bytes(), b)
	os.Stdout.Write([]byte(str))

}
