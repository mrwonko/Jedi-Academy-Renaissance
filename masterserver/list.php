<?php
require("common.php");

header("content-type: text/plain");

if(!isset($_GET['protocol'])) Error("no protocol set");

global $db;

Purge();

$query = "SELECT ip FROM gameservers WHERE protocol = " . $_GET['protocol'];
$result = $db->query($query);
if(!$result) Error($db->error . " - " . $query);

while($row = $result->fetch_assoc())
{
	print($row["ip"]."\n");
}
$result->free();

?>