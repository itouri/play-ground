// 通信用 
const socket = io();
var room_id = 0;

// jQueryの処理
$(() => {
　　// サブミットボタンを押された処理
    $('#chat').submit(() => {
    　　　// 入力フォームには入っている値を取得
        var message = $('#msj').val();
    　　　　　　　　　　　　　　　　
    　　　　// 値がなかったら終了
        if (message === '') return false;

    　　　// 全体にmessage処理（サーバーサイドにメッセージを渡す）
        socket.emit('message', {
            'room': room_id,
            'msg': $('#msj').val()
        });

    　　　　// 入力フォームを空にしてフォーカスする
        $('#msj').val('').focus();
    　　　　　　　　　　　　　　　　// 処理終了
        return false;
    });

    $('#room_id').change(() => {
        socket.emit('change_room', $('#room_id').val());
    });

    $('#get_rooms').click(() => {
        socket.emit('get_rooms');
    });

    $('#make_room').click(() => {
        socket.emit('make_room');
        $('#make_room').hide();
        $('#leave_room').show();
    });

    $('#leave_room').click(() => {
        socket.emit('leave_room');
        $('#make_room').show();
        $('#leave_room').hide();
    });

    //$('.join_room').click(function() {  // appendした要素には .clickは使えないらしい
    $(document).on('click', ".join_room", () => {
        // TODO
        socket.emit('join_room', $(this).attr("value"));
    });
});

// サーバーサイドから来たメッセージを挿入
socket.on('message', (msj, id) => {
    $('#message').append($('<li>').text(id + " : " + msj));
    //$('#message').text(id + " : " + msj);
});

// Roomの一覧を取得
socket.on('show_room_list',(rooms) => {
    $('#rooms').empty();
    for (var room in rooms) {
        // 参考:https://stackoverflow.com/questions/6631501/how-to-list-rooms-on-socket-io-nodejs-server
        if (!rooms[room]['sockets'].hasOwnProperty(room)) {
            var btn = $('<input class="join_room" type="button" value='+room+'>');
            $('#rooms').append(btn);
        }
    }
});

socket.on('change_room', (room) => {
    room_id = room;    
});