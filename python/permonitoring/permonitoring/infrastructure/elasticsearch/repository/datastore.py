from permonitoring.domain.datastore import Datastore
from permonitoring.infrastructure.elasticsearch.handler import ElasticsearchHandler
from typing import List


class DatastoreRepository:

    es_handler: ElasticsearchHandler

    def __init__(self, host: str, port: int):
        self.es_handler = ElasticsearchHandler(host, port)

    def getAllDatastore(self) -> List[Datastore]:
        index = "permonitor"
        body = {"query": {"match_all": {}}}
        resp = self.es_handler.search(index, body)
        # FIXME for-append って遅くない？
        res = []
        for hits in resp['hits']['hits']:
            hit = hits['_source']
            ds = Datastore(hit['id'],
                           int(hit['vm_num']),
                           int(hit['vmdk_num']),
                           int(hit['provisioning_capacity']),
                           int(hit['acutual_capacity']))
            res.append(ds)
        return res
