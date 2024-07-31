var debug = false;

var host = 'iot.eclipse.org';
var port = 80;
var username = '';
var password = '';
var path = '/ws';
var topic = 'WS/RWS/#';
var useTLS = false;
var clientId = 'RwsPanel' + parseInt(Math.random() * 100, 10);
var cleansession = true;
var reconnectTimeout = 2000;

var mqtt;
var ctrlAmbBright = new Array;
var srcLevelChart = new Array;
var srcLevel = new Array;
var dstLevelChart = new Array;
var dstLevel = new Array;


function loadConfigFile() {
	
    $.ajax({
        url: 'settings.json',
        type: 'HEAD',
        async: true,
		success: function (dta) {
			$.getJSON('settings.json', function(data) {
			host             = data.host
			port             = data.port
            username         = data.username
            password         = data.password
			path             = data.path
			topic            = data.topic
			useTLS           = data.useTLS
			cleansession     = data.cleansession
			reconnectTimeout = data.reconnectTimeout
			})
			.done(function() { MQTTconnect(); })
			.fail(function() { alert("fail"); });
		},
		error: function (jqXHR, textStatus, errorThrown) {
			alert("No configuration file found! Using default values.");
			//In this case, there shoud be a settings.json file with the following content:
			//{
			//"host": "iot.eclipse.org",
			//"port": "80",
			//"path": "/ws",
			//"topic": "WS/RWS/#",
			//"useTLS": false,
			//"cleansession": true,
			//"reconnectTimeout": 2000
			//}
			
			MQTTconnect();
        }
    });
};

function MQTTconnect() {
//console.log("myPort: " + my_port);
    if (typeof path == "undefined") {
        path = '/mqtt';
    }
    mqtt = new Paho.Client(host, port, path, clientId);
    var options = {
        userName: username,
        password: password,
        timeout: 3,
        useSSL: useTLS,
        cleanSession: cleansession,
        onSuccess: onConnect,
        onFailure: function (message) {
            $('#status').html("Connection failed: " + message.errorMessage + " Retrying...");
            setTimeout(MQTTconnect, reconnectTimeout);
        }
    };
    mqtt.onConnectionLost = onConnectionLost;
    mqtt.onMessageArrived = onMessageArrived;
    console.log("Host: "+ host + ", Port: " + port + ", Path: " + path + ", TLS: " + useTLS + ", Client Id: " + clientId);
    mqtt.connect(options);
};


function onConnect() {
    $('#status').html('Connected to ' + host + ':' + port + path);
    mqtt.subscribe(topic, {qos: 0});
    $('#topic').html(topic);
};


function onConnectionLost(responseObject) {
    setTimeout(MQTTconnect, reconnectTimeout);
    $('#status').html("Connection lost: " + responseObject.errorMessage + ". Reconnecting...");
};


function onMessageArrived(message) {
    var topic = message.destinationName;
    var payload = message.payloadString;
    //console.log("Topic: " + topic + ", Message payload: " + payload);
    $('#message').html(topic + ', ' + payload);
    //var timestamp = Math.round((new Date()).getTime() / 1000);
    switch (topic) {
            
        // CONTROLLER

        case 'WS/RWS/DG/Controller/Status': 
            $('#CtrlStatusValue').html('(Payload value: ' + payload + ')');
            $('#CtrlStatusLabel').text(payload + '');
            break;

        case 'WS/RWS/DG/Controller/AmbientBrightness': 
            $('#CtrlAmbBrightValue').html('(Payload value: ' + payload + ')');
            $('#CtrlAmbBrightLabel').text(payload + '?');
            ctrlAmbBright.push(parseInt(payload));
            if (ctrlAmbBright.length >= 200) {
                ctrlAmbBright.shift();
            }
            $('.CtrlAmbBrightSparkline').sparkline(ctrlAmbBright, {
                type: 'line',
                tooltipSuffix: " ?",
                width: 'auto',
                height: '25'
            });
            break;

        case 'WS/RWS/Dashboard/ManualPumpReq': 
            $('#ManualPumpReqValue').html('(Payload value: ' + payload + ')');
            if (payload == '0'){
                $('#ManualPumpReqLabel').text('Off');
            }
            else if (payload == '1'){
                $('#ManualPumpReqLabel').text('On');
            }
            else {
                $('#ManualPumpReqLabel').text('Unk');
            }
            break;

        case 'WS/RWS/Dashboard/ManualPumpAck': 
            $('#ManualPumpAckValue').html('(Payload value: ' + payload + ')');
            if (payload == '0'){
                $('#ManualPumpAckLabel').text('Off');
            }
            else if (payload == '1'){
                $('#ManualPumpAckLabel').text('On');
            }
            else {
                $('#ManualPumpAckLabel').text('Unk');
            }
            break;

        case 'WS/RWS/Dashboard/ManualValveReq': 
            $('#ManualValveReqValue').html('(Payload value: ' + payload + ')');
            if (payload == '0'){
                $('#ManualValveReqLabel').text('Off');
            }
            else if (payload == '1'){
                $('#ManualValveReqLabel').text('On');
            }
            else {
                $('#ManualValveReqLabel').text('Unk');
            }
            break;

        case 'WS/RWS/Dashboard/ManualValveAck': 
            $('#ManualValveAckValue').html('(Payload value: ' + payload + ')');
            if (payload == '0'){
                $('#ManualValveAckLabel').text('Off');
            }
            else if (payload == '1'){
                $('#ManualValveAckLabel').text('On');
            }
            else {
                $('#ManualValveAckLabel').text('Unk');
            }
            break;

        default: console.log('Error: Data do not match the MQTT topic. (topic: ' + topic + ', payload: ' + payload + ')'); break;
    }
};


function ToggleDebug() {
    debug = !debug;
    ShowDebugInfos();
}


function ManualFctPump(fct_mode) {
    var message = new Paho.Message(fct_mode);
    message.retained = false;
    message.qos = 1;
    message.destinationName = "WS/RWS/Dashboard/ManualPumpReq";
    mqtt.send(message);
};


function ManualFctValve(fct_mode) {
    var message = new Paho.Message(fct_mode);
    message.retained = false;
    message.qos = 1;
    message.destinationName = "WS/RWS/Dashboard/ManualValveReq";
    mqtt.send(message);
};


function AddEventHandlers() {
	
    $('#ToggleMoreInfos').click(function(){ToggleDebug()});
    
    $('#ManFctPumpOn').click(function(){ManualFctPump('1')});
    $('#ManFctPumpOff').click(function(){ManualFctPump('0')});
    
    $('#ManFctValveOn').click(function(){ManualFctValve('1')});
    $('#ManFctValveOff').click(function(){ManualFctValve('0')});
	
    //Change drop-down label
	$(".dropdown-menu .dropdown-item").click(function(){$("#dropdownMenu2").html($(this).text()+' <span class=\"caret\"></span>');});
}

function ShowDebugInfos() {
    var display_val = "none";

    if (debug == true) {
        display_val = "initial";
    }

    $('#CtrlStatusValue').css("display", display_val);

    $('#CtrlAmbBrightValue').css("display", display_val);

    $('#ManualPumpReqValue').css("display", display_val);
    $('#ManualPumpAckValue').css("display", display_val);

    $('#ManualValveReqValue').css("display", display_val);
    $('#ManualValveAckValue').css("display", display_val);
}


$(document).ready(function() {
	loadConfigFile();
    AddEventHandlers();
    ShowDebugInfos();
});
