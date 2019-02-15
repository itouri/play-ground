package memento

// Originatorの内部状態を保存する
type Memento struct {
	state string
	value int
}

func (m *Memento) getState() (string, int) {
	return m.state, m.value
}

// 現状を表現する構造体
type Originator struct {
	state string
	value int
}

// SetとCreateMementoの違いがあんまわからん
func (o *Originator) Set(state string, value int) {
	o.state = state
	o.value = value
}

// 自身の状態を Memento として出力
func (o *Originator) CreateMemento() *Memento {
	return &Memento{
		state: o.state,
		value: o.value,
	}
}

// Memento から自身の状態を復帰
func (o *Originator) RestoreMemento(m *Memento) {
	o.state, o.value = m.getState()
}
