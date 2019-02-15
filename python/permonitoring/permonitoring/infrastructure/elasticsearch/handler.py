from elasticsearch import Elasticsearch


class ElasticsearchHandler:

    es: Elasticsearch

    def __init__(self, host: str, port: int):
        self.es = Elasticsearch(host, port=port)

    def search(self, index: str, body: str):
        res = self.es.search(index=index, body=body)
        return res

    def get(self, index: str, body: str):
        res = self.es.get(index=index, body=body)
        return res
