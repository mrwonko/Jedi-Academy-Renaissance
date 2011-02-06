<?php
$db_name = "";
$db_url = "localhost";
$db_user = "";
$db_password = "";

$purgetime = 90;

function Error($message)
{
	die("Error: " . $message);
}

$db = new mysqli($db_url, $db_user, $db_password, $db_name);
if(mysqli_connect_errno()) Error(mysqli_connect_error());

function Purge()
{
	global $db, $purgetime;
	$query = 'DELETE FROM gameservers WHERE time < UNIX_TIMESTAMP() - '.$purgetime;
	if(!$db->query($query)) Error($db->error . " - " . $query);
}
?>