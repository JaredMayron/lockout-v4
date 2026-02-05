<?php

$groups = explode(',', $_GET['groups']);

$url = $_ENV['CIVICRM_WEBSITE'] . $_ENV['CIVICRM_APIv4URI'] . '/Contact/get';
$params = [
  'select' => ['id', 'display_name', 'Card_ID.new_card_id', 'row_count'],
  'where' => [['groups', 'IN', $groups], ['Card_ID.new_card_id', 'IS NOT EMPTY']],
//  'limit' => 15,
//  'offset' => ($_REQUEST['page'] - 1) * 15,
];

$request = stream_context_create([
  'http' => [
    'method' => 'POST',
    'header' => [
      'Content-Type: application/x-www-form-urlencoded',
      'X-Civi-Auth: Bearer ' . $_ENV['CIVICRM_AUTH'],
      'X-Civi-Key: ' . $_ENV['CIVICRM_KEY'], //712f2480718923f6320ee4d0094820d5'
    ],
    'content' => http_build_query(['params' => json_encode($params)]),
  ]
]);
//$data = file_get_contents($url, FALSE, $request);
//$data = str_replace('"display_name"', '"name"', $data);
//$data = str_replace('"Card_ID.new_card_id"', '"card"', $data);

$contacts = json_decode(file_get_contents($url, FALSE, $request), TRUE);

print_r($contacts, true);

$data = "";
foreach($contacts['values'] as $c){
  $data .= $c['id'] . '|' . $c['display_name'] . '|' . $c['Card_ID.new_card_id'] . "\n";
}

print($data);
