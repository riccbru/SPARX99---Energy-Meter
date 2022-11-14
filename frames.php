<!DOCTYPE html>
<html>

<head>
    <title>Sparx99 - Andreassi</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-iYQeCzEYFbKjA/T2uDLTpkwGzCiq6soy8tYaI1GyVh/UjpbCx/TYkiZhlZB6+fzT" crossorigin="anonymous">

    <style>
    .center {
        margin: auto;
        width: 50%;
        padding: 10px;
    }
    </style>

</head>

<body>
  <br>
    <div class="center">
        <table class="table">
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Data&Ora</th>
                    <th>Active Energy</th>
                    <th>Passive Energy</th>
                </tr>
            </thead>

            <tbody>
                <?php
            
            if (isset($_GET["ind"])) {
                
                $server = "localhost";
                $user = "sparx99";
                $passwd = "wgIJwDaqtS";
                $db = "sparx99_misure-energia";
                
                $connection = new mysqli($server, $user, $passwd, $db);
                
                if ($connection -> connect_error) {
                    die();
                }
                
                $table = "andreassi" . $_GET["ind"];
                //$sql = "INSERT INTO andreassi1 (DateTime, ActiveEnergy, PassiveEnergy) VALUES (NOW(), 0.01, 0.1);";
                $sql = "SELECT * FROM $table ORDER BY ID DESC";
                
                $result = $connection -> query($sql);
                
                if (!$result) {
                    die();
                }

                while ($row = $result -> fetch_assoc()) {
                    echo "<tr>";
 					echo "<td>" . $row["ID"] . "</td>";
                  	echo "<td>" . $row["DateTime"] . "</td>";
                  	echo "<td>" . $row["ActiveEnergy"] . "</td>";
                  	echo "<td>" . $row["PassiveEnergy"] . "</td>";
                  	echo "</tr>";
                }
            }
            ?>
            </tbody>
        </table>
    </div>
</body>


</html>