class Game {
    constructor() {
        this.COL = 8;
        this.COLXCOL = this.COL * this.COL;
        this.BLACK = 1;
        this.WHITE = -1;
        this.initGame();
    }

    initGame() {
        this.field = [
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, -1, 1, 0, 0, 0,
            0, 0, 0, 1, -1, 0, 0, 0,
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
        if (field[selected] != 0) {
            return list;
        }
        for (var x_inc = -1; x_inc <= 1; x_inc++) {
            for (var y_inc = -1; y_inc <= 1; y_inc++) {
                if (x_inc === 0 && y_inc === 0) {
                    continue;
                }
                target = [];
                L: for (var _x = x + x_inc, _y = y + y_inc; _x >= 0 & _y >= 0 && _x < COL && _y < COL; _x = _x + x_inc, _y = _y + y_inc) {
                    if (field[_y * COL + _x] * turn > 0) {
                        list = list.concat(target);
                        break L;
                    } else if (field[_y * COL + _x] == 0) {
                        break L;
                    } else {
                        target.push(_y * COL + _x);
                    }
                }
            }
        }
        return list;
    }
    
    isCanPut(selected) {
        var x = (selected % COL) | 0;
        var y = (selected / COL) | 0;
        var target = [];
        if (field[selected] != 0) {
            return false;
        }
        for (var x_inc = -1; x_inc <= 1; x_inc++) {
            for (var y_inc = -1; y_inc <= 1; y_inc++) {
                if (x_inc === 0 && y_inc === 0) {
                    continue;
                }
                target = [];
                L: for (var _x = x + x_inc, _y = y + y_inc; _x >= 0 & _y >= 0 && _x < COL && _y < COL; _x = _x + x_inc, _y = _y + y_inc) {
                    if (field[_y * COL + _x] * turn > 0) {
                        if (target.length > 0) {
                            return true;
                        }
                        break L;
                    } else if (field[_y * COL + _x] == 0) {
                        break L;
                    } else {
                        target.push(_y * COL + _x);
                    }
                }
            }
        }
        return false;
    }

    getNodeList() {
        var node_list = [];
        for (var i = 0; i < COLXCOL; i++) {
            if (canPut(field, i, turn)) {
                node_list.push(i);
            }
        }
        return node_list;
    }

    putField(selected) {
        if (!isCanPut(selected)) {
            return false;
        }

        var effectArray = getEffectArray(selected);
        var _field = field.concat();
        _field[number] = turn;
        for (var i = 0; i < effectArray.length; i++) {
            var _number = effectArray[i] | 0;
            switch (_field[_number]) {
            case 1:
                _field[_number] = -1;
                break;
            case -1:
                _field[_number] = +1;
                break;
            }
        }
        this.turn *= -1; //TODO 関数化
        return true;
    }
}