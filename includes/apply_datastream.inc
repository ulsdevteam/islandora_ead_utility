<?php

/**
 * @file
 * Form for applying the datastream for an individual EAD or MODS record that
 * comes from a EAD Finding Aid.
 */

/**
 * Form building function.
 */
function islandora_ead_utility_apply_datastream_handler($form, &$form_state, $hash) {
  $joined = base64_decode($hash);
  @list($filename, $bib_id, $subject, $fedora_PID) = explode("|", $joined);
  $file_contents = file_exists($filename) ? htmlspecialchars(file_get_contents($filename)) :
      '<i>File did not exist or the permissions were not sufficient to access ' .
      'that file\'s path.</i>';
  // Set a value for display purposes.
  $bib_id = ($bib_id) ? $bib_id : 'unknown identifier';
  $islandora_object = islandora_object_load($fedora_PID);
  $form = array(
    'foo' => array(
      '#type' => 'item',
      '#markup' => theme('islandora_ead_utility_apply_datastream', array(
        'file_contents' => $file_contents,
        'filename' => 'For "' . $bib_id . '", viewing ' . $subject . ' file: ' . $filename . '.',
        'subject' => $subject,
        'has_object' => (is_object($islandora_object)),
        'Fedora_PID' => (is_object($islandora_object) ?
          l($fedora_PID, 'islandora/object/' . $fedora_PID) :
          $fedora_PID),
      ))
    ),
  );
  return $form;
}

// This will display a form that details all of the objects that would be updated
// if they are all applied.  The user will have a chance to Cancel.
function islandora_ead_utility_apply_all_datastream_handler($form, &$form_state, $hash) {
  if ($hash) {
    $applyall_manifest_filename = urldecode(base64_decode($hash));
    $manifest = file_get_contents($applyall_manifest_filename);
    $lines = explode("\r\n", $manifest);

    $temp_path = variable_get('file_temporary_path', '/tmp');

    $last_pid = '';
    $i = 1;
    // Loop through the files and apply the datastream to the referenced object
    foreach ($lines as $line) {
      @list($pid, $filename) = explode("=", $line, 2);
      if ($filename) {
        $filename = str_replace($temp_path . '/', '', trim($filename));
        $files[$pid][] = str_replace($temp_path . '/', '', trim($filename));
        if ($last_pid <> $pid) {
          if (array_key_exists($last_pid, $files)) {
            $form['info_' . $i] = array(
                '#type' => 'item',
                '#markup' => '<b>' . $last_pid . '</b><ul><li>' . implode("</li><li>", $files[$last_pid]) . '</li></ul>',
            );
          }
          $last_pid = $pid;
          $i++;
        }
      }
    }
    $form['info_' . $i] = array(
        '#type' => 'item',
        '#markup' => '<b>' . $last_pid . '</b><ul><li>' . implode("</li><li>", $files[$last_pid]) . '</li></ul>',
    );
  }
  if ($i > 1) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
      '#prefix' => '<b class="imu_red">Are you sure that you want to apply these datastreams to the underlying objects?</b><br>',
    );
  }
  return $form;
}

function islandora_ead_utility_apply_all_datastream_handler_submit($form, &$form_state) {
  drupal_set_message('form submitted');
}