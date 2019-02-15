var ctx;
var evented = false;
var game = new Game();
var renderer = new Renderer();
var revision = 0;
var point_x = 0;
var point_y = 0;
var COL = 8;

function init(_ctx) {
    if (!evented) {
        setEvents();
        evented = true;
    }
    revision = 0;
    point_x = 0;
    point_y = 0;
    ctx = _ctx;
    renderer.render(ctx, -1, game.field, revision);
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
    var objects = [renderer.RECT_BOARD];
    var click_obj = null;
    var selected = {
        name: "",
        value: 0
    }
    for (var i = 0; i < objects.length; i++) {
        if (objects[i].w >= x &&
            objects[i].x <= x &&
            objects[i].h >= y && 
            objects[i].y <= y) {
            selected.name = "RECT_BOARD";
            break;
        }
    }

    if (renderer.RECT_BOARD.w >= x &&
        renderer.RECT_BOARD.x <= x && 
        renderer.RECT_BOARD.h >= y &&
        renderer.RECT_BOARD.y <= y) {
        selected.name = "RECT_BOARD";
    }

    switch (true) {
    case selected.name === "RECT_BOARD":
        selected.name = "RECT_BOARD";
        selected.value = Math.floor(y / renderer.CELL_SIZE) * COL + Math.floor(x / renderer.CELL_SIZE)
        break;
    }
    return selected;
}

function ev_mouseMove(e) {
    getMousePosition(e);
    var selected = hitTest(point_x, point_y);
    renderer.render(ctx, selected.value, game.field, revision);
}

function ev_mouseClick(e) {
    var selected = hitTest(point_x, point_y);
    if (selected.name === "RECT_BOARD") {
        if (game.putField(selected.value) === true) {
            revision += 1;
            renderer.render(ctx, selected.value, game.field, revision);
            updateUI();
        }
    }
}

function updateUI() {
    var stones = game.getStoneNums();
    $("#stones").html("■"+stones.black+" - □"+stones.white);
    var str = (game.turn == game.BLACK) ? "Turn: ■" : "Turn: □";
    $("#turn").html(str);
    if(game.isEnd) {
        $("#reset").show();
    } else {
        $("#reset").hide();
    }
}

// jQuery
$(() => {
    $('#reset').click(() => {
        game.initGame();
        init(ctx);
    });
});
    