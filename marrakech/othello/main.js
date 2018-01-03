var ctx = ctx;
var evented = false;
var game = new Game();
var Render = new Render();
var revision = 0;
var point_x = 0;
var point_y = 0;
var COL = 8;

var CELL_SIZE = RECT_CANV_w / COL | 0;

// var RECT_CANV = {
//     x: 0,
//     y: 0,
//     w: 500,
//     h: 500
// };

// var RECT_BOARD = {
//     x: 0,
//     y: 0,
//     w: 500,
//     h: 500
// };

var RECT_CANV_x = 0;
var RECT_CANV_y = 0;
var RECT_CANV_w = 500;
var RECT_CANV_h = 500;

var RECT_BOARD_x = 0;
var RECT_BOARD_y = 0;
var RECT_BOARD_w = 500;
var RECT_BOARD_h = 500;

function init() {
    if (!evented) {
        setEvents();
        evented = true;
    }
    revision = 0;
    point_x = 0;
    point_y = 0;
}

function setEvents() {
    var isTouch;
    if ('ontouchstart' in window) {
        isTouch = true;
    } else {
        isTouch = false;
    }
    if (isTouch) {
        ctx.canvas.addEventListener('touchstart', ev_mouseClick)
    } else {
        ctx.canvas.addEventListener('mousemove', ev_mouseMove)
        ctx.canvas.addEventListener('mouseup', ev_mouseClick)
    }
}

function getMousePosition(e) {
    if (!e.clientX) { //SmartPhone
        if (e.touches) {
            e = e.originalEvent.touches[0];
        } else if (e.originalEvent.touches) {
            e = e.originalEvent.touches[0];
        } else {
            e = event.touches[0];
        }
    }
    var rect = e.target.getBoundingClientRect();
    point_x = e.clientX - rect.left;
    point_y = e.clientY - rect.top;
}

function hitTest(x, y) {
    // var objects = [RECT_BOARD];
    var click_obj = null;
    var selected = {
        name: "",
        value: 0
    }
    // for (var i = 0; i < objects.length; i++) {
    //     if (objects[i].w >= x && objects[i].x <= x && objects[i].h >= y && objects[i].y <= y) {
    //         selected.name = "RECT_BOARD";
    //         break;
    //     }
    // }

    if (RECT_BOARD_w >= x && 
        RECT_BOARD_x <= x && 
        RECT_BOARD_h >= y && 
        RECT_BOARD_y <= y) {
        selected.name = "RECT_BOARD";
    }

    switch (true) {
    case selected.name === "RECT_BOARD":
        selected.name = "RECT_BOARD";
        selected.value = Math.floor(y / CELL_SIZE) * COL + Math.floor(x / CELL_SIZE)
        break;
    }
    return selected;
}

function ev_mouseMove(e) {
    getMousePosition(e);
    var selected = hitTest(point_x, point_y);
    Render.render(ctx, selected.value, game.field);
}

function ev_mouseClick(e) {
    var selected = hitTest(point_x, point_y);
    if (selected.name === "RECT_BOARD") {
        if (game.putField(selected.value) === true) {
            revision += 1;
            render(ctx, selected.value, game.field);
        }
    }
}
