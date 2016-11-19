<?php

if($_SERVER["HTTP_USER_AGENT"]=="garagarduino") {
	$arduino_data=explode("|",key($_GET));
	
	$air_temperature=str_replace("_",".",$arduino_data[0]);
	$wort_temperature=str_replace("_",".",$arduino_data[1]);
	$hot_relay=$arduino_data[2];
	$cold_relay=$arduino_data[3];
	
	$graphite_host="statsd.server.tld";
	$graphite_port="8125";
	
	$batch_number="dev";
	
	exec("echo \"Homebrew.batch_$batch_number.wort_temperature:$wort_temperature|g\" |  /usr/bin/nc -w 1 -u graphite.sare.net 8125");
	exec("echo \"Homebrew.batch_$batch_number.air_temperature:$air_temperature|g\" |  /usr/bin/nc -w 1 -u graphite.sare.net 8125");
	exec("echo \"Homebrew.batch_$batch_number.hot:$hot_relay|g\" |  /usr/bin/nc -w 1 -u graphite.sare.net 8125");
	exec("echo \"Homebrew.batch_$batch_number.cold:$cold_relay|g\" |  /usr/bin/nc -w 1 -u graphite.sare.net 8125");
	
	//this file should rotate
	file_put_contents("data.csv", date("Y-m-d H:i:s").",".$air_temperature.",".$wort_temperature.",".$hot_relay.",".$cold_relay."\n",FILE_APPEND);
	
} else {
	die("here be dragons");
}



?>
