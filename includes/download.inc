<?php

/**
 * @file
 * Form for downloading an individual EAD or MODS xml file that comes from a
 * EAD Finding Aid.
 */

/**
 * Form building function.
 */
function islandora_ead_utility_download($form, &$form_state, $hash) {
  $joined = base64_decode($hash);
  @list($filename, $bib_id, $subject, $fedora_PID) = explode("|", $joined);

  if (file_exists($filename) && $bib_id && $subject) {
    // Set a value for display purposes.
    $bib_id = ($bib_id) ? $bib_id : 'unknown identifier';

    $download_filename = $bib_id . '_' . $subject . '.xml';

    header('Content-Type: text/xml');
    header('Content-disposition: attachment; filename='.$download_filename);
    header('Content-Transfer-Encoding: Binary');
    header('Content-Length: ' . filesize($filename));
    readfile($filename);
    exit;
  }
  else {
    drupal_set_message(t('The underlying file could not be found (or the ' .
        'permissions were not sufficient to access) in order to serve it up ' .
        'for download.'));
  }
}

function islandora_ead_utility_download_all($form, &$form_state, $hash) {
  if ($hash) {
    $records_manifest_filename = urldecode(base64_decode($hash));
    $manifest = file_get_contents($records_manifest_filename);
    $files = explode("\r\n", $manifest);

    $initial_dir = getcwd();
    $temp_path = variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', '/tmp'));
    chdir($temp_path);
    $temp_zip_filename = tempnam($temp_path, 'eadmods_zip_');

    $zipfile = new ZipArchive();
    $zipfile->open($temp_zip_filename, ZIPARCHIVE::CREATE);
    foreach ($files as $filename) {
      if ($filename) {
        $filename = str_replace($temp_path . '/', '', trim($filename));
        $zipfile->addFile(trim($filename));
      }
    }
    $zipfile->close();
    chdir($initial_dir);

    if (file_exists($temp_zip_filename)) {
      // Set a value for display purposes.
      $download_filename = date('Ymd') . '_EAD_MODS.zip';

      header('Content-Type: application/octet-stream');
      header('Content-disposition: attachment; filename='.$download_filename);
      header('Content-Transfer-Encoding: Binary');
      header('Content-Length: ' . filesize($temp_zip_filename));
      readfile($temp_zip_filename);
      exit;
    }
    else {
      drupal_set_message(t('The underlying files could not be found to make a zip file to download.'));
    }
  }
}
