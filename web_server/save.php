<?php

$host		= 'db01.hostline.ru';	// ��� ����� (���������� � ����������)
$database	= 'vh52420_terma';		// ��� ���� ������, ������� �� ������ �������
$user		= 'vh52420_root';		// �������� ���� ��� ������������, ���� ������������ �����������
$pswd		= 'trust#1sql';			// �������� ���� ������
 
 // ������������ � ����
$db = mysql_connect($host, $user, $pswd) or die("�� ���� ����������� � MySQL.");
mysql_select_db($database) or die("�� ���� ������������ � ����.");

// ������� ��� �����
$res = mysql_query("DELETE FROM `ww_line_info` WHERE 1");

// ������� ��� ����������
$res = mysql_query("DELETE FROM `ww_line_points` WHERE 1");


$line_count = $_POST["line_count"];
$id_coord = 0;

// ������������ ������ �����
while ($line_count--)
{
	// ������������ ������ ����� �����
	$point_count = $_POST["line_".$line_count."_point_count"];
	$line_coords = $_POST["line_".$line_count."_point_string"];
	
	$w = $_POST["line_".$line_count."_width"];
	$c = $_POST["line_".$line_count."_color"];
	
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
		
		/*echo "x = [".$x."] ";
		echo "y = [".$y."] ";
		echo "z = [".$z."] ";*/
		
		$query =
			"INSERT INTO `ww_line_points` (`id_coord`, `x`, `y`, `z`, `width`, `color` )\r\n".
			"VALUES ($id_coord, $x, $y, $z, $w, \"$c\");\r\n\r\n";
		$res = mysql_query($query);
		
		$id_coord++;
	}
}

?>