<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @param type $eadxml_file
 * @param type $ead_identifier_xpath
 * @param type $mods_identifier_solr_fieldname
 * @return type
 */
function islandora_ead_utility_find_object_pid_from_eadxml_file($eadxml_file, $ead_identifier_xpath, $mods_identifier_solr_fieldname) {
  // 1. parse the eadxml file to get the configured EAD identifier value.
  //    Uses variable_get('islandora_ead_utility_ead_identifier_tag_xpath',
  //    '/ead:record/ead:datafield[tag="35"]/ead:subfield[code="a"]').
  $ead_identifier_value = islandora_ead_utility_get_ead_identifer_value(
      $eadxml_file, $ead_identifier_xpath, 'ead', 'http://www.loc.gov/ead/ead.xsd');

  $found_object_PID = NULL;
  if ($ead_identifier_value) {
    // 2. run a SOLR query to find the object that matches the configured field
    //    with that value.  Uses variable_get('islandora_ead_utility_mods_solr_fieldname',
    //    'mods_identifier_oclc_s'),
    $filters = array($mods_identifier_solr_fieldname . ':' . $ead_identifier_value);
    $found_object_PID = islandora_ead_utility_solr_query_for_object('PID', $filters, 1);
  }
  return $found_object_PID;
}

/**
 *
 * @param type $xml_content
 * @param type $xpath
 * @param type $namespace
 * @param type $namespace_uri
 * @return type
 */
function islandora_ead_utility_get_ead_identifer_value($xml_content, $xpath, $namespace = '', $namespace_uri = '') {
  $xml = new SimpleXMLElement($xml_content);
  if ($namespace && $namespace_uri) {
    $xml->registerXPathNamespace($namespace, $namespace_uri);
  }
  $match = '';
  $match_results = $xml->xpath($xpath);
  if (is_array($match_results) && count($match_results) > 0) {
    $matches = $match_results[0];
    $s = (string)$matches;
    // Sanitize that value to remove prefix, special characters, hoopla, etc.
    $match = str_replace(array(
        '(OCoLC)', '(WlCaITV)', ''
        ), "", $s);
  }
  return $match;
}

/**
 * Helper function to search Solr for any set of filter values and return ONLY
 * a single value.
 *
 * @param array $filters
 *   These should be in a legal Solr syntax and each have a syntax like:
 * {FIELD_NAME}:{FIELD_VALUE}
 * @param integer $rows
 *   How many rows to return.
 * @return mixed (string:NULL)
 *   The value that was found or NULL.
 */
function islandora_ead_utility_solr_query_for_object($field, $filters, $rows) {
  $value = NULL;
  $query_processor = new IslandoraSolrQueryProcessor();

  $query_processor->solrQuery = implode(" AND ", $filters);
  $query_processor->solrStart = 0;
  $query_processor->solrLimit = $rows;
  $query_processor->solrParams = array(
    'fl' => $field,
    'fq' => '',
  );
  $query_processor->executeQuery(FALSE);
  if ($query_processor->islandoraSolrResult['response']['numFound'] > 0) {
    $solr_results_doc = $query_processor->islandoraSolrResult['response']['objects']['0']['solr_doc'];
    $value = (isset($solr_results_doc[$field]) && is_array($solr_results_doc[$field])) ? (($single_result) ? $solr_results_doc[$field][0] : implode("<br>", $solr_results_doc[$field])) :
      (isset($solr_results_doc[$field]) ? $solr_results_doc[$field] : '');
  }
  return $value;
}