class Game {
    constructor() {
        this.COL = 8;
        this.BLACK = -1;
        this.WHITE = 1;
        this.initGame();
    }

    initGame() {
        this.field = [
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, -1, 0, 0, 0,
            0, 0, 0, -1, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        ];
        this.turn = this.BLACK;
    }

    getEffectArray(selected) {
        var list = [];
        var x = (selected % COL) | 0;
        var y = (selected / COL) | 0;
        var target = [];
        if (this.field[selected] != 0) {
            return list;
        }
        for (var x_inc = -1; x_inc <= 1; x_inc++) {
            for (var y_inc = -1; y_inc <= 1; y_inc++) {
                if (x_inc === 0 && y_inc === 0) {
                    continue;
                }
                target = [];
                L: for (var _x = x + x_inc, _y = y + y_inc; _x >= 0 & _y >= 0 && _x < COL && _y < COL; _x = _x + x_inc, _y = _y + y_inc) {
                    if (this.field[_y * COL + _x] * this.turn > 0) {
                        list = list.concat(target);
                        break L;
                    } else if (this.field[_y * COL + _x] == 0) {
                        break L;
                    } else {
                        target.push(_y * COL + _x);
                    }
                }
            }
        }
        return list;
    }

    // おけるところがあるか調べる ないならtrue
    isPass(){ // bool
        for (var i=0; i < this.field.length; i++) {
            var effectArray = this.getEffectArray(i);
            if (effectArray.length != 0) {
                return false;
            }
        }
        return true;
    }

    putField(selected) {
        var effectArray = this.getEffectArray(selected);
        if (effectArray.length == 0) {
            return false;
        }
        this.field[selected] = this.turn;

        effectArray.forEach((effect) => {
            var number = effect | 0;
            this.field[number] = this.turn; 
        });

        this.changeTurn();
        return true;
    }

    changeTurn(){
        this.turn *= -1; //TODO 関数化
        if (this.isPass()) {
            console.log("pass!")
            this.turn *= -1;
            if (this.isPass()) {
                // gameEnd();
            }
        }
    }
}