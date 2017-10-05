<?
header('Content-Type: application/xml');
$msg = htmlentities(trim(stripslashes($_REQUEST['msg'])));
$ts = time();
$ip = gethostbyname("hackorama.com"); 
print ("<?xml version=\"1.0\"?>");
print ("<test timestamp=\"$ts\">");
print ("<ip>$ip</ip>");
print ("<message>$msg</message>");
print ("</test>");
?>