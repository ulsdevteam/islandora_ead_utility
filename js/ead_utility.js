/**
 * @file
 * Javascript file for islandora_digital_workflow
 */


/*
 * This will enable / disable the prompts based on the checkbox checked state.
 * 
 * @param {type} control
 * @returns {undefined}
 */
function set_override_elements(control) {
  if (control.checked) {
    jQuery('#edit-ead-identifier-tag-xpath').removeAttr("disabled");
    jQuery('#edit-ead-identifier-tag-xpath').removeAttr("readonly");
    jQuery('.edit-ead-identifier-tag-xpath label').addClass("enabled_text");
    jQuery('#edit-ead-identifier-tag-xpath').removeClass("disabled_text");
    jQuery('.edit-ead-identifier-tag-xpath label').removeClass("disabled_text");
    jQuery('.form-item-ead-identifier-tag-xpath').removeClass("form-disabled");
    jQuery('#edit-mods-solr-fieldname').removeAttr("disabled");
    jQuery('#edit-mods-solr-fieldname').removeAttr("readonly");
    jQuery('.edit-mods-solr-fieldname label').addClass("enabled_text");
    jQuery('#edit-mods-solr-fieldname').removeClass("disabled_text");
    jQuery('.edit-mods-solr-fieldname label').removeClass("disabled_text");
    jQuery('.form-item-mods-solr-fieldname').removeClass("form-disabled");
    jQuery('#edit-namespace').removeAttr("disabled");
    jQuery('#edit-namespace').removeAttr("readonly");
    jQuery('.edit-namespace label').addClass("enabled_text");
    jQuery('#edit-namespace').removeClass("disabled_text");
    jQuery('.edit-namespace label').removeClass("disabled_text");
    jQuery('.form-item-namespace').removeClass("form-disabled");
  }
  else {
    jQuery('#edit-ead-identifier-tag-xpath').attr('disabled', 'disabled');
    jQuery('#edit-ead-identifier-tag-xpath').attr('readonly', 'readonly');
    jQuery('#edit-ead-identifier-tag-xpath').addClass("disabled_text");
    jQuery('.edit-ead-identifier-tag-xpath label').addClass("disabled_text");
    jQuery('.edit-ead-identifier-tag-xpath label').removeClass("enabled_text");
    jQuery('.form-item-ead-identifier-tag-xpath').addClass("form-disabled");
    jQuery('#edit-mods-solr-fieldname').attr('disabled', 'disabled');
    jQuery('#edit-mods-solr-fieldname').attr('readonly', 'readonly');
    jQuery('#edit-mods-solr-fieldname').addClass("disabled_text");
    jQuery('.edit-mods-solr-fieldname label').addClass("disabled_text");
    jQuery('.edit-mods-solr-fieldname label').removeClass("enabled_text");
    jQuery('.form-item-mods-solr-fieldname').addClass("form-disabled");
    jQuery('#edit-namespace').attr('disabled', 'disabled');
    jQuery('#edit-namespace').attr('readonly', 'readonly');
    jQuery('#edit-namespace').addClass("disabled_text");
    jQuery('.edit-namespace label').addClass("disabled_text");
    jQuery('.edit-namespace label').removeClass("enabled_text");
    jQuery('.form-item-namespace').addClass("form-disabled");
  }
}
