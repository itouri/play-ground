import unittest
from permonitoring.domain.datastore import Datastore


class TestDatastoreMethods(unittest.TestCase):
    def test_datasotre_init(self):
        datastore = Datastore("abc", 1, 2, 12, 20)

        self.assertEqual(datastore.id, "abc")
        self.assertEqual(datastore.vm_num, 1)
        self.assertEqual(datastore.vmdk_num, 2)
        self.assertEqual(datastore.provisioning_capacity, 12)
        self.assertEqual(datastore.acutual_capacity, 20)


if __name__ == "__main__":
    unittest.main()
    