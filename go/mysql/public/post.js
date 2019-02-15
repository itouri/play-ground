$(document).ready(function () {

  // var ORIGIN = location.origin;
  var ORIGIN = "http://localhost:1323";

  var fileReader = null;

  var printStatus = function (message, status) {
    var statusArea = $('div#status');
    statusArea.css('display', '')
              .removeClass('alert-info alert-success alert-danger')
              .text(message);
    switch (status) {
      case 'success':
        statusArea.addClass('alert-success');
        break;
      case 'danger':
        statusArea.addClass('alert-danger');
        break;
      default:
        statusArea.addClass('alert-info');
        break;
    }
  };

  var enableButton = function () {
    $('button').attr('disabled', false)
               .removeClass('btn-secondary')
               .addClass('btn-primary');
  }

  var disableButton = function () {
    $('button').attr('disabled', true)
               .removeClass('btn-primary')
               .addClass('btn-secondary');
  }

  var makeValid = function (formParts) {
    formParts.removeClass('is-invalid')
             .addClass('is-valid');
  }

  var makeInvalid = function (formParts) {
    formParts.removeClass('is-valid')
             .addClass('is-invalid');
  }

  $('button#submit').on('click', function () {
    disableButton();
    printStatus('スポットを投稿しています……', 'info');
    var form = {
      spot_name  : $('#spot_name'),
      image      : $('#image'),
      latitude   : $('#latitude'),
      longitude  : $('#longitude'),
      description: $('#description'),
      hint       : $('#hint')
    };
    var isValid = true;
    if (form.spot_name.val())   { makeValid(form.spot_name); }   else { isValid = false; makeInvalid(form.spot_name); }
    if (form.description.val()) { makeValid(form.description); } else { isValid = false; makeInvalid(form.description); }
    if (form.hint.val())        { makeValid(form.hint); }        else { isValid = false; makeInvalid(form.hint); }
    //FIXME
    // if (form.image.prop('files')[0] && form.image.prop('files')[0].type.substring(0, 5) === 'image') {
    //   makeValid(form.image);
    // } else {
    //   isValid = false;
    //   makeInvalid(form.image);
    // }
    if ($.isNumeric(form.latitude.val()) && form.latitude.val() >= -90 && form.latitude.val() <= 90) {
      makeValid(form.latitude);
    } else {
      isValid = false; makeInvalid(form.latitude);
    }
    if ($.isNumeric(form.longitude.val()) && form.longitude.val() >= -180 && form.longitude.val() <= 180) {
      makeValid(form.longitude);
    } else {
      isValid = false; makeInvalid(form.longitude);
    }
    if (!isValid) {
      printStatus('入力値が無効な項目があります。', 'danger');
      enableButton();
      return;
    }
    //FIXME var encoded = fileReader.result.split('base64,')[1];
    var data = {
      spot_name  : form.spot_name.val(),
      user_id   : 1,
//        imageids   : [encoded],
      image_ids   : [1,2,3,4],
      latitude   : parseFloat(form.latitude.val()),
      longitude  : parseFloat(form.longitude.val()),
      description: form.description.val(),
      hint       : form.hint.val()
    };
    console.log(JSON.stringify(data))
    console.log(data.description)
    $.ajax({
      url        : ORIGIN + '/api/spot',
      method     : 'POST',
      contentType: 'application/json',
      data       : JSON.stringify(data)
    }).done(function (data, status, jqXHR) {
      printStatus('スポットを投稿しました。spot_id: ' + data.spot_id, 'success');
    }).fail(function (data, status, jqXHR) {
      console.error(data);
      printStatus('スポットを投稿できませんでした。', 'danger');
    }).always(function () {
      enableButton();
    });
  });

  $('button#location').on('click', function () {
    if (!navigator.geolocation) {
      printStatus('お使いのブラウザは Geolocation API に対応していません。', 'danger');
      return;
    }
    disableButton();
    printStatus('現在地を取得しています……', 'info');
    navigator.geolocation.getCurrentPosition(function (position) {
      $('#latitude').val(position.coords.latitude);
      $('#longitude').val(position.coords.longitude);
      printStatus('現在地を反映しました。', 'success');
      enableButton();
    }, function () {
      printStatus('現在地を取得できませんでした。', 'danger');
      enableButton();
    });
  });

  $('#image').on('change', function (element) {
    $('#preview').removeAttr('src');
    file = element.target.files[0]
    $('style#custom-file-control').replaceWith('<style id="custom-file-control">.custom-file-control::after {content: "' + file.name + '";}</style>')
    if (file.type.substring(0, 5) === 'image') {
      fileReader = new FileReader();
      fileReader.onload = function() {
        $('#preview').attr('src', fileReader.result);
      }
      fileReader.readAsDataURL(file);
    }
  });

});