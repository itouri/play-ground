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
        res = self.es_handler.search(index, body)
        return res
