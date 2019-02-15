from permonitoring.domain.datastore import Datastore
from permonitoring.usecase.presenter import Presenter
from permonitoring.usecase.datastore import UcDatastore
from typing import List


class Interactor():

    uc_datastore: UcDatastore
    presenter: Presenter

    def __init__(self, uc_datastore: UcDatastore, presenter: Presenter):
        self.uc_datastore = uc_datastore
        self.presenter = presenter

    def Input(self) -> List[Datastore]:
        datastores = self.uc_datastore.getAllDatastore()
        return datastores

    def Fileter(self):
        pass

    def Process(self):
        pass

    def Output(self, datastores: List[Datastore]):
        self.presenter.ViewDatastores(datastores)
