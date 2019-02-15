from permonitoring.interface.presenter import Presenter
from permonitoring.infrastructure.elasticsearch.repository.datastore import DatastoreRepository
from permonitoring.usecase.interactor import Interactor
from permonitoring.interface.controller import Controller


def main():
    HOST = "localhost"
    PORT = 9200
    datastore = DatastoreRepository(HOST, PORT)
    presenter = Presenter()
    interactor = Interactor(datastore, presenter)
    controller = Controller(interactor)
    controller.SVmotionschedule()


if __name__ == "__main__":
    main()
