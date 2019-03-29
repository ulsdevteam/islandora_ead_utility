<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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