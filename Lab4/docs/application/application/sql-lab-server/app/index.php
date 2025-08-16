<html>
  <head>
      <title>My first App (no 🐛 allowed)</title>
      <link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
  </head>
  <body>
    <div class="container">
        <div class="row">
          <div class="col-md-8 col-md-offset-2">

                <h3>log in</h3>
                <!-- TODO add sign up page -->
                <form action="login.php" method="POST">
                    <fieldset>
                        <div class="form-group">
                            <label for="username">Username:</label>
                            <input type="text" id="username" name="username" class="form-control">
                        </div>
                        <div class="form-group">
                            <label for="password">Password:</label>
                            <div class="controls">
                                <input type="password" id="password" name="password" class="form-control">
                            </div>
                        </div>
                        <div class="form-actions">
                            <input type="submit" value="Login" class="btn btn-primary">
                            <!-- TODO remove debug messages from failed login -->
                        </div>
                    </fieldset>
                </form>
            </div>
        </div>
      </div>
  </body>
</html>