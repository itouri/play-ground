// 画像が右へ動いていくサンプル
enchant();

window.onload = function () {
    var game = new Game(320, 320);
    var playerInfos = [];

    // クラスがここにあるのなぁ．．．．gameをグローバル化できないかな？
    var PlayerInfo = Class.create(Sprite, {
        // idはUI表示の座標を決定するために必要
        initialize: function(id) {
            Sprite.call(this, 32, 32);
            this.x = 0;
            this.y = ( 32 + 5 ) * id // hight + margin * id
            this.image = game.assets['chara1.png'];
            
            this.playerName = id; // TODO: 接続者の名前
            this.coin = 30;
            this.carpet = 0;

            game.rootScene.addChild(this);

            var labelCoin = new Label();
            labelCoin.x = this.x + 5;
            labelCoin.y = this.y + 5;
            labelCoin.text = this.coin;
            game.rootScene.addChild(labelCoin);
        }
    });

    var carpetColors = []; // [7][7] -1で初期化 
    var carpetTurns = []; // [7][7] -1で初期化

    for(i=0; i<7; i++){
	    carpetColors[i] = [];
        carpetTurns[i] = [];
	    for(j=0; j<7; j++){
		    carpetColors[i][j] = 0;
            carpetTurns[i][j] = 0;
	    }
    }

    game.preload('chara1.png');
    game.preload('map0.png'); // 画像を追加するときはpreloadを忘れないように！

    game.onload = function() {
        var scene = new Scene();
        // TODO: 新しい接続がきしだいplayerInfoをつくる
        for (var i = 0; i < 4; i++) {
            var pI = new PlayerInfo(playerInfos.length);
            playerInfos.push(pI);   
        }
        var map = new Map(16, 16);
        map.image = game.assets['map0.png'];
        map.x = 100;
        map.loadData(carpetColors);
        game.rootScene.addChild(map);
    };
    game.start();
};