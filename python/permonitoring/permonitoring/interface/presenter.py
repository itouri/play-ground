from typing import List
from permonitoring.domain.datastore import Datastore
from permonitoring.usecase.presenter import Presenter as AbsPresenter


class Presenter(AbsPresenter):

    def ViewDatastores(self, datastores: List[Datastore]):
        print(datastores)
