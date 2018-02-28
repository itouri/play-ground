package factory_method

import (
	"fmt"
)

// Productがメンバー変数を持っていたら?
// Productができること，Concrete.AnOperation()が実行する
type Product interface {
	Use() string
}

type ConcreteProduct struct {
	User string
}

func (self *ConcreteProduct) Use() string {
	fmt.Printf("User is %s", self.User)
	return self.User
}

type Createtor interface {
	FactoryMethod() *Product
	AnOparation() // Productを使う方法
}

type ConcreteCreate struct {
}

func (self *ConcreteCreate) FactoryMethod(owner string) ConcreteProduct {
	return ConcreteProduct{owner}
}

// Productに対する実際の操作
func (self *ConcreteCreate) AnOparation(product ConcreteProduct) {
	product.Use()
}
