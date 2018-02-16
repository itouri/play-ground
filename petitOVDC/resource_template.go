package main

import "./model"

/* Hierarchy of Template
 * Resource
 * ├── Instance
 * │   ├── Lxc
 * │   └── Null
 * └── None
 */

type ResourceTemplateHandler interface {
	ResourceName() string
}

type InstanceTemplateHandler interface {
	ResourceTemplateHandler
	GetVcpu() int32
}

// なんのために必要?
func GetResourceTemplate(tmpl *model.InstanceTemplate) ResourceTemplateHandler {
	if tmpl == nil {
		return nil
	}
	return nil
}

func main() {

}
