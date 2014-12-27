#!/usr/bin/php5
<?php
# Time-stamp: "2008-06-03 22:27:56 tuxostat"
# This is just a simple script that logs environental data from OWFS sensors to MySQL


# CONFIGURATION
$mysql_server = "192.168.2.23";
$mysql_user = "tuxostat_log";
$mysql_dbName = "tuxostat";
$log_tableName = "env_log";

$sensors = array("10.34E30F010800" => "JA-BR", "10.96F70F010800" => "LR", "10.6DD40F010800" => "JA-BR-Vent");

mysql_connect($mysql_server, $mysql_user) or die("Unable to connect to MySQL Server. Error: ".mysql_error());
mysql_select_db($mysql_dbName);

foreach ($sensors as $id => $name)
{
  $theData = trim(exec("cat /1wire/".$id."/temperature"));
  $celcius = floatval($theData);
  $farenheit = ((9/5)*$celcius)+32;
  $query = 'INSERT INTO '.$log_tableName.' SET timestamp='.time().',sensorID="'.$id.'",valC='.$celcius.',sensorName="'.$name.'",valF='.$farenheit.';';
  echo $query."\n";
  mysql_query($query) or die("Unable to execute query ".$query.".<br /> Error: ".mysql_error());
}

mysql_close();

?>