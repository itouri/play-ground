(function (global) {
    "use strict";
    // Class ------------------------------------------------
    function Game() {}

    // Header -----------------------------------------------
    global.Game = Game;
    global.Game.initGame = initGame;

    // ------------------------------------------------------
    var COL = 8;
    var ctx;
    var evented = false;
    var state = {}
    var point = {
        x: 0,
        y: 0
    }
    var init_state = {
        map: [0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, -1, 1, 0, 0, 0,
                0, 0, 0, 1, -1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                ],
        mode: 0,
        turn: -1,
        revision: 0,
        selected: {
            name: "",
            value: 0
        }
    };
    
    function initGame(_ctx) {
        ctx = _ctx;
        state = objCopy(init_state);
        if (!evented) {
            evented = true;
            setEvents();
        }
        Render.render(ctx, state, point);
    }

})((this || 0).self || global);