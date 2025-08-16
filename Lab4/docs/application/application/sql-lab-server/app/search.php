<?php
		echo '<i> lookup page views';
    		$form = '<form><input type="text" name="q" id="q"></p><p><input type="Submit" value="search"></p></form>';
    		echo $form;
    		$q = "";
    		if (isset($_GET['q'])){
        		$q = $_GET['q'];
    		}
    		else{
        		//do nothing    
    		}
    		$con = new SQLite3("app.db");
    		$query = "SELECT views FROM 'pages' WHERE php='$q'";
    		$result = $con->query($query);
    		$row = $result->fetchArray();
    		if ($row){
			      echo "page <b>$q</b>.php has <i>$row[0]</i> views";
    		}
    		else{
        		if (isset($_GET['q'])){
    	    			echo " this php page <b> " . ($_GET['q']) . " </b> does not exist!";
    	 		}
    		}
?> 
