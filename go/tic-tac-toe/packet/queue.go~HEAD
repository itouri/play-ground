package packet

import (
	"math"
	"sync"
)

type packetInfo struct {
	Offset int
	Size   int
}

type Queue struct {
	// streamBuffer MemoryStream
	offsetList []packetInfo
	offset     int
	mutex      sync.Mutex
}

// コンストラクタ
func NewQueue() (q Queue) {
	// q.streamBuffer = new(MemoryStream())
	q.offsetList = []packetInfo{}
	q.offset = 0
	return
}

func (q *Queue) Enqueue(data []byte, size int) int {
	info := packetInfo{
		Offset: q.offset,
		Size:   size,
	}

	q.mutex.Lock()
	q.offsetList = append(q.offsetList, info)
	// m_streamBuffer.Position = m_offset
	// m_streamBuffer.Write(data, 0, size)
	// m_streamBuffer.Flush()
	q.offset += size
	q.mutex.Unlock()

	return size
}

func (q *Queue) Dequeue(buffer []byte, size int) int {
	if len(q.offsetList) <= 0 {
		//TODO return error?
		return -1
	}
	recvSize := 0
	q.mutex.Lock()
	info := q.offsetList[0]

	// バッファから該当するパケットデータを取得する
	dataSize := int(math.Min(float64(size), float64(info.Size))) //TODO
	// m_streamBuffer.Position = info.offset
	// recvSize = m_streamBuffer.Read(buffer, 0, dataSize)

	// pop
	if recvSize > 0 {
		q.offsetList = q.offsetList[1:]
	}

	// すべてのキューデータを取り出したときはストリームをクリアしてメモリを節約する
	if len(q.offsetList) == 0 {
		q.clear()
		q.offset = 0
	}
	return recvSize
}

// キューをクリア
func (q *Queue) clear() {
	// byte[] buffer = m_streamBuffer.GetBuffer();
	// Array.Clear(buffer, 0, buffer.Length);

	// m_streamBuffer.Position = 0;
	// m_streamBuffer.SetLength(0);
}
