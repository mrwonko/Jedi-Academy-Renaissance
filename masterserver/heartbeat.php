<?php
require('common.php');

header('content-type: text/plain');

//validate/sanitize protocol
if(!isset($_GET['protocol'])) Error('No protocol set!');
$protocol = $_GET['protocol'];
if(!is_numeric($protocol))
{
	Error('Protocol must be numeric!');
}
$protocol = intval($protocol);

// validate/sanitize port
if(!isset($_GET['port'])) Error('No port set!');
$port = $_GET['port'];
if(!is_numeric($port))
{
	Error('Port must be numeric!');
}
$port = intval($port);

// validate/sanitize IP
$ip = $_SERVER['REMOTE_ADDR'];
$ipOkay = (preg_match('/[0-9a-f\:\.]+/', $ip, $matches) == 1); //IPv6 use hexadecimal values, colon separated.
if($ipOkay)
{
	$ipOkay = ($ip == $matches[0]);
}
if(!$ipOkay)
{
	Error('Invalid IP!');
}

// build query
$query = 'REPLACE gameservers (IP, time, protocol) VALUES ("'.$ip.':'.$port.'", UNIX_TIMESTAMP(), '.$protocol.')';
global $db;
if(!$db->query($query)) Error($db->error . ' - ' . $query);

Purge();

print('Ok.')

?>
