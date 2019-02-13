const $h1 = $('h1');
const $text = $('.text>input');
const $size = $('.size>input');
const $color = $('.color>input');
const $combined = $('#combined');

text = new Rx.BehaviorSubject($text.val());
size = new Rx.BehaviorSubject($size.val());
color = new Rx.BehaviorSubject($color.val());

text.subscribe(val => $h1.text(val));
size.subscribe(val => $h1.css('font-size', val + 'px'));
color.subscribe(val => $h1.css('color', val));

bind = (type, elem, subject) => {
    Rx.Observable.fromEvent(elem, type)
        .subscribe(e => subject.next(e.target.value));
};

text.combineLatest(size, color, (text, size, color) => {
    return "text: " + text + "<br>Size: " + size + "px<br>Color: " + color;
}).subscribe((val) => { 
    return $combined.html(val);
});

bind('keyup', $text, text);
bind('keyup change', $size, size);
bind('change', $color, color);