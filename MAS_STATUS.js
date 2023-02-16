let mysql = require('mysql');               //連接 mysql
let express = require("express");           //用於前端與後端的框架
let bodyParser = require("body-parser");    //用於解析請求資料的 Module
let json;
let app = express();

var flame = "0";
var power = "1";
var motor = "1";
var tilt = "0";

app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());                 //將資料解析成json data
app.post('/json', function(req, res){
    
    //Create mysql Connection
    let connection = mysql.createConnection({
        host: '127.0.0.1',
        user: 'fico',
        password: 'Uscc/4863',
        database: 'ESP8266_test'
    });

    json = req.body;                        //擷取 request data body json
    console.log(json);

    //mysql Connection
    connection.connect(function(e) {
        if (e) {
            return console.error('error: ' + e.message);
        }
        console.log('\nConnected to the MySQL server...\n');
    });
    if((flame != json['flame']) || (tilt != json['tilt']) || (power != json['power']) || (motor != json['motor'])){
    //對 MySQL 傳送 Query
        $insert = 'INSERT INTO MAS_STATUS (flame, tilt, motor, power) VALUES(' + json['flame'] + ',' +  json['tilt'] + ',' + json['motor'] + ',' + json['power'] + ')';
        connection.query($insert, function(e){
            if(e){
                console.log("Error ocurred in executing the query.");
                return;
            }
            console.log("Success save.");
        });
        flame = json['flame']
        tilt = json['tilt']
        power = json['power']
        motor = json['motor']
    }
    /*
    $query = 'SELECT * from button_test';
    connection.query($query, function(e, rows){
        if(e){
            console.log("Error ocurred in executing the query.");
            return;
        }
        console.log("The records of button table:\n");
        console.log("push_date\t\tpush_times\n");
        for(let row of rows){
            console.log(row['push_date'],"\t\t",row['push_times'])
        }
    });
    */

    //結束 MySQL 連接
    connection.end(function(){
        console.log('\nConnection closed.\n');
    });

    //response回Client
    res.json({
        json
    });
});

//開啟 9453 port listen request
app.listen(9453, function(req, res){
    console.log("listen...");
});