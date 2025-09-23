<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/player_management.php";
    
    session_start();
    if(isset($_SESSION['username'])) {
        header("Location: /php/pagine/main_menu.php");
        exit();
    }
    $mess = "";
    if($_SERVER["REQUEST_METHOD"]==="POST"){
        $mode = $_POST["mode"];
        $username = $_POST["username"];
        $password = $_POST["password"];
        $r_password="";
        if($mode === "Signup"){
            $r_password = $_POST["r_password"];
            $mess = checkInput($username,$password,$r_password);
        }
        if($mess === ""){
            $user_info=getInfo($username,"player");
            if($user_info!==false){
                if($mode === "Signup"){
                    if($user_info!==null){ // ho un record
                        $mess = "Username gia presente";
                    }else{
                        $connection=db_connect();
                        $hash = password_hash($password,PASSWORD_BCRYPT);
                        $query = mysqli_prepare($connection, "INSERT INTO utenti (username,password_hash)
                        VALUES (?,?)"); 
                        mysqli_stmt_bind_param($query,"ss",$username,$hash); 
                        if(!mysqli_stmt_execute($query))
                            $mess = "esecuzione fallita";
                        else{
                            session_regenerate_id(true);
                            $_SESSION["username"] = $username;
                            $_SESSION["soldi"] = 50000; //valore di default
                            header("Location: /php/pagine/main_menu.php"); //se ho successo, reindirizzo
                        }
                        mysqli_stmt_close($query);
                        mysqli_close($connection);
                    }
                }else if($mode === "Login"){
                    if($user_info===null){ // non ho un record
                        $mess = "Username non esiste";
                    }else{
                        $saved_password = $user_info["password_hash"];
                        if(!password_verify($password,$saved_password)){
                            $mess = "Le credenziali inserite sono errate";
                        }else{
                            session_regenerate_id(true);
                            $_SESSION["username"] = $username;
                            $_SESSION["soldi"] = $user_info["soldi"];
                            header("Location: /php/pagine/main_menu.php"); //se ho successo, reindirizzo
                        }
                    }
                }else
                    $mess = "Richiesta errata";
            }else
                $mess = "ERRORE QUERY";
        }
    }

    function checkInput($username,$password,$r_password){
        $result ="";
        $UserCrit = "/^[A-Za-z0-9]{4,18}$/";
        $PasswCrit = "/^(?=.*\d)(?=.*[^a-zA-Z0-9\s]).{8,20}$/";
        if(!preg_match($UserCrit,$username))
            $result= $result."L'username inserito non va bene<br>";
        if(!preg_match($PasswCrit,$password))
            $result= $result."La password inserita non va bene <br>";
        if($password !== $r_password)
            $result= $result. "Le due password non corrispondono";

        return $result;
    }
?>
<!DOCTYPE html>
<html lang = "it">
    <head>
        <title>Login</title>
        <meta charset = "utf-8">
        <meta name = "author" content = "Sebastiano Pala">
        <link rel="stylesheet" href="../../CSS/Layout.css">
        <link rel="stylesheet" href="../../CSS/Login.css">
        <script>
            window.appData = {
                initialMode: "<?= htmlspecialchars($_POST['mode'] ?? 'Login') ?>",
            };
        </script>
        <script src="/script/login.js"></script>
        <script src="/script/layout.js"></script>
    </head>
    <body>
        <div class="page_style container">
            <div id="title">
                <img  alt="clubs">
                <img  alt="hearts">
                <h1 style="color: white;">POKER NIGHT</h1>
                <img  alt="diamonds">
                <img  alt="spades">
            </div>
            <form id="form" method="POST">
                <div id="user_container" class="page_style upr_container">
                    <div class="input_container">
                        <label for="username">Username:</label>
                        <input type="text" id="username" name="username" required>
                    </div>
                    <span class="hide" hidden>Requisiti: 4-18 caratteri, solo lettere e numeri</span>
                </div>
                <div id="passw_container" class="page_style upr_container">
                    <div class="input_container">
                        <label for="password">Password:</label>
                        <input type="password" id="password" name="password" required>
                    </div>
                    <span class="hide" hidden>Requisiti: 8-20 caratteri, almeno un numero<br>e carattere speciale</span>
                </div>
                <div id="r_passwd_container" class="page_style upr_container hide" hidden>
                    <div id="r_pwd" class="input_container">
                        <label for="r_password">Conferma Password:</label>
                        <input type="password" id="r_password" name="r_password" class="hide" hidden>
                    </div>
                </div>
                <input id="submit" name="mode" type="submit" value="Login" class="menu_button darken">
                <?php if($mess !== ""):?>
                    <p id = "mess" class='error_message'> <?php echo $mess ?></p>
                <?php endif; ?>
            </form>
            <div id="change_container">
                <p id="change_text" style="color: white;">Non hai un account?</p>
                <button id="change" class="menu_button">Crealo qui</button>
            </div>
            <nav id="menu">
                <a href="/php/pagine/home.php" class="page_style">
                    <img  alt="clubs">
                    Torna alla home
                    <img  alt="diamonds">
                </a>
            </nav>
        </div>
    </body>
</html>