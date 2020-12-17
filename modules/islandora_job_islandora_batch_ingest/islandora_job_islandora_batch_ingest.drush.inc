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
  $set_id = drush_get_option('set_id', 0);
  islandora_job_submit_background('islandora_job_islandora_batch_ingest_ingest_set', $set_id);
}
