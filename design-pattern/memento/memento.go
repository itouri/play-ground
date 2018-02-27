package memento

// Originatorの内部状態を保存する
type Memento struct {
	state string
	value int
}

func (m *Memento) getState() (string, int) {
	return m.state, m.value
}

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

// 保存したMemento達を管理する．
// この構造体はtestの方にあるべきかも
// type Caretaker struct {
// 	savedStates []*Memento
// }

// func (c *Caretaker) AddMemento(m *Memento) {
// 	c.savedStates = append(c.savedStates, m)
// }

// func (c *Caretaker) GetMemento(index int) *Memento {
// 	if 0 <= index && index < len(c.savedStates) {
// 		return c.savedStates[index]
// 	}
// 	return nil
// }
