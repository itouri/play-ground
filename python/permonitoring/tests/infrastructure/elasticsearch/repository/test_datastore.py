import unittest
from permonitoring.domain.datastore import Datastore
from permonitoring.infrastructure.elasticsearch.repository.datastore import DatastoreRepository

resp = {
  "took": 3,
  "timed_out": False,
  "_shards": {
    "total": 10,
    "successful": 10,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": 2,
    "max_score": 1,
    "hits": [
      {
        "_index": "permonitor",
        "_type": "datastore",
        "_id": "sU5C62gBjHiU8catkAdp",
        "_score": 1,
        "_source": {
          "id": "abc",
          "vm_num": 1,
          "vmdk_num": 2,
          "provisioning_capacity": 12,
          "acutual_capacity": 20
        }
      },
      {
        "_index": "permonitor",
        "_type": "datastore",
        "_id": "CDcR-mgBF37Tj8-8d3bh",
        "_score": 1,
        "_source": {
          "id": "bcd",
          "vm_num": 2,
          "vmdk_num": 5,
          "provisioning_capacity": 25,
          "acutual_capacity": 59
        }
      }
    ]
  }
}


class TestDatasotreRepositoryMethods(unittest.TestCase):

    def mock_search(self, index: str, body: str):
        return resp

    # TODO 結局 mock 使ってないんだけどいいのかな
    def test_getAllDataStore(self):
        ds_repo = DatastoreRepository("localhost", 9200)
        ds_repo.es_handler.search = self.mock_search

        correct_dss = [
            Datastore("abc", 1, 2, 12, 20),
            Datastore("bcd", 2, 5, 25, 59)
        ]

        dss = ds_repo.getAllDatastore()
        for i in range(len(correct_dss)):
            # この方法だと AssertionError が見づらい
            self.assertEqual(dss[i], correct_dss[i])


if __name__ == "__main__":
    unittest.main()
