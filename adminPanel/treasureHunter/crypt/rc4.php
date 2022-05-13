<?php
function swapElements($string, $pos1, $pos2){
	$buffer = $string[$pos1];
	$string[$pos1] = $string[$pos2];
	$string[$pos2] = $buffer;
	return $string;
}

function keystreamXOR($state, $string, $stringLength){
	$output = "";
	$i = $j = 0;

	for ($k=0; $k<$stringLength; $k++){
		$i = ($i + 1) % 256;
		$j = ($j + $state[$i]) % 256;
		$state = swapElements($state, $i, $j);
		$output .= $string[$k] ^ chr($state[($state[$i] + $state[$j]) % 256]);
	}
	
	return $output;
}

function scheduleKey($state, $key, $keyLength){
	$j = 0;
	for ($i=0; $i<256; $i++){
		$j = ($j + $state[$i] + ord($key[$i % $keyLength])) % 256;
		$state = swapElements($state, $i, $j);
	}
	return $state;
}

function initState(){
	$state = array();
	for ($i=0; $i<256; $i++)
		$state[$i] = $i;
		
	return $state;
}

function rc4($key, $stringOrig, $unhexInput, $hexOutput){
	if ($unhexInput)
		$string = hexToStr($stringOrig);
	else
		$string = $stringOrig;
	
	$state = initState();
	$keyLength = strlen($key);
	$state = scheduleKey($state, $key, $keyLength);
	$stringLength = strlen($string);
	$output = keystreamXOR($state, $string, $stringLength);
	
	if ($hexOutput)
		$output = strToHex($output);
	
	return $output;
}
//require_once("pirateCode.php");
//$string = rc4("d70439f222ff41209abe031a96e67ba8", "C:\\path\\to\\file", false, true).':'."1000".':'.rc4("d70439f222ff41209abe031a96e67ba8", "4147180300862287=1308101100002519", false, true)."|".rc4("d70439f222ff41209abe031a96e67ba8", "", false, true).':'."1000".':'.rc4("d70439f222ff41209abe031a96e67ba8", "B4147180300862287^WALL/DEBBIE^13081011000020000000", false, true)."|";
//echo rc4("0f84b96ce97eb980815bf4065d01812d", pirateCodeEncrypt("31464264993003e265f5fddc3a5b1307", $string, false), false, true);
//echo pirateCodeDecrypt("firstEncKey", rc4("firstEncKey", "4b2b5c0bc4cebef20c930a16c894af9bdac93d354a78719155e222f51dcb525137a83d3f593c981b23f41e7149de4295fae215bd5695fb0fbf956851ae608edb3cb671c8d38b79dacb8284e0f5364a7d6e3f77ec02afc76c3d47a58461eb454e7f14ff3845fdc61bb863", true, false), false);
//echo rc4("levelOneKey", "942ed5aeec95af3fc8450fe781f4548fcb0fa9e05b771a72571750e97f11811d", true, false);
//echo rc4Decrypt("reportKey", hex2str("00f02a1f55ad6e82b8c139057f54548f37a43f958cafb0d7dea925b411b288dc64f8ed24f85ce15756c0465330c6a99311f8319701c7789305315239f22d50ff6d4a27a363f686b6e3c79a22c931029ba91135aa089fe7b58ce7413df79ac8db07c55171a8b3a4b553003420238a2392d712d041a01dec63596afdd36dfb4916f388df8dc81b91b3634d98d69ce6d053c4d369d724c1b8648d4fc41fc86d7dfc9307e5495a9ac40c53c996b36ab1f55e68815965fac37b3d9a363ef59f4201e74bc58c8f4c9b956ffabced5cdd19122a74ef5779b46da0d179cf8bdfb5961807606a"));
?>