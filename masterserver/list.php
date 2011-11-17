<?php
require("common.php");

// remove old entries
Purge();

// set type
header("content-type: text/plain");

// sanitize protocol
if(!isset($_GET['protocol'])) Error('No protocol set!');
$protocol = $_GET['protocol'];
if(!is_numeric($protocol))
{
	Error('Protocol not numeric!');
}
$protocol = intval($protocol);

// build query
$query = 'SELECT ip FROM gameservers WHERE protocol = ' . $protocol;
global $db;
$result = $db->query($query);
if(!$result) Error($db->error . ' - ' . $query);

while($row = $result->fetch_assoc())
{
	print($row["ip"]."\n");
}
$result->free();

?>
