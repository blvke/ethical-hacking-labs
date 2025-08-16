<?php

if (!copy('app_org.db', 'app.db')) {
    echo "failed to copy $file...\n";
}
$file_pointer = "comments.txt";  
   
// Use unlink() function to delete a file  
if (!unlink($file_pointer)) {
	echo (""); 
}  
echo ("the web app has been reset");  


$handle = fopen($file_pointer, 'w') or die('Cannot open file:  '.$file_pointer); //implicitly creates file
?> 
