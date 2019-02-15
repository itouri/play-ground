class Renderer {
    constructor() {
        this.COL = 8;
        this.RECT_CANV = {
            x: 0,
            y: 0,
            w: 500,
            h: 500
        };
        this.RECT_BOARD = {
            x: 0,
            y: 0,
            w: 500,
            h: 500
        };
        this.CELL_SIZE = this.RECT_CANV.w / this.COL | 0;
        this.COLOR_LINE = "#FFFFFF";
        this.COLOR_WHITE = "#FFFFFF";
        this.COLOR_BLACK = "#000000";
        this.COLOR_SELECT = "#FFFFFF";

        this.COLOR_PANEL_4 = "#006400 ";
        this.COLOR_PANEL_5 = "#03a803 ";
        this.COLOR_PANEL_6 = "#04cb04";

        this.state_cache = null;
        this.prev_revision = -1;
        this.canv_cache = {
            canv_board: null,
            canv_pieaces: null,
            canv_effect: null
        };
    }

    render(ctx, selected_value, field, revision) {
        if (this.prev_revision < 0) {
            this.canv_cache.canv_board = this.drawBoard();
            this.canv_cache.canv_pieaces = this.drawPieceALL(field);
            this.canv_cache.canv_effect = this.drawEffect(selected_value);
            Renderer.RECT_BOARD = this.RECT_BOARD;
            Renderer.CELL_SIZE = this.CELL_SIZE;
        } else {
            if (revision != this.prev_revision) {
                this.canv_cache.canv_pieaces = this.drawPieceALL(field);
            }
            this.canv_cache.canv_effect = this.drawEffect(selected_value);
        }

        ctx.clearRect(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        ctx.drawImage(this.canv_cache.canv_board, 0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        ctx.drawImage(this.canv_cache.canv_pieaces, 0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        ctx.drawImage(this.canv_cache.canv_effect, 0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        this.prev_revision = revision;
    }

    drawBoard() {
        if (!this.canv_cache.canv_board) {
            this.canv_cache.canv_board = document.createElement("canvas");
            this.canv_cache.canv_board.width = this.RECT_CANV.w;
            this.canv_cache.canv_board.height = this.RECT_CANV.h;
        }
        var ctx = this.canv_cache.canv_board.getContext('2d');
        ctx.clearRect(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);

        var grad = ctx.createLinearGradient(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        grad.addColorStop(0, this.COLOR_PANEL_6);
        grad.addColorStop(0.3, this.COLOR_PANEL_5);
        grad.addColorStop(1, this.COLOR_PANEL_4);
        ctx.fillStyle = grad

        for (var x = 0; x < COL; x++) {
            for (var y = 0; y < COL; y++) {
                ctx.strokeStyle = this.COLOR_LINE;
                ctx.beginPath();
                ctx.fillRect(x * this.CELL_SIZE, y * this.CELL_SIZE, this.CELL_SIZE, this.CELL_SIZE);
                ctx.strokeRect(x * this.CELL_SIZE, y * this.CELL_SIZE, this.CELL_SIZE, this.CELL_SIZE);
            }
        }
        var canv_board2 = document.createElement("canvas");
        var ctx_board2 = canv_board2.getContext('2d');
        canv_board2.width = this.RECT_CANV.w;
        canv_board2.height = this.RECT_CANV.h;
        ctx_board2.clearRect(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        ctx_board2.globalAlpha = 0.07;
        ctx_board2.fillStyle = this.COLOR_WHITE;
        ctx_board2.beginPath();
        ctx_board2.arc(this.CELL_SIZE * 1, -3 * this.CELL_SIZE, 7 * this.CELL_SIZE, 0, Math.PI * 2, false);
        ctx_board2.fill();
        ctx.drawImage(canv_board2, 0, 0, this.RECT_CANV.w, this.RECT_CANV.h);


        return this.canv_cache.canv_board;
    }

    drawPieceALL(field) {
        if (!this.canv_cache.canv_pieaces) {
            this.canv_cache.canv_pieaces = document.createElement("canvas");
            this.canv_cache.canv_pieaces.width = this.RECT_CANV.w;
            this.canv_cache.canv_pieaces.height = this.RECT_CANV.h;
        }
        var ctx = this.canv_cache.canv_pieaces.getContext('2d');
        ctx.clearRect(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);

        for (var x = 0; x < COL; x++) {
            for (var y = 0; y < COL; y++) {
                if (field[y * COL + x] != 0) {
                    this.drawPiece(ctx, x * this.CELL_SIZE, y * this.CELL_SIZE, field[y * COL + x]);
                }
            }
        }
        return this.canv_cache.canv_pieaces;
    }

    drawPiece(ctx, x, y, number) {

        var grad = ctx.createLinearGradient(x, y, x + this.CELL_SIZE, y + this.CELL_SIZE);
        var font_color;
        var fill_color;
        if (number > 0) {
            font_color = this.COLOR_BLACK;
            fill_color = this.COLOR_WHITE;
            grad.addColorStop(0, 'rgb(180, 180, 180)');
            grad.addColorStop(0.4, fill_color);
            grad.addColorStop(1, fill_color);
        } else if (number < 0) {
            font_color = this.COLOR_WHITE;
            fill_color = this.COLOR_BLACK;
            grad.addColorStop(0, fill_color);
            grad.addColorStop(0.4, 'rgb(70, 70, 70)');
            grad.addColorStop(1, 'rgb(70, 70, 70)');
        }

        ctx.shadowBlur = 20;
        ctx.shadowColor = "rgba(0, 0, 0, 1)";
        ctx.shadowOffsetX = 5;
        ctx.shadowOffsetY = 5;

        ctx.fillStyle = grad;
        ctx.beginPath();
        ctx.fillRect(x + this.CELL_SIZE / 10, y + this.CELL_SIZE / 10, this.CELL_SIZE - 1 * this.CELL_SIZE / 5, this.CELL_SIZE - 1 * this.CELL_SIZE / 5);

        ctx.shadowColor = "rgba(0, 0, 0, 0)";;
        ctx.shadowBlur = 0;
        ctx.shadowOffsetX = 0;
        ctx.shadowOffsetY = 0;

        return ctx;
    }

    drawEffect(selected_value) {
        if (!this.canv_cache.canv_effect) {
            this.canv_cache.canv_effect = document.createElement("canvas");
            this.canv_cache.canv_effect.width = this.RECT_CANV.w;
            this.canv_cache.canv_effect.height = this.RECT_CANV.h;
        }
        var ctx = this.canv_cache.canv_effect.getContext('2d');
        var x = (selected_value % COL | 0) * this.CELL_SIZE;
        var y = (selected_value / COL | 0) * this.CELL_SIZE;

        ctx.clearRect(0, 0, this.RECT_CANV.w, this.RECT_CANV.h);
        ctx.globalAlpha = 0.5;
        ctx.fillStyle = this.COLOR_SELECT;
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.fillRect(x, y, this.CELL_SIZE, this.CELL_SIZE);

        return this.canv_cache.canv_effect;
    }

    objCopy(obj) {
        return JSON.parse(JSON.stringify(obj));
    }
}