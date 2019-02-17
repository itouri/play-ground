class Datastore(object):

    id: str
    vm_num: int
    vmdk_num: int
    provisioning_capacity: int
    acutual_capacity: int

    def __init__(self, id: str, vm_num: int, vmdk_num: int, 
                 provisioning_capacity: int, acutual_capacity: int) -> None:

        self.id = id
        self.vm_num = vm_num
        self.vmdk_num = vmdk_num
        self.provisioning_capacity = provisioning_capacity
        self.acutual_capacity = acutual_capacity

    def __eq__(self, other):
        if not isinstance(other, Datastore):
            return NotImplemented
        # すべてのメンバー変数で比較
        return self.__dict__ == other.__dict__
