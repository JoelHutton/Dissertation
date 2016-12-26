<?php 

$to = $_GET["to"]; //The receiving mobile number 
$from = $_GET["from"]; //The sending mobile number 
$message = urldecode($_GET["message"]); //SMS content 


// You may wish to log this information in a database 
//
// // Lets send an email with the message data 
// $email_message = "Inbound SMS sent to $to.\nSent From: $from\nMessage: $message"; 
// mail("email@example.com", "Inbound SMS", $message, "From: email@example.com"); 
//

 ?>
