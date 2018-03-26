var $text = $('.text>input');
var $h1 = $('h1');

text = new Rx.BehaviorSubject($text.val());
text.subscribe(val => $h1.text(val));

Rx.Observable.fromEvent($text, 'keyup')
    .subscribe(e => text.next(e.target.value))

// var $color, $combined, $h1, $size, $text, bind, color, size, text;
 
// $h1 = $('h1');
// $text = $('.text>input');
// $size = $('.size>input');
// $color = $('.color>input');
// $combined = $('#combined');

// text = new Rx.BehaviorSubject($text.val());
// size = new Rx.BehaviorSubject($size.val());
// color = new Rx.BehaviorSubject($color.val());

// text.subscribe(function (val) {
//     $h1.text(val);
// });

// size.subscribe(function (val) {
//     $h1.css('font-size', val + 'px');
// });

// color.subscribe(function (val) {
//     $h1.css('color', val);
// });

// bind = function (eType, elem, subject) {
//     Rx.Observable.fromEvent(elem, eType).subscribe(function (e) {
//         subject.next(e.target.value);
//     });
// };

// text.combineLatest(size, color, function (text, size, color) {
//     return "text: " + text + "<br>Size: " + size + "px<br>Color: " + color;
// }).subscribe(function (val) {
//     return $combined.html(val);
// });

// bind('keyup', $text, text);
// bind('keyup change', $size, size);
// bind('change', $color, color);