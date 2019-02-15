package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
)

func panicOnErr(err error) {
	if err != nil {
		panic(err)
	}
	return
}

// copied
func dumpChunk(chunk io.Reader) {
	var length int32
	binary.Read(chunk, binary.BigEndian, &length)

	// read kind string
	buf := make([]byte, 4)
	chunk.Read(buf)

	fmt.Printf("chunk '%v' (%d bytes)\n", string(buf), length)
}

func readChunks(fp *os.File) []io.Reader {
	// discard the signature
	var offset int64 = 8

	//chunks := []io.Reader{}
	var chunks []io.Reader
	fp.Seek(8, 0)

	var length int32
	// var kind [4]byte
	for {
		// b := make([]byte, 4)
		// _, err := fp.Read(b)

		// glance answer
		//panicOnErr(err)

		// length
		// googling
		//buf := bytes.NewReader(b)
		err := binary.Read(fp, binary.BigEndian, &length)
		if err == io.EOF {
			break
		}
		panicOnErr(err)

		// // kind
		// err = binary.Read(buf, binary.LittleEndian, kind)
		// panicOnErr(err)

		// 値を保存してるわけじゃなくて *Fileとoffestと長さを保存してる
		// readChunks()のあとにmain()でfp.Close()するとdumpChunk()で
		// 読めなくなる
		// don't move seek postion by the io.NewSectionReader
		chunks = append(chunks,
			io.NewSectionReader(fp, offset, int64(length)+12))

		offset, _ = fp.Seek(int64(length+8), 1) // what's mean of 1?
	}
	return chunks
}

func main() {
	fp, err := os.Open("Lenna.png")
	defer fp.Close()
	panicOnErr(err)

	chunks := readChunks(fp)
	for _, chunk := range chunks {
		dumpChunk(chunk)
	}
}
