<?php
    ini_set('error_reporting', E_ALL);
    session_start();
    $con = new SQLite3("app.db");
    $username = $_POST["username"];
    $password = $_POST["password"];
    $query = "SELECT password FROM users WHERE name='$username' AND password='$password'";
    $result = $con->query($query);
    $succsess = false;

    while ($row = $result->fetchArray()) {
        if ($row['password'] == $password) {
            $succsess = true;
            $username = $_POST["username"];
        } else {
            echo '<script>console.log('. json_encode( $row ) .');</script>';
        }
    }
    if ($succsess) {
        $_SESSION['login'] = true;
        $_SESSION['username'] = $username;
        header('LOCATION: /comments.php');
    } else {
        echo "<h1>Login failed.</h1>";
        echo '<script>console.log('. json_encode( ($row) ) .');</script>';
    }
?> 