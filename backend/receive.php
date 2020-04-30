<?php
$link = mysqli_connect("localhost", "root", "JAzolkos12/", "mgr");

$json = '{"temp":69,"hum":66,"press": 69,"pm25":56,"pm10":88}'; 

// $json = file_get_contents('php://input');
// $json = trim(file_get_contents("php://input"));
$data = json_decode($json);

$temp = $data->temp;
$hum = $data->hum;
$pm25 = $data->pm25;
$pm10 = $data->pm10;

if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

sendResponse(200, [], 'ok.');

$sql2 = "SELECT * FROM dane ORDER BY id DESC LIMIT 0, 1";
$result = mysqli_query($link, $sql2);
// $row = $result->fetch_assoc();
$newID = 1;
// $newID = $row['id'] + 1;

$sql = "INSERT INTO `dane` (`id`, `time`, `temp`, `hum`, `pm25`, `pm10`) VALUES (" . $newID . ", CURRENT_TIMESTAMP, '" . $temp . "', '" . $hum . "', '" . $pm25 . "', '" . $pm10 . "')";

if(mysqli_query($link, $sql)){
    echo "luksja.";
} else {
    echo "ERROR: Could not able to execute $sql. " . mysqli_error($link);
}
mysqli_close($link);
?>