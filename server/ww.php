<?php

$host		= 'localhost';		// имя хоста (уточняется у провайдера)
$database	= 'smart_home';		// имя базы данных, которую вы должны создать
$user		= 'root';			// заданное вами имя пользователя, либо определенное провайдером
$pswd		= 'trust#1sql';		// заданный вами пароль
 
 // Подключаемся к базе
$db = mysql_connect($host, $user, $pswd) or die("Не могу соединиться с MySQL.");
mysql_select_db($database) or die("Не могу подключиться к базе.");


 /* CAT:Area Chart */ 

 /* pChart library inclusions */ 
 include("pchart/class/pData.class.php"); 
 include("pchart/class/pDraw.class.php"); 
 include("pchart/class/pImage.class.php"); 

 /* Create and populate the pData object */ 
 $MyData = new pData();   
// $MyData->addPoints(array(14,16,18,12,23,27),"Probe 1"); 
// $MyData->addPoints(array(-3,-12,-15,-8,5,-5),"Probe 2"); 
 $MyData->addPoints(array(0,1,1,0,1,0),"heatData"); 
// $MyData->setSerieTicks("Probe 2",4); 
 
//$request = "SELECT * FROM `temp_probe_data` WHERE (temp_probe_data.id mod 2 = 0) ORDER BY `datetime` DESC LIMIT 10";
//$request = "SELECT * FROM `temp_probe_data` ORDER BY `datetime` ASC LIMIT (SELECT COUNT(datetime) FROM `temp_probe_data`), 10";
$request = "SELECT * FROM `temp_probe_data` ORDER BY `datetime` ASC LIMIT 20, 20";
$result  = mysql_query($request,$db);  
$minT = 125;
$maxT = -125;

while ($row = mysql_fetch_array($result))  
{
	$t = $row["temp"];
	$minT = min($t, $minT);
	$maxT = max($t, $maxT);

	$MyData->addPoints($t, "WarmFloor");
	$MyData->addPoints(substr($row["datetime"], 11, 8), "Labels");
}
 


$MyData->setAxisName(0,"Temperatures");
$MyData->setAxisUnit(0,"В°C");
$MyData->setSerieOnAxis("WarmFloor",0);
//$MyData->setSerieOnAxis("Probe 2",0);


 $MyData->setSerieOnAxis("heatData",1);
 $MyData->setAxisName(1,"Heating");
 $MyData->setAxisPosition(1,AXIS_POSITION_RIGHT);
  
// $MyData->addPoints(array("Jan","Feb","Mar","Apr","May","Jun"),"Labels"); 
//$MyData->setSerieDescription("Labels","Months"); 
$MyData->setAbscissa("Labels");

 /* Create the pChart object */ 
 $myPicture = new pImage(900,430,$MyData); 

 /* Draw the background */ 
 $Settings = array("R"=>170, "G"=>183, "B"=>87, "Dash"=>1, "DashR"=>190, "DashG"=>203, "DashB"=>107); 
 $myPicture->drawFilledRectangle(0,0,900,430,$Settings); 

 /* Overlay with a gradient */ 
 $Settings = array("StartR"=>219, "StartG"=>231, "StartB"=>139, "EndR"=>1, "EndG"=>138, "EndB"=>68, "Alpha"=>50); 
// $myPicture->drawGradientArea(0,0,700,230,DIRECTION_VERTICAL,$Settings); 
// $myPicture->drawGradientArea(0,0,700,20,DIRECTION_VERTICAL,array("StartR"=>0,"StartG"=>0,"StartB"=>0,"EndR"=>50,"EndG"=>50,"EndB"=>50,"Alpha"=>80)); 

 /* Add a border to the picture */ 
// $myPicture->drawRectangle(0,0,699,229,array("R"=>0,"G"=>0,"B"=>0)); 
  
 /* Write the picture title */  
 $myPicture->setFontProperties(array("FontName"=>"pchart/fonts/Silkscreen.ttf","FontSize"=>6)); 
 $myPicture->drawText(10,13,"drawAreaChart() - draw an area chart",array("R"=>255,"G"=>255,"B"=>255)); 

 /* Write the chart title */  
 $myPicture->setFontProperties(array("FontName"=>"pchart/fonts/Forgotte.ttf","FontSize"=>11)); 
 $myPicture->drawText(250,55,"Average temperature",array("FontSize"=>20,"Align"=>TEXT_ALIGN_BOTTOMMIDDLE)); 

 /* Draw the scale and the 1st chart */ 
 $myPicture->setGraphArea(60,60,650,390); 
// $myPicture->drawFilledRectangle(60,60,450,190,array("R"=>255,"G"=>255,"B"=>255,"Surrounding"=>-200,"Alpha"=>10)); 
$ww = array(0=>array("Min"=>$minT, "Max"=>$maxT), 1=>array("Min"=>-2, "Max"=>3), 2=>array("Min"=>0, "Max"=>2));
$myPicture->drawScale(array("DrawSubTicks"=>FALSE,"Mode"=>SCALE_MODE_MANUAL,"ManualScale"=>$ww));
// $myPicture->drawScale(array("DrawSubTicks"=>FALSE));

$MyData->setSerieDrawable("WarmFloor", TRUE);
$MyData->setSerieDrawable("heatData", FALSE);
$myPicture->setFontProperties(array("FontName"=>"pchart/fonts/pf_arma_five.ttf","FontSize"=>6)); 
$myPicture->drawAreaChart(array("DisplayValues"=>TRUE,"DisplayColor"=>DISPLAY_AUTO));

// steps
$MyData->setSerieDrawable("WarmFloor", FALSE);
$MyData->setSerieDrawable("heatData", TRUE);
$myPicture->drawStepChart(array("DisplayValues"=>TRUE,"DisplayColor"=>DISPLAY_AUTO));


 /* Draw the scale and the 2nd chart *
 $myPicture->setGraphArea(500,60,670,190); 
 $myPicture->drawFilledRectangle(500,60,670,190,array("R"=>255,"G"=>255,"B"=>255,"Surrounding"=>-200,"Alpha"=>10)); 
 $myPicture->drawScale(array("Pos"=>SCALE_POS_TOPBOTTOM,"DrawSubTicks"=>TRUE)); 
 $myPicture->drawAreaChart(); 

 /* Write the chart legend */ 
$MyData->setSerieDrawable("WarmFloor", TRUE);
$MyData->setSerieDrawable("heatData", TRUE);
$myPicture->drawLegend(510,205,array("Style"=>LEGEND_NOBORDER,"Mode"=>LEGEND_HORIZONTAL)); 

 /* Render the picture (choose the best way) */ 
 $myPicture->autoOutput("pictures/example.drawAreaChart.png");  
?>
