from permonitoring.usecase.interactor import Interactor


class Controller():

    interactor: Interactor

    def __init__(self, interactor: Interactor):
        self.interactor = interactor

    def SVmotionschedule(self):
        datastores = self.interactor.Input()
        self.interactor.Fileter()
        self.interactor.Process()
        self.interactor.Output(datastores)
