var debug = false;

var host = 'iot.eclipse.org';
var port = 80;
var path = '/ws';
var topic = 'WS/RWS/#';
var useTLS = false;
var clientId = 'Panel' + parseInt(Math.random() * 100, 10);
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
        userName: "",
        password: "",
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

        case 'WS/RWS/EG/BarrelSrc/Status': 
            $('#SrcStatusValue').html('(Payload value: ' + payload + ')');
            $('#SrcStatusLabel').text(payload + '');
            break;
        case 'WS/RWS/EG/BarrelSrc/Lvl/local_timestamp': 
            $('#value0').html('(Payload value: ' + payload + ')');
            $('#label0').text(payload + '');
            break;
        case 'WS/RWS/EG/BarrelSrc/Lvl/raw_cm': 
            $('#value1').html('(Payload value: ' + payload + ')');
            $('#label1').text(payload + 'cm');
            break;
        case 'WS/RWS/EG/BarrelSrc/Lvl/cm': 
            $('#value2').html('(Payload value: ' + payload + ')');
            $('#label2').text(payload + 'cm');
            break;
        case 'WS/RWS/EG/BarrelSrc/Lvl/percent': 
            $('#value3').html('(Payload value: ' + payload + ')');
            $('#label3').text(payload + '%');
            $('#barSrc').css('width', payload + '%');
            $('#barSrc').text(payload + '%');
            srcLevel.push(parseFloat(payload));
            if (srcLevel.length >= 50) {
                srcLevel.shift();
            }
            $('.srcLevelSparkline').sparkline(srcLevel, {
                type: 'line',
                tooltipSuffix: " %",
                width: '160',
                height: '25'
            });
            break;
        case 'WS/RWS/EG/BarrelSrc/Lvl/litres': 
            $('#SrcLitresValue').html('(Payload value: ' + payload + ')');
            $('#SrcLitres').text(payload + 'l');
            break;
        case 'WS/RWS/EG/BarrelSrc/FunctionModeReq':
            $('#FunctionModeRequest').html('(Payload value: ' + payload + ')');
            $('#FunctionModeRequestLabel').removeClass('').addClass('label-default');
            $('#FunctionModeRequestLabel').text(getModeStr(payload));
            break;
        case 'WS/RWS/EG/BarrelSrc/FunctionModeAck':
            $('#FunctionModeAcknowledge').html('(Payload value: ' + payload + ')');
            $('#FunctionModeAcknowledgeLabel').removeClass('').addClass('label-default');
            $('#FunctionModeAcknowledgeLabel').text(getModeStr(payload));
            break;
            
        // DESTINATION BARREL

        case 'WS/RWS/DG/BarrelDst/Status': 
            $('#DstStatusValue').html('(Payload value: ' + payload + ')');
            $('#DstStatusLabel').text(payload + '');
            break;
        case 'WS/RWS/DG/BarrelDst/Lvl/local_timestamp': 
            $('#DstValue0').html('(Payload value: ' + payload + ')');
            $('#DstLabel0').text(payload + '');
            break;
        case 'WS/RWS/DG/BarrelDst/Lvl/raw_cm': 
            $('#DstValue1').html('(Payload value: ' + payload + ')');
            $('#DstLabel1').text(payload + 'cm');
            break;
        case 'WS/RWS/DG/BarrelDst/Lvl/cm': 
            $('#DstValue2').html('(Payload value: ' + payload + ')');
            $('#DstLabel2').text(payload + 'cm');
            break;
        case 'WS/RWS/DG/BarrelDst/Lvl/percent': 
            $('#DstValue3').html('(Payload value: ' + payload + ')');
            $('#DstLabel3').text(payload + '%');
            $('#barDst').css('width', payload + '%');
            $('#barDst').text(payload + '%');
            dstLevel.push(parseFloat(payload));
            if (dstLevel.length >= 50) {
                dstLevel.shift();
            }
            $('.dstLevelSparkline').sparkline(dstLevel, {
                type: 'line',
                tooltipSuffix: " %",
                width: '160',
                height: '30'});
            break;
        case 'WS/RWS/DG/BarrelDst/Lvl/litres': 
            $('#DstLitresValue').html('(Payload value: ' + payload + ')');
            $('#DstLitres').text(payload + 'l');
            break;
        case 'WS/RWS/DG/BarrelDst/FunctionModeReq':
            $('#DstFunctionModeRequest').html('(Payload value: ' + payload + ')');
            $('#DstFunctionModeRequestLabel').removeClass('').addClass('label-default');
            $('#DstFunctionModeRequestLabel').text(getModeStr(payload));
            break;
        case 'WS/RWS/DG/BarrelDst/FunctionModeAck':
            $('#DstFunctionModeAcknowledge').html('(Payload value: ' + payload + ')');
            $('#DstFunctionModeAcknowledgeLabel').removeClass('').addClass('label-default');
            $('#DstFunctionModeAcknowledgeLabel').text(getModeStr(payload));
            break;
            
            // CONTROLLER

            case 'WS/RWS/Dashboard/ManualPumpReq': 
                $('#ManualPumpReqValue').html('(Payload value: ' + payload + ')');
                if (payload == '0'){
                    $('#ManualPumpReqLabel').text('Off');
                }
                else{
                    $('#ManualPumpReqLabel').text('On');
                }
                break;

            case 'WS/RWS/Dashboard/ManualPumpAck': 
                $('#ManualPumpAckValue').html('(Payload value: ' + payload + ')');
                if (payload == '0'){
                    $('#ManualPumpAckLabel').text('Off');
                }
                else{
                    $('#ManualPumpAckLabel').text('On');
                }
                break;

            case 'WS/RWS/Dashboard/ManualValveReq': 
                $('#ManualValveReqValue').html('(Payload value: ' + payload + ')');
                if (payload == '0'){
                    $('#ManualValveReqLabel').text('Off');
                }
                else{
                    $('#ManualValveReqLabel').text('On');
                }
                break;

            case 'WS/RWS/Dashboard/ManualValveAck': 
                $('#ManualValveAckValue').html('(Payload value: ' + payload + ')');
                if (payload == '0'){
                    $('#ManualValveAckLabel').text('Off');
                }
                else{
                    $('#ManualValveAckLabel').text('On');
                }
                break;

        default: console.log('Error: Data do not match the MQTT topic. (topic: ' + topic + ', payload: ' + payload + ')'); break;
    }
};


function ToggleDebug() {
    debug = !debug;
    ShowDebugInfos();
}


function ManualSensMeasMode(fct_mode) {
    var message = new Paho.Message(fct_mode);
    message.retained = true;
    message.qos = 1;
    message.destinationName = "WS/RWS/EG/BarrelSrc/FunctionModeReq";
    mqtt.send(message);
    message.destinationName = "WS/RWS/DG/BarrelDst/FunctionModeReq";
    mqtt.send(message);
};


function ManualFctPump(fct_mode) {
    var message = new Paho.Message(fct_mode);
    message.retained = true;
    message.qos = 1;
    message.destinationName = "WS/RWS/Dashboard/ManualPumpReq";
    mqtt.send(message);
};


function ManualFctValve(fct_mode) {
    var message = new Paho.Message(fct_mode);
    message.retained = true;
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

    $('#ManFctSensMeasMode_200ms').click(function(){ManualSensMeasMode('0')});
    $('#ManFctSensMeasMode__5sec').click(function(){ManualSensMeasMode('1')});
    $('#ManFctSensMeasMode_10sec').click(function(){ManualSensMeasMode('2')});
    $('#ManFctSensMeasMode__1min').click(function(){ManualSensMeasMode('3')});
    $('#ManFctSensMeasMode__5min').click(function(){ManualSensMeasMode('4')});
    $('#ManFctSensMeasModeDS_20sec').click(function(){ManualSensMeasMode('5')});
    $('#ManFctSensMeasModeDS__1min').click(function(){ManualSensMeasMode('6')});
    $('#ManFctSensMeasModeDS__5min').click(function(){ManualSensMeasMode('7')});
    $('#ManFctSensMeasModeDS_10min').click(function(){ManualSensMeasMode('8')});
    $('#ManFctSensMeasModeDS_30min').click(function(){ManualSensMeasMode('9')});
    $('#ManFctSensMeasModeDS__1std').click(function(){ManualSensMeasMode(':')});
	
    //Change drop-down label
	$(".dropdown-menu .dropdown-item").click(function(){$("#dropdownMenu2").html($(this).text()+' <span class="caret"></span>');});
}

function getModeStr(mode)
{
    switch (mode){
        case '0': return 'Permanent Meassure'; break;
        case '1': return 'Interval Meassure 5 Sec'; break;
        case '2': return 'Interval Meassure 10 Sec'; break;
        case '3': return 'Interval Meassure 1 Min'; break;
        case '4': return 'Interval Meassure 5 Min'; break;
        case '5': return 'Deep Sleep Meassure 20 Sec'; break;
        case '6': return 'Deep Sleep Meassure 1 Min'; break;
        case '7': return 'Deep Sleep Meassure 5 Min'; break;
        case '8': return 'Deep Sleep Meassure 10 Min'; break;
        case '9': return 'Deep Sleep Meassure 30 Min'; break;
        case ':': return 'Deep Sleep Meassure 1 Std'; break;
        default: console.log('Error: No Function Mode for this payload.'); return 'Mode not defined';
    }
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

    $('#SrcStatusValue').css("display", display_val);
    $('#DstStatusValue').css("display", display_val);

    $('#value0').css("display", display_val);
    $('#DstValue0').css("display", display_val);

    $('#value1').css("display", display_val);
    $('#DstValue1').css("display", display_val);

    $('#value2').css("display", display_val);
    $('#DstValue2').css("display", display_val);

    $('#value3').css("display", display_val);
    $('#DstValue3').css("display", display_val);

    $('#DstLitresValue').css("display", display_val);
    $('#SrcLitresValue').css("display", display_val);

    $('#FunctionModeRequest').css("display", display_val);
    $('#DstFunctionModeRequest').css("display", display_val);

    $('#FunctionModeAcknowledge').css("display", display_val);
    $('#DstFunctionModeAcknowledge').css("display", display_val);
}


$(document).ready(function() {
	loadConfigFile();
    AddEventHandlers();
    ShowDebugInfos();
});
