<?php

/**
 * @file
 * Form for viewing an individual EAD or MODS that comes out of a EAD Finding Aid.
 */

/**
 * Form building function.
 */
function islandora_ead_utility_file_viewer_display($form, &$form_state, $hash) {
  $joined = base64_decode($hash);
  @list($filename, $identifier, $subject, $fedora_PID) = explode("|", $joined);
  $file_contents = file_exists($filename) ? htmlspecialchars(file_get_contents($filename)) :
      '<i>File did not exist or the permissions were not sufficient to access ' .
      'that file\'s path.</i>';
  // Set a value for display purposes.
  $identifier = ($identifier) ? $identifier : 'unknown identifier';
  $form = array(
    'foo' => array(
      '#type' => 'item',
      '#markup' => theme('islandora_ead_utility_file_viewer', array(
        'hash' => $hash,
        'file_contents' => $file_contents,
        'filename' => 'For "' . $identifier . '", viewing ' . $subject . ' XML ' . $filename . ':',
        'subject' => $subject,
      ))
    ),
  );
  return $form;
}
