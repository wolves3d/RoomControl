<?php

$host		= 'localhost';		// им€ хоста (уточн€етс€ у провайдера)
$database	= 'smart_home';		// им€ базы данных, которую вы должны создать
$user		= 'root';			// заданное вами им€ пользовател€, либо определенное провайдером
$pswd		= 'trust#1sql';		// заданный вами пароль
 
 // ѕодключаемс€ к базе
$db = mysql_connect($host, $user, $pswd) or die("Ќе могу соединитьс€ с MySQL.");
mysql_select_db($database) or die("Ќе могу подключитьс€ к базе.");

$probe_uid = $_POST["probe_uid"];
$probe_data = $_POST["probe_data"];

// -----------------------------------------------------------------------------

$query = "SELECT `id` FROM `probes` WHERE `uid`='$probe_uid'";

$result = mysql_query($query);
if (!$result)
{
	printf("sql error: %s\n", mysql_error($db));
}
			
if (0 == mysql_numrows($result))
{
	die ("Unknown sensor ID!");
}

$sensor_id = mysql_result($result, 0);
mysql_free_result($result);

// -----------------------------------------------------------------------------

$query = "INSERT INTO `temp_probe_data` (`probe_id`, `datetime`, `temp` )\r\n".
			"VALUES ('$sensor_id', NOW(), $probe_data);\r\n";
			
if (!mysql_query($query))
{
	printf("sql error: %s\n", mysql_error($db));
}			

?>