<?php

/**
 * @file
 * Configuration form for the islandora_job module.
 */

/**
 * Form function to generate the configuration form.
 *
 * @param array $form
 *   Array of Drupal Form API elements.
 * @param array $form_state
 *   Array containing the form state.
 */
function islandora_job_admin_form(array $form, array &$form_state) {
  $form['islandora_job_server_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Hostname'),
    '#description' => t('Address to the job server'),
    '#required' => TRUE,
    '#default_value' => variable_get('islandora_job_server_host', 'localhost'),
  );
  $form['islandora_job_server_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Port'),
    '#description' => t('Port that the job server is listening on'),
    '#required' => TRUE,
    '#default_value' => variable_get('islandora_job_server_port', 4730),
  );
  $form['islandora_job_multisite_prefix'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable multisite prefix'),
    '#description' => t('Add a multisite prefix to job queue name'),
    '#default_value' => variable_get('islandora_job_multisite_prefix', FALSE),
  );
  return system_settings_form($form);
}
