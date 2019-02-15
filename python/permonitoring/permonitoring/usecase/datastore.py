from typing import List
from permonitoring.domain.datastore import Datastore
from permonitoring.domain.repository.datastore import DatastoreRepository


class UcDatastore():

    datasotre_repository: DatastoreRepository

    def __init__(self, datastore_repo: DatastoreRepository):
        self.datasotre_repository = datastore_repo

    def getAllDatastore(self) -> List[Datastore]:
        datastores = self.datasotre_repository.getAllDatastore()
        return datastores
