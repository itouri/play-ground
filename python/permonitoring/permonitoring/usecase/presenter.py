from abc import ABCMeta, abstractclassmethod
from typing import List
from permonitoring.domain.datastore import Datastore


class Presenter(metaclass=ABCMeta):

    @abstractclassmethod
    def ViewDatastores(self, datastores: List[Datastore]):
        raise NotImplementedError
