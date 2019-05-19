$(document).ready(function () {
    var free_fields = document.getElementsByClassName('block');
    var wait = document.getElementById('wait');
    console.log(free_fields);
    var socket = io.connect('http://' + document.domain + ':' + location.port);
    socket.on('*', function () {
    });
    socket.on('connect', function () {
        socket.emit('my event', {data: 'I\'m connected!'});
    });
    socket.on('gas_val', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[0].style.display = 'block';
        free_fields[0]['childNodes'][5].textContent = msg.data;
    });
    socket.on('gas_tag', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[0]['childNodes'][3].textContent = msg.data;
    });


    socket.on('tag_2', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[1]['childNodes'][3].textContent = msg.data;
    });
    socket.on('val_2', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[1].style.display = 'block';
        free_fields[1]['childNodes'][5].textContent = msg.data;
    });


    socket.on('tag_3', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[2]['childNodes'][3].textContent = msg.data;
    });
    socket.on('val_3', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[2].style.display = 'block';
        free_fields[2]['childNodes'][5].textContent = msg.data;
    });


    socket.on('tag_4', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[3]['childNodes'][3].textContent = msg.data;
    });
    socket.on('val_4', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[3].style.display = 'block';
        free_fields[3]['childNodes'][5].textContent = msg.data;
    });


    socket.on('tag_5', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[4]['childNodes'][3].textContent = msg.data;
    });
    socket.on('val_5', function (msg) {
        wait.style.visibility = 'hidden';
        free_fields[4].style.display = 'block';
        free_fields[4]['childNodes'][5].textContent = msg.data;
    });
});