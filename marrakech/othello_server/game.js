class Game {
    const COL = 8;
    const COLXCOL = COL * COL;
    const BLACK = 1;
    const WHITE = -1;

    constructor() {
        initGame();
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

        this.turn = BLACK;
    }

    getEffectArray(field, number, turn) {
        var list = [];
        var x = (number % COL) | 0;
        var y = (number / COL) | 0;
        var target = [];
        if (field[number] != 0) {
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

    isCanPut(field, number, turn) {
        var x = (number % COL) | 0;
        var y = (number / COL) | 0;
        var target = [];
        if (field[number] != 0) {
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

    getNodeList(field, turn) {
        var node_list = [];
        for (var i = 0; i < COLXCOL; i++) {
            if (canPut(field, i, turn)) {
                node_list.push(i);
            }
        }
        return node_list;
    }

    putField(field, number, turn) {
        var effectArray = getEffectArray(field, number, turn);
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
        return _field;
    }


    evalField(field) {
        var ev = 0;
        for (var i = 0; i < COLXCOL; i++) {
            switch (field[i]) {
            case 1:
                ev += 1;
                break;
            case -1:
                ev += -1;
                break;
            }
        }
        return ev;
    }

    isEnd(field) {
        for (var i = 0; i < COLXCOL; i++) {
            if (field[i] != 0) {
                return false;
            }
        }
        return true;
    }

    getWinner(field) {
        var score = 0;
        if (!isEnd(field)) {
            return 0;
        } else {
            for (var i = 0; i < COLXCOL; i++) {
                if (map[i] < 0) {
                    score += -1;
                } else if (map[i] > 0) {
                    score += 1;
                }
            }
            if (score > 0) {
                return 1;
            } else if (score < 0) {
                return -1;
            } else {
                return 0;
            }
        }
    }
}