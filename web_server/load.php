<?php

$host		= 'db01.hostline.ru';	// им€ хоста (уточн€етс€ у провайдера)
$database	= 'vh52420_terma';		// им€ базы данных, которую вы должны создать
$user		= 'vh52420_root';		// заданное вами им€ пользовател€, либо определенное провайдером
$pswd		= 'trust#1sql';			// заданный вами пароль
 
 // ѕодключаемс€ к базе
$db = mysql_connect($host, $user, $pswd) or die("Ќе могу соединитьс€ с MySQL.");
mysql_select_db($database) or die("Ќе могу подключитьс€ к базе.");

echo '<?xml version="1.0"?>';

$res = mysql_query("SELECT `id_line` FROM `ww_line_info` ORDER BY `id_line` DESC LIMIT 1", $db);

if ($row = mysql_fetch_array($res))
{
	$lineCount = $row[0] + 1;

	echo "\r\n<doc>";
	while ($lineCount--)
	{
		$res = mysql_query("SELECT `id_coord` FROM `ww_line_info` WHERE `id_line`=$lineCount ORDER BY `pos` ASC", $db);
	
		$once = true;
	
		echo "\r\n\t<line>";
		while ($row = mysql_fetch_array($res))
		{
			$id_coord = $row[0];
			$request = mysql_query("SELECT * FROM `ww_line_points` WHERE `id_coord`=$id_coord", $db);
			$coords = mysql_fetch_array($request);

			if ($once)
			{
				echo "\r\n\t\t";
				echo '<global width="'.$coords['width'].'" ';
				echo 'color="'.$coords['color'].'"/>';
				$once = false;
			}
			
			echo "\r\n\t\t";
			echo '<position x="'.$coords['x'].'" ';
			echo 'y="'.$coords['y'].'" ';
			echo 'z="'.$coords['z'].'"/>';
		}
		echo "\r\n\t</line>";
	}
	echo "\r\n</doc>";
}

/*
	// ќбрабатываем каждую точку линии
	$point_count = $_POST["line_".$line_count."_point_count"];
	$line_coords = $_POST["line_".$line_count."_point_string"];
	$offset = 0;
	
	while ($point_count--)
	{
		$query =
			"INSERT INTO `ww_line_info` (`id_line`, `pos`, `id_coord`)\r\n".
			"VALUES ($line_count, $point_count, $id_coord);\r\n\r\n";
		$res = mysql_query($query);
	
		$id_end = strpos($line_coords, ",", $offset);
		$x = substr($line_coords, $offset, $id_end - $offset);
		
		$offset = $id_end + 1;
		$id_end = strpos($line_coords, ",", $offset);
		$y = substr($line_coords, $offset, $id_end - $offset);
		
		$offset = $id_end + 1;
		$id_end = strpos($line_coords, ";", $offset);
		$z = substr($line_coords, $offset, $id_end - $offset);
		
		$offset = $id_end + 1;
		
		
		$query =
			"INSERT INTO `ww_line_points` (`id_coord`, `x`, `y`, `z`)\r\n".
			"VALUES ($id_coord, $x, $y, $z);\r\n\r\n";
		$res = mysql_query($query);
		
		$id_coord++;
	}
	*/

?>