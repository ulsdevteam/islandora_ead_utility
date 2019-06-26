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
  // Set a value for display purposes.
  $bib_id = ($bib_id) ? $bib_id : 'unknown identifier';
  $islandora_object = islandora_object_load($fedora_PID);
  if (array_key_exists('hash', $_POST)) {
    return array('foo' => array('#type' => 'item',
        '#markup' => theme('islandora_ead_utility_apply_datastream', array(
        'file_contents' => '',
        'filename' => 'For "' . $bib_id . '", viewing ' . $subject . ' file: ' . $filename . '.',
        'has_object' => (is_object($islandora_object)),
        'Fedora_PID' => (is_object($islandora_object) ?
          l($fedora_PID, 'islandora/object/' . $fedora_PID) :
          $fedora_PID),
      ))));
  }
  $file_contents = file_exists($filename) ? htmlspecialchars(file_get_contents($filename)) :
      '<i>File did not exist or the permissions were not sufficient to access ' .
      'that file\'s path.</i>';
  $form = array(
    'apply_btn' => array(
      '#type' => 'submit',
      '#prefix' => '<span class="imu_red">Are you sure that you want to apply this ' .
        $subject . ' datastream to the underlying object?</span> &nbsp;',
      '#value' => t('Apply Datastream'),
      '#attributes' => array(
        'class' => array('imu_red'),
      ),
    ),
    'hash' => array(
      '#type' => 'hidden',
      '#value' => $hash,
    ),
    'foo' => array(
      '#type' => 'item',
      '#markup' => theme('islandora_ead_utility_apply_datastream', array(
        'file_contents' => $file_contents,
        'filename' => 'For "' . $bib_id . '", viewing ' . $subject . ' file: ' . $filename . '.',
        'has_object' => (is_object($islandora_object)),
        'Fedora_PID' => (is_object($islandora_object) ?
          l($fedora_PID, 'islandora/object/' . $fedora_PID) :
          $fedora_PID),
      ))
    ),
  );
  return $form;
}

function islandora_ead_utility_apply_datastream_handler_submit($form, &$form_state) {
  $msg = 'Form submitted';
  if ($form_state['triggering_element']['#value'] == t('Apply Datastream')) {
    $hash = $form_state['values']['hash'];
    $joined = base64_decode($hash);
    @list($filename, $bib_id, $subject, $fedora_PID) = explode("|", $joined);
    $file_contents = file_exists($filename) ? file_get_contents($filename) :
        '<i>File did not exist or the permissions were not sufficient to access ' .
        'that file\'s path.</i>';
    // Set a value for display purposes.
    $bib_id = ($bib_id) ? $bib_id : 'unknown identifier';
    $islandora_object = islandora_object_load($fedora_PID);
    if (is_object($islandora_object)) {
      $pathinfo = pathinfo($filename);
      $parts = explode(".", $pathinfo['filename']);
      $dsid = strtoupper($parts[count($parts) - 1]);
      islandora_ead_utility_add_datastream($islandora_object, $dsid, $file_contents);
      $msg .= ".  [" . $dsid . "] updated for " . $fedora_PID . ".";
    }
  }
  drupal_set_message($msg, 'status');
}

// This will display a form that details all of the objects that would be updated
// if they are all applied.  The user will have a chance to Cancel.
function islandora_ead_utility_apply_all_datastream_handler($form, &$form_state, $hash) {
  if ($hash) {
    $applyall_manifest_filename = urldecode(base64_decode($hash));
    $manifest = file_get_contents($applyall_manifest_filename);
    $lines = explode("\r\n", $manifest);

    $temp_path = variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', '/tmp'));

    $last_pid = '';
    $i = 1;
    // Loop through the files and apply the datastream to the referenced object
    foreach ($lines as $line) {
      @list($pid, $filename) = explode("=", $line, 2);
      if ($filename) {
        $filename = str_replace($temp_path . '/', '', trim($filename));
        $files[$pid][] = trim($filename);
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
  $form['hash'] = array(
    '#type' => 'hidden',
    '#value' => $hash,
  );
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
  $hash = $form_state['values']['hash'];
  if ($hash) {
    $applyall_manifest_filename = urldecode(base64_decode($hash));
    $manifest = file_get_contents($applyall_manifest_filename);
    $lines = explode("\r\n", $manifest);
    foreach ($lines as $line) {
      @list($pid, $filename) = explode("=", $line, 2);
      if ($filename) {
        $islandora_object = islandora_object_load($pid);
        if (is_object($islandora_object)) {
          // Parse the filename to get the datastream id (DSID) value.
          $pathinfo = pathinfo($filename);
          $parts = explode(".", $pathinfo['filename']);
          $dsid = strtoupper($parts[count($parts) - 1]);
          $file_contents = file_get_contents($filename);
          islandora_ead_utility_add_datastream($islandora_object, $dsid, $file_contents);
          drupal_set_message("[" . $dsid . "] updated for " . $pid . ".", 'status');
        }
        else {
          drupal_set_message(t('The Islandora object "' . $pid . '" could not be loaded'), 'warning');
        }
      }
    }
    drupal_set_message('Form submitted.');
  }
  else {
    drupal_set_message('Nothing was submitted.');
  }
}
/**
 * This will ingest a new datastream made from $file_contents to the supplied
 * $object and use the datastream identifier = $datastream_id.  This will
 * either update the existing datastream if it exists on the object, or it will
 * create a new datastream.
 *
 * @param stdObject $object
 * @param string $datastream_id
 *   The datastream identifier.
 * @param string $file_contents
 *   The contents to store in the new datastream.
 * @return array
 *   An array with success and message info.
 */
function islandora_ead_utility_add_datastream($object, $datastream_id, $file_contents) {
  try {
    $ingest = !isset($object[$datastream_id]);
    if ($ingest) {
      $ds = $object->constructDatastream($datastream_id, "M");
      $ds->label = $datastream_id;
    }
    else {
      $ds = $object[$datastream_id];
    }
    if ($ds->mimetype != 'application/xml') {
      $ds->mimetype = 'application/xml';
    }
    $ds->setContentFromString($file_contents);
    if ($ingest) {
      $object->ingestDatastream($ds);
    }
    return array(
      'success' => TRUE,
      'messages' => array(
        array(
          'message' => t('Created @dsid derivative for OBJ (@pid).'),
          'message_sub' => array(
            '@dsid' => $datastream_id,
            '@pid' => $object->id,
          ),
          'type' => 'dsm',
        ),
      ),
    );
  }
  catch (exception $e) {
    return array(
      'success' => FALSE,
      'messages' => array(
        array(
          'message' => t('Failed to add @dsid datastream for @pid. Error message: "@message" with trace: @stack'),
          'message_sub' => array(
            '@dsid' => $datastream_id,
            '@pid' => $object->id,
            '@message' => $e->getMessage(),
            '@stack' => $e->getTraceAsString(),
          ),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        ),
      ),
    );
  }
}
