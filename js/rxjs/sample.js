var source = Rx.Observable.fromEvent($('#input'), 'click');
source.subscribe(() => {
    console.log("NExT");
});
