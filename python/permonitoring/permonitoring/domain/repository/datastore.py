from abc import ABCMeta, abstractclassmethod
from typing import List
from permonitoring.domain.datastore import Datastore


class DatastoreRepository(metaclass=ABCMeta):

    @abstractclassmethod
    def getAllDatastore(self) -> List[Datastore]:
        raise NotImplementedError
