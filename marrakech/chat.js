var app = require('express')();//expressを使うため
var http = require('http').Server(app);//expressを使って通信を扱う
var io = require('socket.io')(http);//socketを使うため
var POST = 3000;//localhost:3000

var room_id = 0;

//ルートディレクトリにアクセスした時に動く処理
app.get('/', function(req, res) {
  //index.htmlにリダイレクトする
  res.sendFile(__dirname + '/index.html');
});

//socket.ioに接続された時に動く処理
io.on('connection', function(socket) {
  //接続時に振られた一意のIDをコンソールに表示
  console.log('入室したID : %s', socket.id);

  var current_room_id = 0;

  //接続時に全員にIDを表示（messageというイベントでクライアント側とやりとりする）
  io.emit('message', socket.id + 'さんが入室しました！', 'System');

  // Room管理
  socket.on('change_room', function(room) {
      io.emit('message', room + "にルームを変更したよ！", socket.id);
      socket.leave(current_room_id);
      socket.join(room);
      current_room_id = room;
      io.to(socket.id).emit('change_room', current_room_id);
  });
  
  // Roomの一覧を取得
  socket.on('get_rooms', function() {
      io.to(socket.id).emit('show_room_list', io.sockets.adapter.rooms);
  });

  // クライアントがRoomを追加
  socket.on('add_room', function() {
      socket.leave(current_room_id);
      socket.join(room_id);
      io.to(socket.id).emit('message', room_id+"に入ったよ！", "ForYou");
      io.to(socket.id).emit('change_room', room_id);
      current_room_id = room_id;
      room_id++;
  });

  // クライアントから指示を受けた部屋に接続
  socket.on('join_room', function(room) {
      socket.leave(current_room_id);
      socket.join(Number(room));
      current_room_id = Number(room);
      io.to(socket.id).emit('message', current_room_id+"に入ったよ！", "ForYou");
      io.to(socket.id).emit('change_room', Number(room));
  });

  //messageイベントで動く
  //全員に取得したメッセージとIDを表示
  socket.on('message', function(obj) {
    io.to(obj.room).emit('message', obj.msg, socket.id);
  });

  //接続が切れた時に動く
  //接続が切れたIDを全員に表示
  socket.on('disconnect', function(e) {
    console.log('接続が切れたID : %s', socket.id);
    // 退室はこれでOK
    io.emit('message', socket.id + 'さんが退出しました！', 'System');
  });
});

//接続待ち状態になる
http.listen(POST, function() {
  console.log('接続開始', POST);
});