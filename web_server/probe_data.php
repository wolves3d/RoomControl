<?php

$host		= 'localhost';		// ��� ����� (���������� � ����������)
$database	= 'smart_home';		// ��� ���� ������, ������� �� ������ �������
$user		= 'root';			// �������� ���� ��� ������������, ���� ������������ �����������
$pswd		= 'trust#1sql';		// �������� ���� ������
 
 // ������������ � ����
$db = mysql_connect($host, $user, $pswd) or die("�� ���� ����������� � MySQL.");
mysql_select_db($database) or die("�� ���� ������������ � ����.");

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