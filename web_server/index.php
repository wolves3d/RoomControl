<?php

function ListFees($db)
{
	$res = mysql_query("SELECT * FROM `fees`", $db);
	
	echo '<select name="id_fee" ';
	echo 'onchange="document.getElementById(\'value\').value = this.value;">';
	while($row = mysql_fetch_array($res))
	{
		echo '<option value="'.$row['id_fee'].'">'.$row['name']."</option>";
	}
	echo "</select>";
}

function ListOwners($id_site, $db)
{	
	$owners = "�� �������";

	$res = mysql_query("
		SELECT * FROM `owners` INNER JOIN `shares` ON `shares`.id_owner = `owners`.id_owner
		WHERE `shares`.id_site = ".$id_site, $db);
	
	if ($res)
	{
		$is_first = true;
		
		while($row = mysql_fetch_array($res))
		{
			if ($is_first)
			{
				$owners = "";
				$is_first = false;
			}
			else
			{
				$owners = $owners.", ";
			}

			$owners = $owners.$row['sec_name']." ".$row['name'][0].".".$row['name2'][0].". ".$row['percent']."%";
		}
	}

	return $owners;
}


$host = 'localhost';		// ��� ����� (���������� � ����������)
$database = 'myaccount';	// ��� ���� ������, ������� �� ������ �������
$user = 'root';				// �������� ���� ��� ������������, ���� ������������ �����������
$pswd = '';					// �������� ���� ������
 
$db = mysql_connect($host, $user, $pswd) or die("�� ���� ����������� � MySQL.");
mysql_select_db($database) or die("�� ���� ������������ � ����.");


echo "<html>\r\n";
echo "<head></head>\r\n\r\n";
echo "<body>\r\n";

echo "<table border=\"1\" width=\"500\">\r\n\t";
echo "<tr><td>������������ �������</td><td>���� (���)</td><td>���� ������ (��)</td><td>��������</tr></tr>\r\n\t";
	$query = "SELECT * FROM `fees`";
	$res = mysql_query($query);
	while($row = mysql_fetch_array($res))
	{
		echo "<tr>\r\n";
		echo "\t\t<td>".$row['name']."</td><td>".$row['value']."</td><td>".$row['pay_till']."</td>\r\n";
		echo "\t\t".'<td><input type="button" name="searchbarcode" id="edit" onclick="CheckInputCode();" value="������"></td>'."\r\n";
		echo "\t</tr>\r\n";
	}
//	mysql_close($db);

	echo "\t<tr><form action=\"add_fee.php\">\r\n";
		echo "\t\t<td>".'<input type="text" id="BarCode" name="fee" size="14" maxlength="14" value="">'."</td>\r\n";
		echo "\t\t<td>".'<input type="text" id="BarCode" name="value" size="14" maxlength="14" value="">'."</td>\r\n";
		echo "\t\t<td>".'<input type="text" id="BarCode" name="date" size="14" maxlength="14" value="">'."</td>\r\n";
		echo "\t\t<td>";
//		echo "\t\t\t".'<td><form action="index.php">'."\r\n";
		echo '<button type="submit" name="add_fee_button" value="">��������</button></td>'."\r\n";
		
//		<form action="index.php">
//		<input type="text" name="user">
//	   <button value="activate" name="button" type="submit">
//   ��������� ������</button>
	echo "\t</form></tr>\r\n";
echo "</table>\r\n\r\n";

echo "<br><br>������ ������\r\n";
echo "<table border=\"1\" width=\"900\">\r\n\t<tr><form action=\"index.php\">\r\n\t";
echo '<td width="15%">��.�<input type="text" id="id_site" name="id_site" size="4" maxlength="4" value="'.$_GET["id_site"].'"></td>'."\r\n\t";
echo '<td width="10%">���.<input type="text" id="letter" name="letter" size="1" maxlength="1" value=""></td>'."\r\n\t";
echo '<td width="25%">'; ListFees($db); echo "</td>\r\n\t";
echo "<td>���������: <textarea>".ListOwners($_GET["id_site"], $db)."</textarea></td>";
echo '<td width="20%">�����<input type="text" id="value" name="value" size="6" maxlength="6"> ���.</td>'."\r\n\t";

echo '<td width="10%"><button type="submit">';
if (isset($_GET["id_fee"])) echo '��������'; else echo '���������';
echo "</button></td>\r\n";

echo "</form></tr>\r\n</table>\r\n";

echo "\r\n</body>\r\n";
echo "</html>";
?>