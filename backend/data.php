<?php
$link = mysqli_connect("localhost", "root", "JAzolkos12/", "mgr");

header('Access-Control-Allow-Origin: *');
header('Content-Type: application/json; charset=UTF-8');

function sendResponse($resp_code, $data, $message){
    echo json_encode(array('code'=>$resp_code, 'message'=>$message, 'data'=>$data));
}

if($link === false){
	die("ERROR: Could not connect. " . mysqli_connect_error());
	sendResponse(500, [], 'Internal server error.');
}

$sql = "SELECT * FROM dane";
if($result = mysqli_query($link, $sql)){
    if(mysqli_num_rows($result) > 0){

    	$ids = array();
    	$times = array();
    	$temps = array();
    	$hums = array();
    	$press = array();
    	$pm25s = array();
    	$pm10s = array();

        while ($row = mysqli_fetch_array($result)) {
				array_push($ids, $row['id']);
				array_push($times, $row['time']);
				array_push($temps, (int)$row['temp']);
				array_push($hums, (int)$row['hum']);
				array_push($press, (int)$row['hum']);
				array_push($pm25s, (int)$row['pm25']);
				array_push($pm10s, (int)$row['pm10']);
		}
		
        $myObj->labels = $times;
		$myObj->temperature = $temps;
		$myObj->humidity = $hums;
		$myObj->pressure = $press;
		$myObj->pm25 = $pm25s;
		$myObj->pm10 = $pm10s;

		$myJSON = json_encode($myObj);
		sendResponse(200, $myObj, 'Data');

        mysqli_free_result($result);
    } else {
		sendResponse(404, [], 'Data not avaliable.');
    }
} else {
	sendResponse(500, [], 'Internal server error.');
}
mysqli_close($link);
?>