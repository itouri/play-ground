package factory_method

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestFactoryMethod(t *testing.T) {
	owners := []string{"A", "B", "C"}
	concreteCreate := ConcreteCreate{}
	var products [3]ConcreteProduct
	for i, owner := range owners {
		products[i] = concreteCreate.FactoryMethod(owner)
	}

	for i, product := range products {
		assert.Equal(t, owners[i], product.Use())
	}
}
