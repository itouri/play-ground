class Main {
    constructor(ctx) {
        this.ctx = ctx;
        this.evented = false;
        this.game = new Game();
        this.init();
    }

    init() {
        if (!this.evented) {
            this.setEvents();
            this.evented = true;
        }
        this.revision = 0;
        this.point_x = 0;
        this.point_y = 0;
    }

    setEvents() {
        var isTouch;
        if ('ontouchstart' in window) {
            isTouch = true;
        } else {
            isTouch = false;
        }
        if (isTouch) {
            this.ctx.canvas.addEventListener('touchstart', this.ev_mouseClick)
        } else {
            this.ctx.canvas.addEventListener('mousemove', this.ev_mouseMove)
            this.ctx.canvas.addEventListener('mouseup', this.ev_mouseClick)
        }
    }

    getMousePosition(e) {
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
        this.point_x = e.clientX - rect.left;
        this.point_y = e.clientY - rect.top;
    }

    hitTest(x, y) {
        var objects = [Render.RECT_BOARD];
        var click_obj = null;
        var selected = {
            name: "",
            value: 0
        }
        for (var i = 0; i < objects.length; i++) {
            if (objects[i].w >= x && objects[i].x <= x && objects[i].h >= y && objects[i].y <= y) {
                selected.name = "RECT_BOARD";
                break;
            }
        }
        switch (true) {
        case selected.name === "RECT_BOARD":
            selected.name = "RECT_BOARD";
            selected.value = Math.floor(y / Render.CELL_SIZE) * COL + Math.floor(x / Render.CELL_SIZE)
            break;
        }
        return selected;
    }

    ev_mouseMove(e) {
        this.getMousePosition(e);
        var selected = this.hitTest(this.point_x, this.point_y);
        Render.render(ctx, selected.value, this.game.field);
    }

    ev_mouseClick(e) {
        var selected = this.hitTest(this.point_x, this.point_y);
        if (selected.name === "RECT_BOARD") {
            if (game.putField(selected.value) === true) {
                this.revision += 1;
                Render.render(ctx, selected.value, this.game.field);
            }
        }
    }
}