<?php
require("common.php");

header("content-type: text/plain");

if(!isset($_GET['protocol'])) Error("no protocol set");
if(!isset($_GET['port'])) Error("no port set");

global $db;

$query = 'REPLACE gameservers (IP, time, protocol) VALUES ("'.$_SERVER['REMOTE_ADDR'].":".$_GET['port'].'", UNIX_TIMESTAMP(), '.$_GET['protocol'].')';
if(!$db->query($query)) Error($db->error . " - " . $query);

Purge();

print("Ok.")

?>