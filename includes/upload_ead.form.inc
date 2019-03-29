<?php

/**
 * @file
 * Form for uploading and processing a EAD.
 */

/**
 * Form building function.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_ead_utility_form($form, &$form_state) {
  if (!isset($form_state['stage'])) {
    $form_state['stage'] = 'step1';
  }
  $form = array();
  switch ($form_state['stage']) {
    case 'step1':
      return islandora_ead_utility_form_step1($form, $form_state);
     break;

    case 'step2':
      return islandora_ead_utility_form_step2($form, $form_state);
     break;

    case 'complete':
      return islandora_ead_utility_form_step2($form, $form_state, TRUE);
     break;
  }
  return $form;
}

/**
 * Prompt the user for the EAD file upload and option to override the settings.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_ead_utility_form_step1($form, &$form_state) {
  $values = isset($form_state['multistep_values']['rate_the_room']) ? $form_state['multistep_values']['rate_the_room'] : array();
  $form = islandora_ead_utility_common_upload_form_elements();

  $form['override_settings_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Override settings'),
    'override' => array(
      '#type' => 'checkbox',
      '#title' => t('Override'),
      '#default_value' => 0,
      '#attributes' => array(
        'onchange' => 'set_override_elements(this)',
      ),
    ),
    'mods_solr_fieldname' => array(
      '#title' => t('MODS identifier Solr fieldname.'),
      '#type' => 'textfield',
      '#attributes' => array(
        'placeholder' => MODS_SOLR_FIELDNAME_PLACEHOLDER,
        'readonly' => 'readonly',
        'disabled' => 'disabled',
        'class' => array('disabled_text'),
      ),
      '#description' => t('Solr fieldname for the MODS identifier that corresponds ' .
        'to the ead identifier (above).'),
      '#default_value' => variable_get('islandora_ead_utility_mods_solr_fieldname', ''),
    ),
    'namespace' => array(
      '#title' => t('Fedora namespace'),
      '#type' => 'textfield',
      '#attributes' => array(
        'placeholder' => 'islandora',
        'readonly' => 'readonly',
        'disabled' => 'disabled',
        'class' => array('disabled_text'),
        ),
      '#description' => t('Namespace for objects in the datastore.  For an object ' .
        '"<b>islandora</b>:123", the namespace is "islandora".  ') .
        (module_exists('islandora_datastreams_io') ?  t('"Islandora Datastreams ' .
            'Input/Output Utility" is installed and')
          : t('The module "') . l('Datastreams Input/Output Utility',
              'https://github.com/ulsdevteam/islandora_datastreams_io') . '"') .
        t(' is capable of importing the ZIP files that are created by this utility\'s ' .
            '"Download all" function.')
        ,
      '#default_value' => variable_get('islandora_ead_utility_namespace'),
      ),
  );

  // Display the upload form.
  $form['ead_upload'] = array(
    '#type' => 'file',
    '#title' => t('EAD Finding Aid'),
    '#description' => t('An EAD Finding Aid containing the metadata for a number of objects.'),
    '#upload_location' => variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', file_directory_temp())),
    '#upload_validators' => array(
      'file_validate_extensions' => array(''),
    ),
  );
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );
  return $form;
}

/**
 * Helper function to return the array elements that would be common to all
 * steps of processing the EAD Finding Aid.
 *
 * @return array
 *   Elements for a Drupal form definition.
 *
 */
function islandora_ead_utility_common_upload_form_elements() {
  $form = array();
  if (user_access(ISLANDORA_EAD_UTILITY_MANAGE)) {
    $form['admin'] = array(
        '#type' => 'item',
        '#markup' => l('Islandora EAD Utility - Settings',
              'admin/islandora/islandora_ead_utility',
              array('attributes' => array('target' => '_blank'))),
    );
  }
  return $form;
}

/**
 * The form to display the second / final step of parsing the EAD Finding Aid.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param type $is_completed
 *   Whether or not the parsing of the underlying EAD has been completed or if
 * it still needs to be parsed.
 * @return array
 *   The Drupal form definition.
 */
function islandora_ead_utility_form_step2($form, &$form_state, $is_completed = FALSE) {
  $form = islandora_ead_utility_common_upload_form_elements();
  // Display the Processing form.
  $form['name'] = array(
    '#type' => 'item',
    '#title' => t('Name'),
    '#description' => $_FILES['files']['name']['ead_upload'],
  );
  $form['type'] = array(
    '#type' => 'item',
    '#title' => t('Type'),
    '#description' => $_FILES['files']['type']['ead_upload'],
  );
  $form['tmp_name'] = array(
    '#type' => 'item',
    '#title' => t('Temporary file name'),
    '#description' => $_FILES['files']['tmp_name']['ead_upload'],
  );
  $form['size'] = array(
    '#type' => 'item',
    '#title' => t('Size (bytes)'),
    '#description' => number_format($_FILES['files']['size']['ead_upload']),
  );

  // call the function to perform the parsing of the EAD Finding Aid file based on the file type.
  if ($_FILES['files']['type']['ead_upload'] <> 'text/xml') {
    drupal_set_message(t('The uploaded file is not supported (mime type "<b><i>@uploaded_file_mimetype</i></b>").  The only supported ' .
        'file mime type is "<b>text/xml</b>".',
          array(
              '@uploaded_file_mimetype' => $_FILES['files']['type']['ead_upload'],
          )), 'error');
  }

  // If the user selected to override the configured MODS field, pass the values
  // in to whichever parsing function is needed for the uploaded file.
  $mods_identifier_solr_fieldname = variable_get('islandora_ead_utility_mods_solr_fieldname', EAD_UTIL_MODS_SOLR_FIELDNAME_PLACEHOLDER);
  $namespace = variable_get('islandora_ead_utility_namespace', 'islandora');
  if (isset($_POST['override']) && $_POST['override']) {
    if (isset($_POST['mods_solr_fieldname']) && $_POST['mods_solr_fieldname']) {
      $mods_identifier_solr_fieldname = $_POST['mods_solr_fieldname'];
    }
    if (isset($_POST['namespace']) && $_POST['namespace']) {
      $namespace = $_POST['namespace'];
    }
  }

  // This code will perform everything in the temp directory.
  $ead_and_mods_records = ((($_FILES['files']['type']['ead_upload'] == 'text/xml')) ?
      islandora_ead_utility_eadxml_split($_FILES['files']['tmp_name']['ead_upload'], $mods_identifier_solr_fieldname, $namespace) :
      array());
  islandora_ead_utility_add_operation_links_and_title($ead_and_mods_records);
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#suffix' => ' ',
  );
  $objects_found_by_match = 0;
  if (!$is_completed && ($_FILES['files']['type']['ead_upload'] == 'text/xml')) {
    $header = array(
        'bib_id',
        'Fedora PID',
        'EAD',
        array('data' => 'MODS', 'title' => 'Transformed EAD into MODS v3.5 using MARC21slim2MODS3-5.xsl'),
        array('data' => 'EAD size', 'class' => array('imu_numeric_header')),
        array('data' => 'MODS size', 'class' => array('imu_numeric_header')),
    );
    $temp_path = drupal_realpath(variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', file_directory_temp())));
    $applyall_files = $download_files = array();
    foreach ($ead_and_mods_records as $idx => $ead_and_mods_arr) {
      if (array_key_exists('MODS', $ead_and_mods_arr) && array_key_exists('filepath', $ead_and_mods_arr['MODS'])) {
        $download_files[] = $ead_and_mods_arr['MODS']['filepath'] . '
';
        if ($ead_and_mods_arr['Fedora_PID']) {
          $applyall_files[] = $ead_and_mods_arr['Raw_Fedora_PID'] . '=' . $temp_path . '/' . $ead_and_mods_arr['MODS']['filepath'] . '
';
        }
      }
      if ($ead_and_mods_arr['Fedora_PID']) {
        $objects_found_by_match++;
      }
      unset($ead_and_mods_records[$idx]['Raw_Fedora_PID']);
    }

    $temp_records_manifest_filename = tempnam(variable_get('file_temporary_path', file_directory_temp()), 'ead_utility_downloadall_manifest_');
    file_put_contents($temp_records_manifest_filename, $download_files);
    $temp_applyall_manifest_filename = tempnam(variable_get('file_temporary_path', file_directory_temp()), 'ead_utility_applyall_manifest_');
    file_put_contents($temp_applyall_manifest_filename, $applyall_files);
    $form['download_all_link'] = array(
      '#type' => 'item',
      '#markup' => l(t('Download All'), 'admin/islandora/tools/islandora_ead_utility/downloadall/' . base64_encode(urlencode($temp_records_manifest_filename)), array('attributes' => array(
          'class' => array('m_link_button'),
          'target' => '_blank'))),
      '#suffix' => ' ',
    );
    $table_markup = theme('table', array(
        'header' => $header,
        'rows' => $ead_and_mods_records));
    if (user_access(ISLANDORA_EAD_UTILITY_CAN_APPLY_DATASTREAM)) {
      $form['apply_all'] = array(
        '#type' => 'item',
        '#markup' => (($objects_found_by_match > 0) ?
              l(t('Apply all datastreams'),
                  'admin/islandora/tools/islandora_ead_utility/applyall/' . base64_encode(urlencode($temp_applyall_manifest_filename)), array('attributes' => array(
              'class' => array('m_link_button'),
            'target' => '_blank'))) 
            : '<span title="There were no matching objects found for the records below" class="m_link_button b_disabled_text">Apply all datastreams</span>'),
        '#suffix' => $table_markup,
      );
    }
    else {
      $form['download_all_link']['#suffix'] = $table_markup;
    }
  }

  return $form;
}

/**
 * To put together a return array that is structured differently for a set of
 * table rows. This will change that variable by reference.
 *
 * @param array $ead_and_mods_records
 *   The raw data about the EAD xml and MODS xml that were generated by parsing
 * the EAD Finding Aid.
 */
function islandora_ead_utility_add_operation_links_and_title(&$ead_and_mods_records) {
  foreach ($ead_and_mods_records as $index => $arr) {
    $ead_and_mods_records[$index]['MODS'] = islandora_ead_utility_get_operation_links('MODS', $arr);
    $ead_and_mods_records[$index]['Raw_Fedora_PID'] = $ead_and_mods_records[$index]['Fedora_PID'];
    $ead_and_mods_records[$index]['Fedora_PID'] =  ($ead_and_mods_records[$index]['Fedora_PID']) ? l($ead_and_mods_records[$index]['Fedora_PID'],
            'islandora/object/' . $ead_and_mods_records[$index]['Fedora_PID'] . '/manage',
      array(
          'attributes' => array(
              'target' => '_blank'
      ))) : '';
    $ead_and_mods_records[$index]['MODS size'] = array(
        'data' => number_format($ead_and_mods_records[$index]['MODS size']),
          'class' => array('imu_numeric'),
        );
  }
}

/**
 * Helper function to make a set of links related to the EAD or MODS files
 * that came out of parsing the EAD Finding Aid.  These files use base64 to
 * encode the filename -- and it passes this to be handled by the route:
 *   admin/islandora/tools/islandora_ead_utility/viewer/{b64_value}
 *   admin/islandora/tools/islandora_ead_utility/download/{b64_value}
 *   admin/islandora/tools/islandora_ead_utility/apply_datastream/{b64_value}
 *
 * @param string $subject
 *   'EAD' or 'MODS'
 * @param type $arr
 * @return type
 */
function islandora_ead_utility_get_operation_links($subject, $arr) {
  // Based on user permissions, make links to View, Download, Apply Datastream.
  $links_arr = array();

  $b64_filename_bib_id_subj = base64_encode($arr[$subject] . '|' .$arr['bib_id'] . '|' . $subject . '|' . $arr['Fedora_PID']);
  $file_contents = file_get_contents($arr[$subject]);
  $file_truncated_ellipsis = (strlen($file_contents) > 600) ? ' ... ' : '';

  // Download single file
  $links_arr[] = l('View', 'admin/islandora/tools/islandora_ead_utility/viewer/' . $b64_filename_bib_id_subj,
      array(
          'attributes' => array(
              'target' => '_blank'
      )));
  $links_arr[] = l('Download', 'admin/islandora/tools/islandora_ead_utility/download/' . $b64_filename_bib_id_subj,
      array(
          'attributes' => array(
              'target' => '_blank',
              'title' => 'Download ' . $subject . ' record'
      )));
  if (user_access(ISLANDORA_EAD_UTILITY_CAN_APPLY_DATASTREAM) && isset($arr['Fedora_PID']) && $arr['Fedora_PID']) {
    $links_arr[] = l('Apply', 'admin/islandora/tools/islandora_ead_utility/apply_datastream/' . $b64_filename_bib_id_subj,
        array(
            'attributes' => array(
                'target' => '_blank',
                'title' => 'Apply ' . $subject . ' record as object datastream'
        )));
  }

  return array(
      'filepath' => $arr[$subject],
      'data' => implode(" | ", $links_arr),
      'title' => substr($file_contents, 0, 600) . $file_truncated_ellipsis);
}

/**
 * Validates the form input.
 *
 * Checks the namespace value is legal.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function.
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_ead_utility_form_validate(&$elements, &$form_state, $form_id = NULL) {
  // Check that the input for namespace satisfies this:
  $namespace = $form_state['values']['namespace'];
  if ($namespace <> '' && !preg_match("/^([A-Za-z0-9]{2,50}|-|\.)$/iD", $namespace)) {
    form_set_error('namespace', t('<b>Namespace value must only include alphanumeric characters.</b>',
        array('@namespace' => $namespace)));
  }
}

/**
 * Form submit handler.
 *
 * May kick off a batch process.
 */
function islandora_ead_utility_form_submit($form, &$form_state) {
  switch ($form_state['stage']) {
    case 'step2':
      $form_state['multistep_values'][$form_state['stage']] = $form_state['values'];
      if ($form_state['triggering_element']['#value'] == t('Download All')) {
        $form_state['complete'] = TRUE;
        $files = explode("\t", $form_state['values']['download_files']);
        drupal_set_message(t('"Download All" should be done.'), 'info');
        return;
      } elseif ($form_state['triggering_element']['#value'] != 'Cancel') {
        $form_state['complete'] = TRUE;
      }
     break;

    default:
      $form_state['multistep_values'][$form_state['stage']] = $form_state['values'];
     break;
  }

  if ($form_state['triggering_element']['#value'] == 'Cancel') {
    $form_state['new_stage'] = islandora_ead_utility_move_to_previous_stage($form, $form_state);
  }
  else {
    $form_state['new_stage'] = islandora_ead_utility_move_to_next_stage($form, $form_state);
  }

  if (isset($form_state['multistep_values']['form_build_id'])) {
    $form_state['values']['form_build_id'] = $form_state['multistep_values']['form_build_id'];
  }
  $form_state['multistep_values']['form_build_id'] = $form_state['values']['form_build_id'];

  $form_state['stage'] = $form_state['new_stage'];
  $form_state['rebuild'] = ($form_state['triggering_element']['#value'] == t('Download All')) ? FALSE : TRUE;
}

/**
 * Helper for setting the next stage of processing on this multi-step form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   The name of the stage to be used in determining which form to display.
 */
function islandora_ead_utility_move_to_next_stage($form, &$form_state) {
  switch ($form_state['stage']) {
    case 'step1':
      return 'step2';
     break;

    case 'step2':
      return 'complete';
     break;
  }
}

/**
 * Helper for setting the previous stage of processing on this multi-step form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   The name of the stage to be used in determining which form to display.
 */
function islandora_ead_utility_move_to_previous_stage($form, &$form_state) {
  switch ($form_state['stage']) {
    case 'step2':
      return 'step1';
     break;
  }
}

/**
 * Helper function to run an XSL transform on the provided $info['input'] file.
 *
 * @param array $info
 *   This array contains the xsl value and the input (xml) filenames.
 * @return string
 *   The content of the transformed result.
 */
function islandora_ead_utility_runXslTransform($info) {
  $xsl = new DOMDocument('1.0', 'utf-8');
  $xsl->load($info['xsl']);
  $input = new DOMDocument('1.0', 'utf-8');
  $input->loadXML($info['input']);

  $processor = new XSLTProcessor();
  $processor->importStylesheet($xsl);

  if (isset($info['php_functions'])) {
    // Register php functions limited to those provided in the
    // $info['php_functions'] value.
    $processor->registerPHPFunctions($info['php_functions']);
  } elseif (isset($info['php_functions_all']) && $info['php_functions_all']) {
    // No restriction on the functions that are registered in the xslt processor.
    $processor->registerPHPFunctions();
  }

  return $processor->transformToXML($input);
}

/**
 * Helper function to get the identifier value from any EAD xml.
 *
 * @param mixed $recordobj_or_xml
 *   Object or string representation of the XML.
 * @param integer $index
 *   The index of the extracted eadxml from the File/EAD.php parsing.
 * @return string
 *   The identifier value.
 */
function islandora_ead_utility_get_ead_mods_bib_id_from_record($recordobj_or_xml, $index = 0, $namespace) {
  $ead2mods_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/MARC21slim2MODS3-5.xsl';
  $modsxmlRecordExtract_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/modsxmlRecordExtract.xsl';
  $eadxmlRecordExtract_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/eadxmlRecordExtract.xsl';
  $bibIDgrabber_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/bibIDgrabber.xsl';

  $return_array = array();
  $temp_path = drupal_realpath(variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', file_directory_temp())));
  $filename = $temp_path . '/ead_record_' . $index . '.ead.xml';
  $tmp = file_put_contents($filename, $recordobj_or_xml);
  $eadxml_file = fopen($filename, "wb");

  if (is_object($recordobj_or_xml)) {
    $eadxml_findingaid_record = $recordobj_or_xml->toXML();
    fwrite($eadxml_file, $eadxml_findingaid_record);
    fclose($eadxml_file);

    $ead_record = ($eadxml_findingaid_record) ? islandora_ead_utility_runXslTransform(
              array(
                'xsl' => $eadxmlRecordExtract_xsl,
                'input' => $eadxml_findingaid_record,
              )
            ) : '';
  }
  else {
    $ead_record = $recordobj_or_xml;
    $eadxml_findingaid_record = $recordobj_or_xml;
  }

  $bib_id = ($eadxml_findingaid_record) ? islandora_ead_utility_runXslTransform(
            array(
              'xsl' => $bibIDgrabber_xsl,
              'input' => $eadxml_findingaid_record,
            )
          ) : '';
  // Delete the initial journal record collection file and save this using the
  // $bib_id value
  @unlink($filename);

  $filename = $temp_path . '/' . $namespace. '_'.  $bib_id . '_EAD.xml';

  $bytes_written = file_put_contents($filename, $ead_record);
  $return_array['bib_id'] = $bib_id;
  $return_array['EAD'] = $filename;

  $modsCollection = ($eadxml_findingaid_record) ? islandora_ead_utility_runXslTransform(
            array(
              'xsl' => $ead2mods_xsl,
              'input' => $eadxml_findingaid_record,
            )
          ) : '';
  $MODS = ($modsCollection) ? islandora_ead_utility_runXslTransform(
            array(
              'xsl' => $modsxmlRecordExtract_xsl,
              'input' => $modsCollection,
            )
          ) : '';

  $filename = $temp_path . '/' . $namespace. '_'.  $bib_id . '_MODS.xml';
  $bytes_written = file_put_contents($filename, $MODS);
  $return_array['MODS'] = $filename;

  return $return_array;
}

/**
 * Code to parse a EADXML format file.
 *
 * All parameters are optional aside from the $filepath.  When this is called
 * from another module, the configuration values for most of the functionality
 * behind linking an object and applying the datastream to the right object is
 * not handled here.  That module must know what to do with the EAD and MODS
 * xml files.
 *
 * @param string $filepath
 *   The path to the uploaded EAD Finding Aid file.
 * @param string $mods_identifier_solr_fieldname
 *   The value to search the existing Islandora repository for an object that
 * matches the EAD identifier value.
 * @param string $namespace
 *   The fedora namespace for the intended objects - ONLY for the purposes of
 * filenames that get created for export.  See "Islandora Datastreams Input/Output
 * Utility" https://github.com/ulsdevteam/islandora_datastreams_io for more info
 * on importing ZIP files.
 * @return array
 *   An associative array that contains info about the EAD and MODS files that
 * were able to be parsed out of the EAD Finding Aid.
 */
function islandora_ead_utility_eadxml_split($filepath, $mods_identifier_solr_fieldname = '', $namespace = '') {
  module_load_include('inc', 'islandora_ead_utility', 'includes/solr_query');

  $mods_identifier_solr_fieldname = (!$mods_identifier_solr_fieldname) ?
      variable_get('islandora_ead_utility_mods_solr_fieldname', EAD_UTIL_MODS_SOLR_FIELDNAME_PLACEHOLDER) : $mods_identifier_solr_fieldname;
  $namespace = (!$namespace) ?
      variable_get('islandora_ead_utility_namespace', 'islandora') : $namespace;
  $temp_path = drupal_realpath(variable_get('islandora_ead_utility_temp_path', variable_get('file_temporary_path', file_directory_temp())));

  $return_files = array();
  $eadxml = file_get_contents($filepath);
  // Use PHP's DOM Document to pull out each of the EADXML records.
  $doc = new DOMDocument('1.0', 'utf-8'); // DomDocument();
  $doc->loadXML($eadxml);

  // I think we want to transform the EAD xml into CSV - and then use the
  // CSV to MODS utility to turn that into MODS xml files.
  $eadxmlCheckIn_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/DAOcheckIn.xsl';
  $checkin_CSV_file = ($eadxml) ? islandora_ead_utility_runXslTransform(
          array(
            'xsl' => $eadxmlCheckIn_xsl,
            'input' => $eadxml,
          )
        ) : '[]';

  $eadxmlead2mods_xsl = drupal_realpath(drupal_get_path('module', 'islandora_ead_utility')) .
        '/transforms/ead2mods.xsl';
  $mods_files = ($eadxml) ? islandora_ead_utility_runXslTransform(
          array(
            'xsl' => $eadxmlead2mods_xsl,
            'input' => $eadxml,
          )
        ) : '';

  if ($mods_files) {
    $splits = explode('[[END]]', $mods_files);
    // Loop through all of the files -- denoted by the delimiter string of
    // "[[END]]".  For each string, if not present, add the
    //  "< ?xml version="1.0" encoding="utf-8"? >" to the top and take out the
    // line that contains "filename=" keeping the filename to save the MODS.
    foreach ($splits as $possible_mods_file) {
      $possible_mods_file = trim($possible_mods_file);
      if ($possible_mods_file) {
        $current_mods = (strstr($possible_mods_file, 'xml version')) ? $possible_mods_file : '<' . '?xml version="1.0" encoding="utf-8"?' . '>' . "\n" . $possible_mods_file;
        $lines = explode("\n", $current_mods);
        $bib_id = $current_filename = '';
        $recalc_mods_arr = array();
        foreach ($lines as $line) {
          if (!$current_filename && strstr($line, 'filename=')) {
            $current_filename = trim(str_replace("filename=", "", $line));
            $bib_id = str_replace(".mods.xml", "", $current_filename);
          }
          else {
            $recalc_mods_arr[] = $line;
          }
        }
        $recalc_mods = implode("\n", $recalc_mods_arr);
        if ($current_filename) {
          $full_filename = $temp_path . '/' . $current_filename;
          $return_files[] = array(
              'MODS size' => filesize($full_filename),
              'MODS' => $full_filename,
              'bib_id' => $bib_id,
              'Fedora_PID' => $namespace . ':' . $bib_id);
          file_put_contents($full_filename, $recalc_mods);
        }
      }
    }
  }
  return $return_files;
}

