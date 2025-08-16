<?php 
    $pageStart = '<script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.0/jquery.min.js"></script>';


    $pageStart .= '<script type="text/javascript">
        function checkValid(a){
            if( /[^a-zA-Z0-9 ]/.test( (a.value)) ) {
                $("#helpBlock").text("Error, invalid characters detected");
                console.log(false);return false;
            } else {
                console.log(true);return true;
            }
        }</script>';
    session_start();

    
    if(!isset($_SESSION['login'])) {
         echo "<h2>You must be logged in to access the comments</h2>";
	 die();
    }


function read_comments() {

    $comments = file_get_contents('comments.txt');

    if(empty($comments)) {

        echo '<p><i>There are no comments at this time.</i></p>';

    } else {

        echo $comments;

    }

}

/** Function print_form() prints the user submission form, including inline CSS and POST data if a field is left blank. It takes one integer argument that does the following:
    * 0 = print blank form
    * 1 = mark the textarea border red (to indicate invalid information, defined later as null); also prints POST data
    * 2 = mark the text input field border red (to indicate invalid information, defined later as null); also prints POST data
    * 3 = mark both fields red */

function print_form($val) {



    #$sign = hash_hmac('sha256', $_SESSION['login'], $_SESSION['username']);
    $sign = hash('sha256', $_SESSION['username']);

    /* This line is purely stylistic. */
    echo '<p><i>Leave a question/comment:</i></p>';

    echo '<span id="helpBlock" class="help-block" style="color:red;"></span>';

    $form = '<form method="post" onsubmit="return checkValid(comment)" action="#"><textarea name="comment" id="comment" style="';

    /* toggle CSS on textarea based on validation */
    if (($val == 1)||($val == 3)) {

        $form .= 'border: 1px solid #912; ';

    }

    $form .= 'width: 80%; height: 10em;">';

    /* display post data if any part of the form is invalid */
    if ($val != 0) {

        $form .= ($_POST['comment']);

    }

    $form .= '</textarea><p><i>Your name: </i> <b>';

    $form .= '<input type="hidden" name="token256" id="token256" value="' . $sign . '">';
    $form .= $_SESSION['username'] . '</b>';

    $form .= '</p><p><input type="Submit" value="Post comment"></p>';

    $form .= '<span id="helpBlock" class="help-block" style="color:red;"></span></form>';

    $form .= '<form method="post"><p><input type="submit" value="Log me out" name="logout" ></p></form>';
    
    echo $form;


}

/** The role of process_form() is to evaluate whether there is any information to be written; if so, validate it; then call read_comments() and print_form() appropriately */
function process_form() {

    $err = 0;
    /*
    $sign = hash_hmac('sha256', $_SESSION['login'], $_SESSION['username']);
    */
    $sign = hash('sha256', $_SESSION['username']);
    

    if (isset($_POST["logout"])){
        session_destroy();
        unset($_SESSION['username']);
        echo("<script>window.location = 'index.php';</script>");
     /*
     header('LOCATION: /index.php');
     */
    }
   

    if (isset($_POST['comment'])) {

        /* validate the comment box */
        if (empty($_POST['comment'])) {
            echo 'nothing to post!';
            $err++;

        }

        /* validate the name box */
        /*if (empty($_POST['name'])) {

            $err = $err + 2;

	}*/

	/* validate the token256 */
	if ($_POST['token256'] != $sign) {
		$err = $err + 4;
        echo 'token does not match';
	}

        /* if valid, process the form */
        if ($err == 0) {

            /* create full HTML comment string */
            $comment = '<p>“' . ($_POST['comment']) . '”<br><span style="text-align: right; font-size: 0.75em;">—' . ($_SESSION['username']) . ', ' . date('F j\, g\:i A') . '</span></p>';

            /* write file to comments page */
            file_put_contents('comments.txt', $comment, FILE_APPEND | LOCK_EX);

	}
	else {
	    echo "<p>Error Code: " . $err . "</p>";
	   }
    }

    read_comments();
    print_form($err);

}

print $pageStart;

process_form();

    
?>
