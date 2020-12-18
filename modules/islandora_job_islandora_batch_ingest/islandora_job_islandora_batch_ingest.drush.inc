<?php

/**
 * @file
 * Custom Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_job_islandora_batch_ingest_drush_command() {
  return array(
    'islandora_job_islandora_batch_ingest' => array(
      'aliases' => array('ijibi'),
      'description' => dt('Ingest a batch set with Islandora Jobs.'),
      'drupal dependencies' => array(
        'islandora',
        'islandora_job',
        'islandora_job_islandora_batch_ingest',
      ),
      'options' => array(
        'set_id' => array(
          'description' => dt('The batch set ID to process.'),
          'required' => TRUE,
        ),
        'force' => array(
          'description' => dt('Required to re-run a batch.'),
          'required' => FALSE,
        ),
      ),
      'examples' => array(
        'drush -u 1 -v ijibi --set_id=1' => dt('Sends batch set 1 to Islandoa Jobs for ingesting.'),
      ),
    ),
  );
}

/**
 * Drush command to add batch set to Gearman.
 */
function drush_islandora_job_islandora_batch_ingest() {
  module_load_include('inc', 'islandora_batch', 'includes/utilities');
  module_load_include('inc', 'islandora_batch', 'includes/db');
  $set_id = drush_get_option('set_id', 0);
  $force = drush_get_option('force', FALSE);
  $completed_sets = islandora_batch_get_sets(TRUE);

  if ($force == FALSE && in_array($set_id, $completed_sets)) {
    drush_print(t("Batch set (@s) is already ingested, use --force to re-ingest", array('@s' => $set_id)));
  }

  if (islandora_batch_set_exists($set_id)) {
    if (islandora_job_submit_background('islandora_job_islandora_batch_ingest_ingest_set', $set_id)) {
      drush_print(t("Batch set (@s) has been added to the queue.", array('@s' => $set_id)));
    }
    else {
      drush_print(t("Batch set (@s) could not be added to the queue, please consult your administrator.", array('@s' => $set_id)));
    }
  }
  else {
    drush_print(t("Batch set (@s) does not exist.", array('@s' => $set_id)));
  }
}
