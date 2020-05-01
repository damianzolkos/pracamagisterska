<?php
$link = mysqli_connect("localhost", "root", "password", "mgr");

header('Access-Control-Allow-Origin: *');

$json = trim(file_get_contents("php://input"));
$data = json_decode($json);

$temp = $data->temp;
$hum = $data->hum;
$press = $data->press;
$pm25 = $data->pm25;
$pm10 = $data->pm10;

function sendResponse($resp_code, $data, $message){
    echo json_encode(array('code'=>$resp_code, 'message'=>$message, 'data'=>$data));
}

if($link === false){
	die("ERROR: Could not connect. " . mysqli_connect_error());
	sendResponse(500, [], 'Internal server error.');
}

$sql = "SELECT * FROM dane ORDER BY id DESC LIMIT 0, 1";
if($result = mysqli_query($link, $sql)){
    if(mysqli_num_rows($result) > 0){
        $row = $result->fetch_assoc();
        $newID = $row['id'] + 1;
        $sql2 = "INSERT INTO `dane` (`id`, `time`, `temp`, `hum`, `press`, `pm25`, `pm10`) VALUES (" . $newID . ", CURRENT_TIMESTAMP, '" . $temp . "', '" . $hum . "', '" . $press . "', '" . $pm25 . "', '" . $pm10 . "')";
        if(mysqli_query($link, $sql2)){
            echo "luksja1.";
        } else {
            echo "ERROR: Could not able to execute $sql. " . mysqli_error($link);
        }
    }
}
mysqli_close($link);
?>